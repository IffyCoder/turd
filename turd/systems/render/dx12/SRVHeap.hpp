#pragma once

namespace turd
{
    class SRVHeap
    {
      public:
        SRVHeap(uint32_t numDescriptors);
        ~SRVHeap();

        void Add(uint32_t index, ComPtr<ID3D12Resource> resource, D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc);

        uint32_t NumDescriptors() const;
      private:
        ComPtr<ID3D12DescriptorHeap> mHeap;
        uint32_t mCbvSrvUavDescriptorSize = 0;
        uint32_t mMaxDescriptors = 0;
    };
}
