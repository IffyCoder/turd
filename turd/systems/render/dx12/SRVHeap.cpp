#include "pch.hpp"
#include "SRVHeap.hpp"
#include "core/Environment.hpp"

namespace turd
{
    SRVHeap::SRVHeap(uint32_t numDescriptors) : mMaxDescriptors(numDescriptors)
    {
        auto dx = GetEnvironment().gRenderSystem->DX();
        auto device = dx->GetD3DDevice();

        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.NumDescriptors = numDescriptors;
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        if (FAILED(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mHeap))))
        {
            throw RuntimeError("Failed to create descriptor heap");
        }

        mCbvSrvUavDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    SRVHeap::~SRVHeap() {}

    uint32_t SRVHeap::NumDescriptors() const { return mMaxDescriptors; }

    void SRVHeap::Add(uint32_t index, ComPtr<ID3D12Resource> resource, D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc)
    {
        if (index >= mMaxDescriptors)
        {
            return;
        }

        auto dx = GetEnvironment().gRenderSystem->DX();
        auto device = dx->GetD3DDevice();

        CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mHeap->GetCPUDescriptorHandleForHeapStart());
        hDescriptor.Offset(index, mCbvSrvUavDescriptorSize);

        device->CreateShaderResourceView(resource.Get(), &srvDesc, hDescriptor);
    }
} // namespace turd
