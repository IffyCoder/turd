#pragma once

namespace turd
{
    class Frame;

    class DX12State
    {
      public:
        DX12State(HWND hWindow);

        ~DX12State();

        const uint32_t NumFrames() const { return 3; }

        void FlushCommandQueue();

        void Resize();

        void MoveToNextFrame();

        void ResetCommandList(Frame *frame);

        void ExecuteCommandList(Frame *frame);

        D3D12_VIEWPORT GetViewPort() { return mScreenViewport; }
        D3D12_RECT GetScissorRect() { return mScissorRect; }
        uint32_t GetDSVDescriptorSize() { return mDsvDescriptorSize; }
        ID3D12DescriptorHeap *GetDSVHeap() { return mDsvHeap.Get(); }
        ID3D12Device *GetD3DDevice() const { return mD3DDevice.Get(); }
        ID3D12CommandQueue *GetCommandQueue() const { return mCommandQueue.Get(); }
        DXGI_FORMAT GetBackbufferFormat() const { return mBackBufferFormat; }
        DXGI_FORMAT GetDepthStencilFormat() const { return mDepthStencilFormat; }
        D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;
        D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
        Frame *GetCurrentFrame() const;

      private:
        void CreateSwapChain();
        void CreateCommandQueue();
        void CreateFrames();
        void CreateDsvRtvHeaps();
        void CreateD3DDevice();

        Size GetSize() const;

        int32_t mCurrentFrame = 0;
        std::vector<std::unique_ptr<Frame>> mFrames;
        ComPtr<ID3D12Device> mD3DDevice = nullptr;
        ComPtr<ID3D12Resource> mDepthStencilBuffer = nullptr;
        ComPtr<IDXGISwapChain3> mSwapChain = nullptr;
        ComPtr<ID3D12DescriptorHeap> mRtvHeap = nullptr;
        ComPtr<ID3D12CommandQueue> mCommandQueue = nullptr;
        ComPtr<ID3D12Fence> mFence = nullptr;
        ComPtr<ID3D12DescriptorHeap> mDsvHeap = nullptr;
        uint32_t mRtvDescriptorSize = 0;
        const DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        const DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        D3D12_VIEWPORT mScreenViewport = {};
        D3D12_RECT mScissorRect = {};
        HANDLE mFenceEvent = nullptr;
        uint32_t mDsvDescriptorSize = 0;
        uint32_t mSyncInterval = 0;
        uint32_t mMSAAQuality = 0;
        uint32_t mMSAASamples = 4;
        HWND mWindow;
    };

} // namespace turd
