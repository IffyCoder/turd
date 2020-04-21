#include "pch.hpp"
#include "RenderSystem.hpp"
#include "core/Environment.hpp"
#include "core/Window.hpp"
#include "dx12/Frame.hpp"

namespace turd
{
    RenderSystem::RenderSystem(Window *pWindow) : mWindow(pWindow)
    {
        mDX = std::make_unique<DX12State>(mWindow->mHandle);
        GetEnvironment().gEventBus->OnResize([&](auto &e) {
            D("Resized to %dx%d", e.Width, e.Height);
            mDX->Resize();
        });
    }

    RenderSystem::~RenderSystem() { mDX->FlushCommandQueue(); }

    std::string RenderSystem::Name() { return "RenderSystem"; }

    void RenderSystem::Update(const float dt)
    {
        auto frame = mDX->GetCurrentFrame();
        auto cmdListAlloc = frame->CommandAllocator;
        auto cmdList = frame->CommandList;

        if (FAILED(cmdListAlloc->Reset()))
        {
            E("Failed to reset command list allocator");
        }

        if (FAILED(cmdList->Reset(cmdListAlloc.Get(), nullptr)))
        {
            E("Failed to reset command list");
            throw;
        }

        cmdList->RSSetViewports(1, &mDX->GetViewPort());
        cmdList->RSSetScissorRects(1, &mDX->GetScissorRect());

        frame->Clear(mDX->CurrentBackBufferView(), mDX->DepthStencilView());

        // TODO: Rendering happens here

        frame->Present();

        mDX->NextFrame();
    }
} // namespace turd
