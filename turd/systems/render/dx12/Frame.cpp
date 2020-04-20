#include "pch.hpp"
#include "Frame.hpp"

namespace turd
{
    static DirectX::XMVECTORF32 ClearColor = {0.662745118f / 2.0f, 0.662745118f / 2.0f, 0.662745118f / 2.0f,
                                              1.000000000f};

    Frame::Frame(ID3D12Device *pDevice)
    {
        if (FAILED(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator))))
        {
            throw; // RuntimeError(_T("Failed to create command allocator for frame"));
        }
        if (FAILED(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator.Get(), nullptr,
                                              IID_PPV_ARGS(&CommandList))))
        {
            throw; // RuntimeError(_T("Failed to create command list for frame"));
        }
        CommandList->Close();
    }

    HRESULT Frame::ResetCommandList(ID3D12PipelineState *state)
    {
        return CommandList->Reset(CommandAllocator.Get(), state);
    }

    void Frame::Clear(D3D12_CPU_DESCRIPTOR_HANDLE backBufferView, D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView)
    {
        // Indicate a state transition on the resource usage.
        CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(FrameBuffer.Get(),
                                                                              D3D12_RESOURCE_STATE_PRESENT,
                                                                              D3D12_RESOURCE_STATE_RENDER_TARGET));

        /*
         * Clear the back buffer and depth buffer.
         */
        CommandList->ClearRenderTargetView(backBufferView, ClearColor, 0, nullptr);
        CommandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0,
                                           0, nullptr);

        CommandList->OMSetRenderTargets(1, &backBufferView, true, &depthStencilView);
    }

    void Frame::Present()
    {
        CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(FrameBuffer.Get(),
                                                                              D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                                              D3D12_RESOURCE_STATE_PRESENT));
    }
} // namespace turd
