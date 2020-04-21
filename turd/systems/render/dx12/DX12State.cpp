#include "pch.hpp"
#include "DX12State.hpp"
#include "Frame.hpp"
#include "Adapter.hpp"

#include "core/Settings.hpp"

namespace turd
{
    DX12State::DX12State(HWND hWindow) : mWindow(hWindow)
    {
        auto &settings = GetSettings();

        if (settings.RendererSettings().VSync)
        {
            mSyncInterval = 1;
        }

        mFrames.reserve(NumFrames());

        CreateD3DDevice();
        CreateFrames();
        CreateCommandQueue();

        Resize();
    }
    DX12State::~DX12State()
    {
        FlushCommandQueue();
        mFrames.clear();
    }

    void DX12State::FlushCommandQueue()
    {
        auto frame = GetCurrentFrame();

        if (!frame)
        {
            return;
        }

        frame->Fence++;

        if (FAILED(mCommandQueue->Signal(mFence.Get(), frame->Fence)))
        {
            throw; // RuntimeError(_T("Failed to signal fence"));
        }

        if (mFence->GetCompletedValue() < frame->Fence)
        {
            HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
            if (!eventHandle)
            {
                throw;
            }

            if (FAILED(mFence->SetEventOnCompletion(frame->Fence, eventHandle)))
            {
                throw; // RuntimeError(_T("Failed to set event"));
            }

            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        }
    }

    Frame *DX12State::GetCurrentFrame() const
    {
        if (mFrames.size() == 0)
        {
            return nullptr;
        }
        return mFrames[mCurrentFrame].get();
    }

    void DX12State::ResetCommandList(Frame *frame)
    {
        if (FAILED(frame->ResetCommandList()))
        {
            throw; // RuntimeError(_T("Failed to reset command list"));
        }
    }

    void DX12State::ExecuteCommandList(Frame *frame)
    {
        if (FAILED(frame->CommandList->Close()))
        {
            throw; // RuntimeError(_T("Failed to close command list"));
        }

        ID3D12CommandList *cmdsLists[] = {frame->CommandList.Get()};
        mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
        FlushCommandQueue();
    }

    void DX12State::NextFrame()
    {
        auto frame = GetCurrentFrame();

        if (FAILED(frame->CommandList->Close()))
        {
            throw; // RuntimeError(_T("Failed to close command list"));
        }

        ID3D12CommandList *cmdsLists[] = {frame->CommandList.Get()};
        mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

        if (FAILED(mSwapChain->Present(mSyncInterval, 0)))
        {
            throw; // RuntimeError(_T("Failed to present"));
        }

        mCurrentFrame = mSwapChain->GetCurrentBackBufferIndex();

        FlushCommandQueue();
    }

