#include "pch.hpp"
#include "Pipeline.hpp"
#include "ConstantBuffer.hpp"
#include "core/ByteBuffer.hpp"
#include "core/Environment.hpp"
#include "core/IO.hpp"

#include "yaml-cpp/yaml.h"

#include <d3dcompiler.h>
#include <d3d12shader.h>

#pragma comment(lib, "d3dcompiler.lib")

namespace turd
{
    struct PipelineInclude : public ID3DInclude
    {
        HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
        {
            if (IncludeType != D3D_INCLUDE_LOCAL)
            {
                E("Loading system wide shaders is not supported");
                return E_FAIL;
            }

            auto buf = IO::File(pFileName);
            if (buf->Empty())
            {
                *ppData = nullptr;
                *pBytes = 0;
                E("Nothing read from %s", pFileName);
                return E_FAIL;
            }

            auto temp = new uint8_t[buf->Size()];
            auto err = ::memcpy_s(temp, buf->Size(), buf->Pointer(), buf->Size());
            if (err != 0)
            {
                delete[] temp;
                E("Failed to copy shader %s", pFileName);
                return E_FAIL;
            }

            *ppData = temp;
            *pBytes = buf->Size();

            return S_OK;
        }

        HRESULT Close(LPCVOID pData)
        {
            if (!pData)
            {
                return E_FAIL;
            }
            delete[] pData;
            return S_OK;
        }
    };

    static PipelineInclude gShaderInclude;
    using Defines = std::vector<D3D_SHADER_MACRO>;
    enum class ShaderStage
    {
        Vertex,
        Pixel,
        Geometry,
        Domain,
        Hull
    };

    ShaderStage ToVal(const std::string &name)
    {
        if (name.compare("vertex") == 0)
        {
            return ShaderStage::Vertex;
        }
        if (name.compare("pixel") == 0)
        {
            return ShaderStage::Pixel;
        }
        if (name.compare("geometry") == 0)
        {
            return ShaderStage::Geometry;
        }
        if (name.compare("domain") == 0)
        {
            return ShaderStage::Domain;
        }
        if (name.compare("hull") == 0)
        {
            return ShaderStage::Hull;
        }
        throw;
    }

    std::string ToProfile(ShaderStage s)
    {
        switch (s)
        {
        case ShaderStage::Vertex:
            return "vs_5_0";
        case ShaderStage::Pixel:
            return "ps_5_0";
        case ShaderStage::Geometry:
            return "gs_5_0";
        case ShaderStage::Domain:
            return "ds_5_0";
        case ShaderStage::Hull:
            return "hs_5_0";
        }
        throw;
    }

    HRESULT CompileShader(_In_ const std::string &srcFile, _In_ const std::string &profile, _In_ const Defines &defines,
                          _Outptr_ ComPtr<ID3DBlob> &blob)
    {
        if (srcFile.empty() || profile.empty() || !blob)
        {
            return E_INVALIDARG;
        }

        blob = nullptr;

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
        flags |= D3DCOMPILE_DEBUG;
#endif

        ComPtr<ID3DBlob> shaderBlob;
        ComPtr<ID3DBlob> errorBlob;

        auto buf = IO::File(srcFile);

        HRESULT hr = D3DCompile(buf->Pointer(), buf->Size(), srcFile.c_str(), defines.data(), &gShaderInclude, "main",
                                profile.c_str(), flags, 0, &shaderBlob, &errorBlob);
        if (FAILED(hr))
        {
            if (errorBlob)
            {
                E((char *)errorBlob->GetBufferPointer());
            }

            return hr;
        }

        blob = shaderBlob;

        return hr;
    }

    void ParseDefines(YAML::Node &node, Defines &defines)
    {
        if (node)
        {
            if (!node.IsMap())
            {
                E("Expected map type");
                throw;
            }

            for (auto it = node.begin(); it != node.end(); ++it)
            {
                defines.push_back({it->first.as<std::string>().c_str(), it->second.as<std::string>().c_str()});
            }
        }

        defines.push_back({nullptr, nullptr});
    }

