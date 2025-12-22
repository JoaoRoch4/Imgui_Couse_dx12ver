//#include "ExampleDescriptorHeapAllocator.hpp"
//#include "FrameContext.hpp"
//
//
//class MainWindow {
//
//public:
//    // Config for example app
//
//    std::unique_ptr<ExampleDescriptorHeapAllocator> g_pd3dSrvDescHeapAlloc;
//
//    MainWindow();
//    uint32_t WINDOW_WIDTH = std::uint32_t{ 2000 };
//    uint32_t WINDOW_HEIGHT = std::uint32_t{ 1000 };
//
//    UINT g_frameIndex = 0;
//    FrameContext g_frameContext[APP_NUM_FRAMES_IN_FLIGHT];
//
//    // Data
//
//
//    ComPtr<ID3D12Device> m_Device;
//    ComPtr<ID3D12DescriptorHeap> m_RtvDescHeap;
//    ComPtr<ID3D12DescriptorHeap> m_SrvDescHeap;
//    ComPtr<ID3D12CommandQueue> m_CommandQueue;
//    ComPtr<ID3D12GraphicsCommandList> m_CommandList;
//    ComPtr<ID3D12Fence> m_fence;
//    ComPtr<IDXGISwapChain3> m_pSwapChain;
//    ComPtr<ID3D12Resource> m_MainRenderTargetResource[APP_NUM_BACK_BUFFERS];
//    D3D12_CPU_DESCRIPTOR_HANDLE
//        m_MainRenderTargetDescriptor[APP_NUM_BACK_BUFFERS];
//
//    HANDLE m_FenceEvent;
//    UINT64 m_FenceLastSignaledValue = 0;
//    bool m_SwapChainTearingSupport = false;
//    bool m_SwapChainOccluded = false;
//    HANDLE m_hSwapChainWaitableObject = nullptr;
//
//
//    // Forward declarations of helper functions
//    bool CreateDeviceD3D(HWND hWnd);
//    void CleanupDeviceD3D();
//    void CreateRenderTarget();
//    void CleanupRenderTarget();
//    void WaitForPendingOperations();
//    FrameContext* WaitForNextFrameContext();
//
//    int launch(HINSTANCE& hInstance);
//
//
//    std::string HrToString(HRESULT hr);
//};
//
//// Forward declare message handler from imgui_impl_win32.cpp
//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
//    UINT msg,
//    WPARAM wParam,
//    LPARAM lParam);