    void DX12State::Resize()
    {
        FlushCommandQueue();

        for (UINT i = 0; i < NumFrames(); i++)
        {
            mFrames[i]->FrameBuffer.Reset();
            mFrames[i]->Fence = mFrames[mCurrentFrame]->Fence;
        }

        mDepthStencilBuffer.Reset();

        if (mSwapChain != nullptr)
        {
            auto size = GetSize();
            if (FAILED(mSwapChain->ResizeBuffers(NumFrames(), size.Width, size.Height, mBackBufferFormat,
                                                 DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH)))
            {
                throw; // RuntimeError(_T("Failed to resize swapchain buffers"));
            }
        }
        else
        {
            CreateSwapChain();
        }

        auto frame = GetCurrentFrame();
        if (FAILED(frame->ResetCommandList()))
        {
            throw; // RuntimeError(_T("Failed to reset command list"));
        }

        mCurrentFrame = 0;

        CreateDsvRtvHeaps();

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
        for (UINT i = 0; i < NumFrames(); i++)
        {
            ComPtr<ID3D12Resource> frameBuffer;
            if (FAILED(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&frameBuffer))))
            {
                throw; // RuntimeError(_T("Failed to get swapchain buffer"));
            }
            mFrames[i]->FrameBuffer = frameBuffer;
            mD3DDevice->CreateRenderTargetView(mFrames[i]->FrameBuffer.Get(), nullptr, rtvHeapHandle);
            rtvHeapHandle.Offset(1, mRtvDescriptorSize);
        }

        const auto size = GetSize();

        D3D12_RESOURCE_DESC depthStencilDesc;
        depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        depthStencilDesc.Alignment = 0;
        depthStencilDesc.Width = size.Width;
        depthStencilDesc.Height = size.Height;
        depthStencilDesc.DepthOrArraySize = 1;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.Format = mDepthStencilFormat;
        depthStencilDesc.SampleDesc.Count = mMSAASamples;
        depthStencilDesc.SampleDesc.Quality = mMSAAQuality;
        depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE optClear;
        optClear.Format = mDepthStencilFormat;
        optClear.DepthStencil.Depth = 1.0f;
        optClear.DepthStencil.Stencil = 0;

        if (FAILED(mD3DDevice->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &depthStencilDesc,
                D3D12_RESOURCE_STATE_COMMON, &optClear, IID_PPV_ARGS(&mDepthStencilBuffer))))
        {
            throw; // RuntimeError(_T("Failed to create depth stencil buffer"));
        }

        mD3DDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), nullptr, DepthStencilView());

        frame->CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(),
                                                                                     D3D12_RESOURCE_STATE_COMMON,
                                                                                     D3D12_RESOURCE_STATE_DEPTH_WRITE));

        if (FAILED(frame->CommandList->Close()))
        {
            throw; // RuntimeError(_T("Failed to close command list"));
        }

        ID3D12CommandList *cmdsLists[] = {frame->CommandList.Get()};
        mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

        FlushCommandQueue();

        mScreenViewport.TopLeftX = 0;
        mScreenViewport.TopLeftY = 0;
        mScreenViewport.Width = static_cast<float>(size.Width);
        mScreenViewport.Height = static_cast<float>(size.Height);
        mScreenViewport.MinDepth = 0.0f;
        mScreenViewport.MaxDepth = 1.0f;

        mScissorRect = {0, 0, LONG(size.Width), LONG(size.Height)};
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DX12State::DepthStencilView() const
    {
        return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DX12State::CurrentBackBufferView() const
    {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrentFrame,
                                             mRtvDescriptorSize);
    }

    void DX12State::CreateSwapChain()
    {
        IDXGISwapChain *pSwapChain;

        Size s = GetSize();
        auto &settings = GetSettings();

        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferDesc.Width = s.Width;
        sd.BufferDesc.Height = s.Height;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferDesc.Format = mBackBufferFormat;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        sd.SampleDesc.Count = mMSAASamples;
        sd.SampleDesc.Quality = mMSAAQuality;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = NumFrames();
        sd.OutputWindow = mWindow;
        sd.Windowed = !settings.WindowSettings().Fullscreen;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        if (FAILED(Adapter::Factory()->CreateSwapChain(mCommandQueue.Get(), &sd, &pSwapChain)))
        {
            throw; // RuntimeError(_T("Failed to create swapchain"));
        }

        if (FAILED(pSwapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void **)&mSwapChain)))
        {
            throw; // RuntimeError(_T("Failed to create swapchain"));
        }

        pSwapChain->Release();
    }

    void DX12State::CreateCommandQueue()
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        if (FAILED(mD3DDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue))))
        {
            throw; // RuntimeError(_T("Failed to create command queue"));
        }
    }

    void DX12State::CreateFrames()
    {
        mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

        if (mFenceEvent == nullptr)
        {
            if (FAILED(HRESULT_FROM_WIN32(GetLastError())))
            {
                throw; // RuntimeError(_T("Failed to create fence event"));
            }
        }

        for (UINT n = 0; n < NumFrames(); n++)
        {
            mFrames.push_back(std::make_unique<Frame>(mD3DDevice.Get()));
        }

        if (FAILED(mD3DDevice->CreateFence(GetCurrentFrame()->Fence, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence))))
        {
            throw; // RuntimeError(_T("Failed to create fence"));
        }
        GetCurrentFrame()->Fence++;
    }

    void DX12State::CreateDsvRtvHeaps()
    {
        mRtvDescriptorSize = mD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        mDsvDescriptorSize = mD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

        mRtvHeap.Reset();
        mDsvHeap.Reset();

        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = NumFrames();
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        if (FAILED(mD3DDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap))))
        {
            throw; // RuntimeError(_T("Failed to create render target view heap"));
        }

        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 2;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        if (FAILED(mD3DDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDsvHeap))))
        {
            throw; // RuntimeError(_T("Failed to create depth stencil view heap"));
        }
    }

    void DX12State::CreateD3DDevice()
    {
#ifdef _DEBUG
        {
            ComPtr<ID3D12Debug> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
            {
                debugController->EnableDebugLayer();
            }
        }
#endif
        auto pAdapter = Adapter::Get();
        if (FAILED(D3D12CreateDevice(pAdapter, Adapter::GetFeatureLevel(), IID_PPV_ARGS(&mD3DDevice))))
        {
            throw; // RuntimeError(_T("Failed to create DirectX device"));
        }

        D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
        msQualityLevels.Format = mBackBufferFormat;
        msQualityLevels.SampleCount = mMSAASamples;
        msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
        msQualityLevels.NumQualityLevels = 0;

        if (FAILED(mD3DDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels,
                                                   sizeof(msQualityLevels))))
        {
            throw; // RuntimeError(_T("Failed to check feature support"));
        }

        mMSAAQuality = msQualityLevels.NumQualityLevels - 1;
        if (mMSAAQuality == 0)
        {
            mMSAASamples = 1;
        }
    }

    Size DX12State::GetSize() const
    {
        RECT r;
        GetClientRect(mWindow, &r);
        return {uint32_t(r.right - r.left), uint32_t(r.bottom - r.top)};
    }
} // namespace turd
