#include "PCH.hpp"
//#include "Classes.hpp"
//
//
//// Main code
//int MainWindow::launch(HINSTANCE& hInstance) {
//
//    g_pd3dSrvDescHeapAlloc = std::make_unique<ExampleDescriptorHeapAllocator>();
//    std::unique_ptr<WindowManager> window = std::make_unique<WindowManager>();
//
//    window->WMCreateWindow(hInstance);
//
//    // Initialize Direct3D
//    if(!CreateDeviceD3D(window->GetHWND())) {
//        CleanupDeviceD3D();
//        ::UnregisterClass(window->GetWc()->lpszClassName,
//            window->GetWc()->hInstance);
//        return 1;
//    }
//
//    // Show the window
//    ::ShowWindow(window->GetHWND(), SW_SHOWDEFAULT);
//    ::UpdateWindow(window->GetHWND());
//
//    // Setup Dear ImGui context
//    IMGUI_CHECKVERSION();
//    ImGui::CreateContext();
//    ImGuiIO& io = ImGui::GetIO();
//    (void)io;
//    io.ConfigFlags |=
//        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
//    io.ConfigFlags |=
//        ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
//
//    // Setup Dear ImGui style
//    ImGui::StyleColorsDark();
//    // ImGui::StyleColorsLight();
//
//    // Setup scaling
//    ImGuiStyle& style = ImGui::GetStyle();
//    style.ScaleAllSizes(
//        window
//        ->get_main_scale()); // Bake a fixed style scale. (until we have a solution for
//    // dynamic style scaling, changing this requires resetting
//    // Style + calling this again)
//    style.FontScaleDpi =
//        window->get_main_scale() + 0.2f; // Set initial font scale. (using
//    // io.ConfigDpiScaleFonts=true makes this unnecessary.
//    // We leave both here for documentation purpose)
//
//    style.Alpha = 0.90f;
//    style.FrameRounding = 4.0f;
//    style.FramePadding = ImVec2(4.0f, 3.0f);
//    style.FrameBorderSize = 1.0f;
//    style.WindowRounding = 5.0f;
//
//    ImPlot::CreateContext();
//
//    // Setup Platform/Renderer backends
//    ImGui_ImplWin32_Init(window->GetHWND());
//
//    ImGui_ImplDX12_InitInfo init_info = {};
//    init_info.Device = m_Device.Get();
//    init_info.CommandQueue = m_CommandQueue.Get();
//    init_info.NumFramesInFlight = APP_NUM_FRAMES_IN_FLIGHT;
//    init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
//    init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
//
//    // Allocating SRV descriptors (for textures) is up to the application, so we
//    // provide callbacks. (current version of the backend will only allocate one
//    // descriptor, future versions will need to allocate more)
//    init_info.SrvDescriptorHeap = m_SrvDescHeap.Get();
//    init_info.SrvDescriptorAllocFn =
//        std::bit_cast<decltype(init_info.SrvDescriptorAllocFn)>(
//            ([&](ImGui_ImplDX12_InitInfo*,
//                D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle,
//                D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) {
//                    return g_pd3dSrvDescHeapAlloc->Alloc(out_cpu_handle,
//                        out_gpu_handle);
//                }));
//
//
//    init_info.SrvDescriptorFreeFn =
//        std::bit_cast<decltype(init_info.SrvDescriptorFreeFn)>(
//            [&](ImGui_ImplDX12_InitInfo*,
//                D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle,
//                D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) {
//                    return g_pd3dSrvDescHeapAlloc->Free(cpu_handle, gpu_handle);
//            });
//    ImGui_ImplDX12_Init(&init_info);
//
//    // Before 1.91.6: our signature was using a single descriptor. From 1.92,
//    // specifying SrvDescriptorAllocFn/SrvDescriptorFreeFn will be required to
//    // benefit from new features.
//    // ImGui_ImplDX12_Init(m_Device, APP_NUM_FRAMES_IN_FLIGHT,
//    // DXGI_FORMAT_R8G8B8A8_UNORM, m_SrvDescHeap,
//    // m_SrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
//    // m_SrvDescHeap->GetGPUDescriptorHandleForHeapStart());
//
//    FontManager font_manager(&io);
//
//    font_manager.LoadFonts();
//    font_manager.SetDefaultFont();
//
//    // Our state
//    bool show_demo_window = false;
//    bool show_another_window = false;
//    ImVec4 clear_color = ImVec4(0.15f, 0.15f, 0.15f, 1.f);
//
//    WindowClass window_obj;
//
//    // Main loop
//    bool done = false;
//    while(!done) {
//        // Poll and handle messages (inputs, window resize, etc.)
//        // See the WndProc() function below for our to dispatch events to the Win32
//        // backend.
//        MSG msg;
//        while(::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
//            ::TranslateMessage(&msg);
//            ::DispatchMessage(&msg);
//            if(msg.message == WM_QUIT)
//                done = true;
//        }
//        if(done)
//            break;
//
//        // Handle window screen locked
//        if((m_SwapChainOccluded &&
//            m_pSwapChain->Present(0, DXGI_PRESENT_TEST) ==
//            DXGI_STATUS_OCCLUDED) ||
//            ::IsIconic(window->GetHWND())) {
//            ::Sleep(10);
//            continue;
//        }
//        m_SwapChainOccluded = false;
//
//        // Start the Dear ImGui frame
//        ImGui_ImplDX12_NewFrame();
//        ImGui_ImplWin32_NewFrame();
//        ImGui::NewFrame();
//
//        {
//            render(window_obj);
//        }
//
//        // 1. Show the big demo window (Most of the sample code is in
//        // ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
//        // ImGui!).
//        if(show_demo_window)
//            ImGui::ShowDemoWindow(&show_demo_window);
//
//        // 2. Show a simple window that we create ourselves. We use a Begin/End pair
//        // to create a named window.
//        {
//            static float f = 0.0f;
//            static int counter = 0;
//
//            ImGui::Begin(
//                "Hello, world!"); // Create a window called "Hello, world!"
//            // and append into it.
//
//            ImGui::Text(
//                "This is some useful text."); // Display some text (you can
//            // use a format strings too)
//            ImGui::Checkbox(
//                "Demo Window",
//                &show_demo_window); // Edit bools storing our window open/close state
//            ImGui::Checkbox("Another Window", &show_another_window);
//
//            ImGui::SliderFloat(
//                "float",
//                &f,
//                0.0f,
//                1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
//            ImGui::ColorEdit3(
//                "clear color",
//                std::bit_cast<float*>(std::ref(
//                    clear_color))); // Edit 3 floats representing a color
//
//            if(ImGui::Button(
//                "Button")) // Buttons return true when clicked (most
//                // widgets return true when edited/activated)
//                counter++;
//            ImGui::SameLine();
//            ImGui::Text("counter = %d", counter);
//
//            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
//                1000.0f / io.Framerate,
//                io.Framerate);
//            ImGui::End();
//        }
//
//        // 3. Show another simple window.
//        if(show_another_window) {
//            ImGui::Begin(
//                "Another Window",
//                &show_another_window); // Pass a pointer to our bool variable (the
//            // window will have a closing button that will
//            // clear the bool when clicked)
//            ImGui::Text("Hello from another window!");
//            if(ImGui::Button("Close Me"))
//                show_another_window = false;
//            ImGui::End();
//        }
//
//        // Rendering
//        ImGui::Render();
//
//        FrameContext* frameCtx = WaitForNextFrameContext();
//        UINT backBufferIdx = m_pSwapChain->GetCurrentBackBufferIndex();
//        frameCtx->CommandAllocator->Reset();
//
//        D3D12_RESOURCE_BARRIER barrier = {};
//        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
//        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
//        barrier.Transition.pResource =
//            m_MainRenderTargetResource[backBufferIdx].Get();
//        barrier.Transition.Subresource =
//            D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
//        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
//        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
//        m_CommandList->Reset(frameCtx->CommandAllocator.Get(), nullptr);
//        m_CommandList->ResourceBarrier(1, &barrier);
//
//        // Render Dear ImGui graphics
//        const float clear_color_with_alpha[4]={ clear_color.x * clear_color.w,
//                                              clear_color.y * clear_color.w,
//                                              clear_color.z * clear_color.w,
//                                              clear_color.w };
//        m_CommandList->ClearRenderTargetView(
//            m_MainRenderTargetDescriptor[backBufferIdx],
//            clear_color_with_alpha,
//            0,
//            nullptr);
//        m_CommandList->OMSetRenderTargets(
//            1,
//            &m_MainRenderTargetDescriptor[backBufferIdx],
//            FALSE,
//            nullptr);
//        m_CommandList->SetDescriptorHeaps(1, m_SrvDescHeap.GetAddressOf());
//        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(),
//            m_CommandList.Get());
//        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
//        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
//        m_CommandList->ResourceBarrier(1, &barrier);
//        m_CommandList->Close();
//
//        ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
//
//        m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists),
//            ppCommandLists);
//        m_CommandQueue->Signal(m_fence.Get(), ++m_FenceLastSignaledValue);
//        frameCtx->FenceValue = m_FenceLastSignaledValue;
//
//        // Present
//        HRESULT hr = m_pSwapChain->Present(1, 0); // Present with vsync
//        // HRESULT hr = m_pSwapChain->Present(0, m_SwapChainTearingSupport ?
//        // DXGI_PRESENT_ALLOW_TEARING : 0); // Present without vsync
//        m_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
//        g_frameIndex++;
//    }
//
//    WaitForPendingOperations();
//
//    // Cleanup
//    ImGui_ImplDX12_Shutdown();
//    ImGui_ImplWin32_Shutdown();
//    ImGui::DestroyContext();
//
//    CleanupDeviceD3D();
//    ::DestroyWindow(window->GetHWND());
//    ::UnregisterClass(window->GetWc()->lpszClassName,
//        window->GetWc()->hInstance);
//
//    font_manager.CleanupFonts();
//    ImPlot::DestroyContext();
//
//    return 0;
//}
//
//// Helper functions
//
//
//MainWindow::MainWindow() {}
//
//bool MainWindow::CreateDeviceD3D(HWND hWnd) {
//    // Setup swap chain
//    // This is a basic setup. Optimally could handle fullscreen mode differently.
//    // See #8979 for suggestions.
//    DXGI_SWAP_CHAIN_DESC1 sd;
//    {
//        ZeroMemory(&sd, sizeof(sd));
//        sd.BufferCount = APP_NUM_BACK_BUFFERS;
//        sd.Width = 0;
//        sd.Height = 0;
//        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//        sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
//        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//        sd.SampleDesc.Count = 1;
//        sd.SampleDesc.Quality = 0;
//        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
//        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
//        sd.Scaling = DXGI_SCALING_STRETCH;
//        sd.Stereo = FALSE;
//    }
//
//    // [DEBUG] Enable debug interface
//#ifdef DX12_ENABLE_DEBUG_LAYER
//    ID3D12Debug* pdx12Debug = nullptr;
//    if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
//        pdx12Debug->EnableDebugLayer();
//#endif
//
//    // Create device
//    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
//    if(D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&m_Device)) !=
//        S_OK)
//        return false;
//
//    // [DEBUG] Setup debug interface to break on any warnings/errors
//#ifdef DX12_ENABLE_DEBUG_LAYER
//    if(pdx12Debug != nullptr) {
//        ID3D12InfoQueue* pInfoQueue = nullptr;
//        m_Device->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
//        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
//        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
//        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
//
//        // Disable breaking on this warning because of a suspected bug in the D3D12
//        // SDK layer, see #9084 for details.
//        const int D3D12_MESSAGE_ID_FENCE_ZERO_WAIT_ =
//            1424; // not in all copies of d3d12sdklayers.h
//        D3D12_MESSAGE_ID disabledMessages[] = {
//            static_cast<D3D12_MESSAGE_ID>(D3D12_MESSAGE_ID_FENCE_ZERO_WAIT_) };
//        D3D12_INFO_QUEUE_FILTER filter = {};
//        filter.DenyList.NumIDs = 1;
//        filter.DenyList.pIDList = disabledMessages;
//        pInfoQueue->AddStorageFilterEntries(&filter);
//
//        pInfoQueue->Release();
//        pdx12Debug->Release();
//    }
//#endif
//
//    {
//        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
//        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//        desc.NumDescriptors = APP_NUM_BACK_BUFFERS;
//        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//        desc.NodeMask = 1;
//        if(m_Device->CreateDescriptorHeap(&desc,
//            IID_PPV_ARGS(&m_RtvDescHeap)) !=
//            S_OK)
//            return false;
//
//        SIZE_T rtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(
//            D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
//            m_RtvDescHeap->GetCPUDescriptorHandleForHeapStart();
//        for(UINT i = 0; i < APP_NUM_BACK_BUFFERS; i++) {
//            m_MainRenderTargetDescriptor[i] = rtvHandle;
//            rtvHandle.ptr += rtvDescriptorSize;
//        }
//    }
//
//    {
//        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
//        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//        desc.NumDescriptors = APP_SRV_HEAP_SIZE;
//        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//        if(m_Device->CreateDescriptorHeap(&desc,
//            IID_PPV_ARGS(&m_SrvDescHeap)) !=
//            S_OK)
//            return false;
//        g_pd3dSrvDescHeapAlloc->Create(m_Device.Get(), m_SrvDescHeap.Get());
//    }
//
//    {
//        D3D12_COMMAND_QUEUE_DESC desc = {};
//        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
//        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//        desc.NodeMask = 1;
//        if(m_Device->CreateCommandQueue(&desc,
//            IID_PPV_ARGS(&m_CommandQueue)) != S_OK)
//            return false;
//    }
//
//    for(UINT i = 0; i < APP_NUM_FRAMES_IN_FLIGHT; i++)
//        if(m_Device->CreateCommandAllocator(
//            D3D12_COMMAND_LIST_TYPE_DIRECT,
//            IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
//            return false;
//
//    if(m_Device->CreateCommandList(0,
//        D3D12_COMMAND_LIST_TYPE_DIRECT,
//        g_frameContext[0].CommandAllocator.Get(),
//        nullptr,
//        IID_PPV_ARGS(&m_CommandList)) != S_OK ||
//        m_CommandList->Close() != S_OK)
//        return false;
//
//    if(m_Device->CreateFence(0,
//        D3D12_FENCE_FLAG_NONE,
//        IID_PPV_ARGS(&m_fence)) != S_OK)
//        return false;
//
//    m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
//    if(m_FenceEvent == nullptr)
//        return false;
//
//    {
//        IDXGIFactory5* dxgiFactory = nullptr;
//        IDXGISwapChain1* swapChain1 = nullptr;
//        if(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK)
//            return false;
//
//        BOOL allow_tearing = FALSE;
//        dxgiFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING,
//            &allow_tearing,
//            sizeof(allow_tearing));
//        m_SwapChainTearingSupport = (allow_tearing == TRUE);
//        if(m_SwapChainTearingSupport)
//            sd.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
//
//        if(dxgiFactory->CreateSwapChainForHwnd(m_CommandQueue.Get(),
//            hWnd,
//            &sd,
//            nullptr,
//            nullptr,
//            &swapChain1) != S_OK)
//            return false;
//        if(swapChain1->QueryInterface(IID_PPV_ARGS(&m_pSwapChain)) != S_OK)
//            return false;
//        if(m_SwapChainTearingSupport)
//            dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
//
//        swapChain1->Release();
//        dxgiFactory->Release();
//        m_pSwapChain->SetMaximumFrameLatency(APP_NUM_BACK_BUFFERS);
//        m_hSwapChainWaitableObject =
//            m_pSwapChain->GetFrameLatencyWaitableObject();
//    }
//
//    CreateRenderTarget();
//    return true;
//}
//
//void MainWindow::CleanupDeviceD3D() {
//    CleanupRenderTarget();
//
//    if(m_hSwapChainWaitableObject != nullptr) {
//        CloseHandle(m_hSwapChainWaitableObject);
//        m_hSwapChainWaitableObject = nullptr;
//    }
//    for(UINT i = 0; i < APP_NUM_FRAMES_IN_FLIGHT; i++)
//        if(g_frameContext[i].CommandAllocator) {
//            g_frameContext[i].CommandAllocator->Release();
//            g_frameContext[i].CommandAllocator = nullptr;
//        }
//
//    if(m_FenceEvent) {
//        CloseHandle(m_FenceEvent);
//        m_FenceEvent = nullptr;
//    }
//}
//
//void MainWindow::CreateRenderTarget() {
//    for(UINT i = 0; i < APP_NUM_BACK_BUFFERS; i++) {
//        ID3D12Resource* pBackBuffer = nullptr;
//        m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
//        m_Device->CreateRenderTargetView(pBackBuffer,
//            nullptr,
//            m_MainRenderTargetDescriptor[i]);
//        m_MainRenderTargetResource[i] = pBackBuffer;
//    }
//}
//
//void MainWindow::CleanupRenderTarget() {
//    WaitForPendingOperations();
//
//    for(UINT i = 0; i < APP_NUM_BACK_BUFFERS; i++)
//        if(m_MainRenderTargetResource[i]) {
//            m_MainRenderTargetResource[i]->Release();
//            m_MainRenderTargetResource[i] = nullptr;
//        }
//}
//
//void MainWindow::WaitForPendingOperations() {
//    m_CommandQueue->Signal(m_fence.Get(), ++m_FenceLastSignaledValue);
//
//    m_fence->SetEventOnCompletion(m_FenceLastSignaledValue, m_FenceEvent);
//    ::WaitForSingleObject(m_FenceEvent, INFINITE);
//}
//
//FrameContext* MainWindow::WaitForNextFrameContext() {
//    FrameContext* frame_context =
//        &g_frameContext[g_frameIndex % APP_NUM_FRAMES_IN_FLIGHT];
//    if(m_fence->GetCompletedValue() < frame_context->FenceValue) {
//        m_fence->SetEventOnCompletion(frame_context->FenceValue, m_FenceEvent);
//        HANDLE waitableObjects[] = { m_hSwapChainWaitableObject, m_FenceEvent };
//        ::WaitForMultipleObjects(2, waitableObjects, TRUE, INFINITE);
//    } else
//        ::WaitForSingleObject(m_hSwapChainWaitableObject, INFINITE);
//
//    return frame_context;
//}
//
//
//std::string MainWindow::HrToString(HRESULT hr) {
//    char s_str[64] = {};
//    sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
//    return std::string(s_str);
//}
