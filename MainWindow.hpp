#include "ExampleDescriptorHeapAllocator.hpp"
#include "FontManager.hpp"
#include "FrameContext.hpp"
#include "render.hpp"
#include "Globals.hpp"
#include "WindowManager.hpp"


// Data
static FrameContext g_frameContext[APP_NUM_FRAMES_IN_FLIGHT] = {};
static UINT g_frameIndex = 0;

std::unique_ptr<ExampleDescriptorHeapAllocator> g_pd3dSrvDescHeapAlloc;

ComPtr<ID3D12Device> m_Device;
ComPtr<ID3D12DescriptorHeap> m_RtvDescHeap;
ComPtr<ID3D12DescriptorHeap> m_SrvDescHeap;
ComPtr<ID3D12CommandQueue> m_CommandQueue;
ComPtr<ID3D12GraphicsCommandList> m_CommandList;
ComPtr<ID3D12Fence> m_fence;
ComPtr<IDXGISwapChain3> m_pSwapChain;
ComPtr<ID3D12Resource> m_mainRenderTargetResource[APP_NUM_BACK_BUFFERS];
static D3D12_CPU_DESCRIPTOR_HANDLE
	m_mainRenderTargetDescriptor[APP_NUM_BACK_BUFFERS] = {};

static HANDLE m_fenceEvent;
static UINT64 m_fenceLastSignaledValue = 0;
static bool m_SwapChainTearingSupport = false;
static bool m_SwapChainOccluded = false;
static HANDLE m_hSwapChainWaitableObject = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void WaitForPendingOperations();
FrameContext *WaitForNextFrameContext();

_Use_decl_annotations_ int WINAPI wWinMain(_In_ HINSTANCE hInstance,
                                           _In_opt_ HINSTANCE hPrevInstance,
                                           _In_ LPWSTR lpCmdLine,
                                           _In_ int nCmdShow);
