#pragma once

namespace turd
{
    class Frame
    {
      public:
        Frame(ID3D12Device *pDevice);
        ~Frame() = default;

        HRESULT ResetCommandList(ID3D12PipelineState *state = nullptr);

        void Clear(D3D12_CPU_DESCRIPTOR_HANDLE backBufferView, D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView);

        void Present();

        uint64_t Fence = 0;

        ComPtr<ID3D12CommandAllocator> CommandAllocator = nullptr;
        ComPtr<ID3D12GraphicsCommandList> CommandList = nullptr;
        ComPtr<ID3D12Resource> FrameBuffer = nullptr;
    };
} // namespace turd