    HRESULT CreatePSO(std::map<ShaderStage, ComPtr<ID3DBlob>> compiledShaders,
                      std::vector<D3D12_INPUT_ELEMENT_DESC> &inputLayout, ID3D12RootSignature *rootSignature,
                      ComPtr<ID3D12PipelineState> &pso)
    {
        auto &env = GetEnvironment();
        auto dx = env.gRenderSystem->DX();
        auto device = dx->GetD3DDevice();

        if (!(CONTAINS(compiledShaders, ShaderStage::Vertex) || CONTAINS(compiledShaders, ShaderStage::Pixel)))
        {
            return E_INVALIDARG;
        }

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

        psoDesc.InputLayout = {inputLayout.data(), uint32_t(inputLayout.size())};
        psoDesc.pRootSignature = rootSignature;
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(compiledShaders[ShaderStage::Vertex].Get());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(compiledShaders[ShaderStage::Pixel].Get());

        if (CONTAINS(compiledShaders, ShaderStage::Geometry))
        {
            psoDesc.GS = CD3DX12_SHADER_BYTECODE(compiledShaders[ShaderStage::Geometry].Get());
        }

        if (CONTAINS(compiledShaders, ShaderStage::Domain))
        {
            psoDesc.DS = CD3DX12_SHADER_BYTECODE(compiledShaders[ShaderStage::Domain].Get());
        }

        if (CONTAINS(compiledShaders, ShaderStage::Hull))
        {
            psoDesc.HS = CD3DX12_SHADER_BYTECODE(compiledShaders[ShaderStage::Hull].Get());
        }

        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = dx->GetBackbufferFormat();
        psoDesc.SampleDesc.Count = 1;
        psoDesc.SampleDesc.Quality = 0;
        psoDesc.DSVFormat = dx->GetDepthStencilFormat();

        return device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));
    }

    Pipeline::Pipeline(const std::string &spec)
    {
        mInputLayout = {{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
                         D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                        {"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
                         D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                        {"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 24,
                         D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                        {"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,
                         D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};

        CreateRootSignature();
        Compile(spec);
    }

    std::string Pipeline::Name() const { return mName; }

    ConstantBuffer *Pipeline::Buffer(const std::string &name)
    {
        if (!CONTAINS(mBuffers, name))
        {
            return nullptr;
        }
        return mBuffers[name].get();
    }

    void Pipeline::Compile(const std::string &spec)
    {
        auto buf = IO::File(spec);
        auto root = YAML::Load(buf->ToString());
        auto pipeline = root["pipeline"];

        mName = pipeline["name"].as<std::string>();

        auto shaders = pipeline["shaders"];
        std::map<ShaderStage, ComPtr<ID3DBlob>> compiledShaders;

        for (auto it = shaders.begin(); it != shaders.end(); ++it)
        {
            auto stage = ToVal(it->first.as<std::string>());

            auto shaderInfo = it->second;
            Defines defines;
            ParseDefines(shaderInfo["defines"], defines);

            auto shaderFile = shaderInfo["file"].as<std::string>();

            ComPtr<ID3DBlob> blob;
            std::string profile = ToProfile(stage);

            auto hr = CompileShader(shaderFile, profile, defines, blob);
            if (FAILED(hr))
            {
                throw;
            }

            compiledShaders[stage] = blob;
        }

        HRESULT hr;

        ComPtr<ID3D12ShaderReflection> vsReflector;
        hr =
            D3DReflect(compiledShaders[ShaderStage::Vertex]->GetBufferPointer(),
                       compiledShaders[ShaderStage::Vertex]->GetBufferSize(), IID_ID3D12ShaderReflection, &vsReflector);
        D3D12_SHADER_DESC vsDesc;
        hr = vsReflector->GetDesc(&vsDesc);

        for (UINT i = 0; i < vsDesc.ConstantBuffers; i++)
        {
            auto bufferReflect = vsReflector->GetConstantBufferByIndex(i);
            auto ptr = std::make_unique<ConstantBuffer>(bufferReflect);
            if (CONTAINS(mBuffers, ptr->Name()))
            {
                E("Constant buffer %s already exists", ptr->Name().c_str());
                continue;
            }
            mBuffers[ptr->Name()] = std::move(ptr);
        }

        ComPtr<ID3D12ShaderReflection> psReflector;
        hr = D3DReflect(compiledShaders[ShaderStage::Pixel]->GetBufferPointer(),
                        compiledShaders[ShaderStage::Pixel]->GetBufferSize(), IID_ID3D12ShaderReflection, &psReflector);
        D3D12_SHADER_DESC psDesc;
        hr = psReflector->GetDesc(&psDesc);

        for (UINT i = 0; i < psDesc.ConstantBuffers; i++)
        {
            auto bufferReflect = psReflector->GetConstantBufferByIndex(i);
            auto ptr = std::make_unique<ConstantBuffer>(bufferReflect);
            if (CONTAINS(mBuffers, ptr->Name()))
            {
                E("Constant buffer %s already exists", ptr->Name().c_str());
                continue;
            }
            mBuffers[ptr->Name()] = std::move(ptr);
        }

        if (FAILED(CreatePSO(compiledShaders, mInputLayout, mRootSignature.Get(), mState)))
        {
            throw;
        }
    }

    void Pipeline::CreateRootSignature()
    {
        auto &env = GetEnvironment();
        auto dx = env.gRenderSystem->DX();
        auto device = dx->GetD3DDevice();

        // Texture table for cube map
        CD3DX12_DESCRIPTOR_RANGE texTable0;
        texTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);

        // Texture table for other textures
        CD3DX12_DESCRIPTOR_RANGE texTable1;
        texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, dx->MaxTextures(), 1, 0);

        CD3DX12_ROOT_PARAMETER slotRootParameter[5] = {};
        slotRootParameter[RootSlot::OBJECTS].InitAsConstantBufferView(0);
        slotRootParameter[RootSlot::PASS].InitAsConstantBufferView(1);
        slotRootParameter[RootSlot::MATERIALS].InitAsShaderResourceView(0, 1);
        slotRootParameter[RootSlot::CUBEMAP].InitAsDescriptorTable(1, &texTable0, D3D12_SHADER_VISIBILITY_PIXEL);
        slotRootParameter[RootSlot::TEXTURES].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_PIXEL);

        auto staticSamplers = GetStaticSamplers();

        CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(5, slotRootParameter, (uint32_t)staticSamplers.size(),
                                                staticSamplers.data(),
                                                D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ID3DBlob *pSerializedRootSig = nullptr;
        ID3DBlob *pErrorBlob = nullptr;

        Scoped cleaner([&]() {
            if (pSerializedRootSig)
            {
                pSerializedRootSig->Release();
            }

            if (pErrorBlob)
            {
                pErrorBlob->Release();
            }
        });

        if (FAILED(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSerializedRootSig,
                                               &pErrorBlob)))
        {
            if (pErrorBlob != nullptr)
            {
                throw; // RuntimeError(Tostd::string((char *)pErrorBlob->GetBufferPointer()));
            }
            else
            {
                throw; // RuntimeError(_T("Failed to serialize root signature"));
            }
        }

        auto hr = device->CreateRootSignature(0, pSerializedRootSig->GetBufferPointer(),
                                              pSerializedRootSig->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));
        if (FAILED(hr))
        {
            throw; // RuntimeError(_T("Failed to create root signature"));
        }
    }

    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> Pipeline::GetStaticSamplers()
    {
        const CD3DX12_STATIC_SAMPLER_DESC pointWrap(0,                                // shaderRegister
                                                    D3D12_FILTER_MIN_MAG_MIP_POINT,   // filter
                                                    D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
                                                    D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
                                                    D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

        const CD3DX12_STATIC_SAMPLER_DESC pointClamp(1,                                 // shaderRegister
                                                     D3D12_FILTER_MIN_MAG_MIP_POINT,    // filter
                                                     D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
                                                     D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
                                                     D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

        const CD3DX12_STATIC_SAMPLER_DESC linearWrap(2,                                // shaderRegister
                                                     D3D12_FILTER_MIN_MAG_MIP_LINEAR,  // filter
                                                     D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
                                                     D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
                                                     D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

        const CD3DX12_STATIC_SAMPLER_DESC linearClamp(3,                                 // shaderRegister
                                                      D3D12_FILTER_MIN_MAG_MIP_LINEAR,   // filter
                                                      D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
                                                      D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
                                                      D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

        const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(4,                               // shaderRegister
                                                          D3D12_FILTER_ANISOTROPIC,        // filter
                                                          D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
                                                          D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
                                                          D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressW
                                                          0.0f,                            // mipLODBias
                                                          8);                              // maxAnisotropy

        const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(5,                                // shaderRegister
                                                           D3D12_FILTER_ANISOTROPIC,         // filter
                                                           D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressU
                                                           D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressV
                                                           D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressW
                                                           0.0f,                             // mipLODBias
                                                           8);                               // maxAnisotropy

        const CD3DX12_STATIC_SAMPLER_DESC shadow(6,                                                // shaderRegister
                                                 D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
                                                 D3D12_TEXTURE_ADDRESS_MODE_CLAMP,                 // addressU
                                                 D3D12_TEXTURE_ADDRESS_MODE_CLAMP,                 // addressV
                                                 D3D12_TEXTURE_ADDRESS_MODE_CLAMP,                 // addressW
                                                 0.0f,                                             // mipLODBias
                                                 16,                                               // maxAnisotropy
                                                 D3D12_COMPARISON_FUNC_LESS_EQUAL,
                                                 D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

        return {pointWrap, pointClamp, linearWrap, linearClamp, anisotropicWrap, anisotropicClamp, shadow};
    }
} // namespace turd
