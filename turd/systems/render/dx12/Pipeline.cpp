#include "pch.hpp"
#include "Pipeline.hpp"
#include "core/ByteBuffer.hpp"
#include "core/IO.hpp"

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

    HRESULT CompileShader(_In_ LPCSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob **blob)
    {
        if (!srcFile || !entryPoint || !profile || !blob)
            return E_INVALIDARG;

        *blob = nullptr;

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
        flags |= D3DCOMPILE_DEBUG;
#endif

        const D3D_SHADER_MACRO defines[] = {"EXAMPLE_DEFINE", "1", NULL, NULL};

        ID3DBlob *shaderBlob = nullptr;
        ID3DBlob *errorBlob = nullptr;
        HRESULT hr = D3DCompile(nullptr, 0, srcFile, defines, &gShaderInclude, entryPoint, profile, flags, 0,
                                &shaderBlob, &errorBlob);
        if (FAILED(hr))
        {
            if (errorBlob)
            {
                E((char *)errorBlob->GetBufferPointer());
                errorBlob->Release();
            }

            if (shaderBlob)
                shaderBlob->Release();

            return hr;
        }

        *blob = shaderBlob;

        return hr;
    }
} // namespace turd
