#include "PCH.hpp"
#include "DX12Renderer.hpp"
#include "FrameContext.hpp"
#include "ExampleDescriptorHeapAllocator.hpp"

namespace app {

DX12Renderer::DX12Renderer() :
    m_fenceEvent(nullptr),
    m_fenceLastSignaledValue(0),
    m_frameIndex(0),
    m_SwapChainTearingSupport(false),
    m_SwapChainOccluded(false),
    m_hSwapChainWaitableObject(nullptr) {
    
    ZeroMemory(m_mainRenderTargetDescriptor, sizeof(m_mainRenderTargetDescriptor));
}

DX12Renderer::~DX12Renderer() {
    CleanupDeviceD3D();
}

bool DX12Renderer::CreateDeviceD3D(HWND hWnd, ExampleDescriptorHeapAllocator* heapAlloc) {
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC1 sd;
    {
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = APP_NUM_BACK_BUFFERS;
        sd.Width = 0;
        sd.Height = 0;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Scaling = DXGI_SCALING_NONE;
        sd.Stereo = FALSE;
    }

    // Enable debug layer
#ifdef DX12_ENABLE_DEBUG_LAYER
    ID3D12Debug* pdx12Debug = nullptr;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
        pdx12Debug->EnableDebugLayer();
#endif

    // Create device
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    if (D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(m_Device.put())) != S_OK)
        return false;

    // Setup debug interface
#ifdef DX12_ENABLE_DEBUG_LAYER
    if (pdx12Debug != nullptr) {
        ID3D12InfoQueue* pInfoQueue = nullptr;
        m_Device->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

        const int D3D12_MESSAGE_ID_FENCE_ZERO_WAIT_ = 1424;
        D3D12_MESSAGE_ID disabledMessages[] = {
            static_cast<D3D12_MESSAGE_ID>(D3D12_MESSAGE_ID_FENCE_ZERO_WAIT_)};
        D3D12_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumIDs = 1;
        filter.DenyList.pIDList = disabledMessages;
        pInfoQueue->AddStorageFilterEntries(&filter);

        pInfoQueue->Release();
        pdx12Debug->Release();
    }
#endif

    // Create RTV descriptor heap
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors = APP_NUM_BACK_BUFFERS;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 1;
        if (m_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_RtvDescHeap.put())) != S_OK)
            return false;

        SIZE_T rtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RtvDescHeap->GetCPUDescriptorHandleForHeapStart();
        for (UINT i = 0; i < APP_NUM_BACK_BUFFERS; i++) {
            m_mainRenderTargetDescriptor[i] = rtvHandle;
            rtvHandle.ptr += rtvDescriptorSize;
        }
    }

    // Create SRV descriptor heap
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = APP_SRV_HEAP_SIZE;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (m_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_SrvDescHeap.put())) != S_OK)
            return false;
        heapAlloc->Create(m_Device.get(), m_SrvDescHeap.get());
    }

    // Create command queue
    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 1;
        if (m_Device->CreateCommandQueue(&desc, IID_PPV_ARGS(m_CommandQueue.put())) != S_OK)
            return false;
    }

    // Create command allocators
    for (UINT i = 0; i < APP_NUM_FRAMES_IN_FLIGHT; i++)
        if (m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(m_frameContext[i].CommandAllocator.put())) != S_OK)
            return false;

    // Create command list
    if (m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_frameContext[0].CommandAllocator.get(), nullptr,
        IID_PPV_ARGS(m_CommandList.put())) != S_OK ||
        m_CommandList->Close() != S_OK)
        return false;

    // Create fence
    if (m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.put())) != S_OK)
        return false;

    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_fenceEvent == nullptr)
        return false;

    // Create swap chain
    {
        IDXGIFactory5* dxgiFactory = nullptr;
        IDXGISwapChain1* swapChain1 = nullptr;
        if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK)
            return false;

        BOOL allow_tearing = FALSE;
        dxgiFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allow_tearing,
            sizeof(allow_tearing));
        m_SwapChainTearingSupport = (allow_tearing == TRUE);
        if (m_SwapChainTearingSupport)
            sd.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        if (dxgiFactory->CreateSwapChainForHwnd(m_CommandQueue.get(), hWnd, &sd, nullptr, nullptr,
            &swapChain1) != S_OK)
            return false;
        if (swapChain1->QueryInterface(IID_PPV_ARGS(m_pSwapChain.put())) != S_OK)
            return false;
        if (m_SwapChainTearingSupport)
            dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

        swapChain1->Release();
        dxgiFactory->Release();
        m_pSwapChain->SetMaximumFrameLatency(APP_NUM_BACK_BUFFERS);
        m_hSwapChainWaitableObject = m_pSwapChain->GetFrameLatencyWaitableObject();
    }

    CreateRenderTarget();
    return true;
}

void DX12Renderer::CleanupDeviceD3D() {
    CleanupRenderTarget();

    if (m_hSwapChainWaitableObject != nullptr) {
        CloseHandle(m_hSwapChainWaitableObject);
        m_hSwapChainWaitableObject = nullptr;
    }

    if (m_fenceEvent) {
        CloseHandle(m_fenceEvent);
        m_fenceEvent = nullptr;
    }
}

void DX12Renderer::CreateRenderTarget() {
    for (UINT i = 0; i < APP_NUM_BACK_BUFFERS; i++) {
        ComPtr<ID3D12Resource> pBackBuffer;
        m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(pBackBuffer.put()));
        m_Device->CreateRenderTargetView(pBackBuffer.get(), nullptr,
            m_mainRenderTargetDescriptor[i]);
        m_mainRenderTargetResource[i] = pBackBuffer;
    }
}

void DX12Renderer::CleanupRenderTarget() {
    WaitForPendingOperations();

    for (UINT i = 0; i < APP_NUM_BACK_BUFFERS; i++)
        if (m_mainRenderTargetResource[i]) {
            m_mainRenderTargetResource[i].detach();
        }
}

void DX12Renderer::WaitForPendingOperations() {
    m_CommandQueue->Signal(m_fence.get(), ++m_fenceLastSignaledValue);
    m_fence->SetEventOnCompletion(m_fenceLastSignaledValue, m_fenceEvent);
    ::WaitForSingleObject(m_fenceEvent, INFINITE);
}

FrameContext* DX12Renderer::WaitForNextFrameContext() {
FrameContext* frameContext = &m_frameContext[m_frameIndex % APP_NUM_FRAMES_IN_FLIGHT];
    if (m_fence->GetCompletedValue() < frameContext->FenceValue) {
        m_fence->SetEventOnCompletion(frameContext->FenceValue, m_fenceEvent);
        HANDLE waitableObjects[] = { m_hSwapChainWaitableObject, m_fenceEvent };
        ::WaitForMultipleObjects(2, waitableObjects, TRUE, INFINITE);
    }
    else
        ::WaitForSingleObject(m_hSwapChainWaitableObject, INFINITE);

    return frameContext;
}

} // namespace app
