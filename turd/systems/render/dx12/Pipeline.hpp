#pragma once

namespace turd
{
    class ConstantBuffer;

    class Pipeline
    {
      public:
        Pipeline(const std::string &spec);

        ~Pipeline() = default;

        enum RootSlot : uint32_t
        {
            OBJECTS = 0,
            PASS = 1,
            MATERIALS = 2,
            CUBEMAP = 3,
            TEXTURES = 4
        };

        std::string Name() const;

        ConstantBuffer *Buffer(const std::string &name);

      private:
        void Compile(const std::string &spec);
        void CreateRootSignature();
        std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers();

        std::string mName;

        std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
        ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
        ComPtr<ID3D12PipelineState> mState = nullptr;

        std::map<std::string, std::unique_ptr<ConstantBuffer>> mBuffers;
    };
} // namespace turd
