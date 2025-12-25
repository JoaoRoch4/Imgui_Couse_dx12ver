#pragma once

#include "PCH.hpp"
#include "Globals.hpp"
#include "FrameContext.hpp"
#include "ExampleDescriptorHeapAllocator.hpp"

/**
 * @brief DirectX 12 Renderer class that manages all D3D12 resources
 * 
 * This class encapsulates all DirectX 12 objects and operations including:
 * - Device and command queue management
 * - Swap chain and render targets
 * - Descriptor heaps
 * - Synchronization primitives
 * - Frame context management
 */
class DX12Renderer {
public:
    DX12Renderer();
    ~DX12Renderer();

    // Initialization and cleanup
    bool CreateDeviceD3D(HWND hWnd, app::ExampleDescriptorHeapAllocator* heapAlloc);
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();

    // Frame management
    app::FrameContext* WaitForNextFrameContext();
    void WaitForPendingOperations();

    // Getters for D3D12 resources
    ID3D12Device* GetDevice() const { return m_Device.get(); }
    ID3D12CommandQueue* GetCommandQueue() const { return m_CommandQueue.get(); }
    ID3D12GraphicsCommandList* GetCommandList() const { return m_CommandList.get(); }
    IDXGISwapChain3* GetSwapChain() const { return m_pSwapChain.get(); }
    ID3D12DescriptorHeap* GetSrvDescHeap() const { return m_SrvDescHeap.get(); }
    ID3D12Resource* GetRenderTarget(UINT index) const { return m_mainRenderTargetResource[index].get(); }
    D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetDescriptor(UINT index) const { return m_mainRenderTargetDescriptor[index]; }
    ID3D12Fence* GetFence() const { return m_fence.get(); }
    HANDLE GetFenceEvent() const { return m_fenceEvent; }
    
    // Synchronization state
    UINT64& GetFenceLastSignaledValue() { return m_fenceLastSignaledValue; }
    UINT& GetFrameIndex() { return m_frameIndex; }
    bool GetSwapChainOccluded() const { return m_SwapChainOccluded; }
    void SetSwapChainOccluded(bool occluded) { m_SwapChainOccluded = occluded; }
    bool GetSwapChainTearingSupport() const { return m_SwapChainTearingSupport; }
    HANDLE GetSwapChainWaitableObject() const { return m_hSwapChainWaitableObject; }

private:
    // D3D12 Core objects
    ComPtr<ID3D12Device> m_Device;
    ComPtr<ID3D12DescriptorHeap> m_RtvDescHeap;
    ComPtr<ID3D12DescriptorHeap> m_SrvDescHeap;
    ComPtr<ID3D12CommandQueue> m_CommandQueue;
    ComPtr<ID3D12GraphicsCommandList> m_CommandList;
    ComPtr<ID3D12Fence> m_fence;
    ComPtr<IDXGISwapChain3> m_pSwapChain;
    
    // Render targets
    ComPtr<ID3D12Resource> m_mainRenderTargetResource[APP_NUM_BACK_BUFFERS];
    D3D12_CPU_DESCRIPTOR_HANDLE m_mainRenderTargetDescriptor[APP_NUM_BACK_BUFFERS];
    
    // Frame contexts
    app::FrameContext m_frameContext[APP_NUM_FRAMES_IN_FLIGHT];
    
    // Synchronization
    HANDLE m_fenceEvent;
    UINT64 m_fenceLastSignaledValue;
    UINT m_frameIndex;
    
    // Swap chain state
    bool m_SwapChainTearingSupport;
    bool m_SwapChainOccluded;
    HANDLE m_hSwapChainWaitableObject;
};
