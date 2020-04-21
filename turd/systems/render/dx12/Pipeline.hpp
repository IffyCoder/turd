#pragma once

namespace turd
{
    class Pipeline
    {
      public:
        Pipeline(const std::string &spec) {}

        ~Pipeline() {}

      private:
        std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
        ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
        ComPtr<ID3D12PipelineState> mState = nullptr;
    };
} // namespace turd
