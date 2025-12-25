#include "PCH.hpp"
#include "App.hpp"
#include "Classes.hpp"

// Forward declare ImGui message handler (global namespace)
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace app {

// Static instance
App* App::s_instance = nullptr;

App::App() :
    m_memory(nullptr),
    m_console(nullptr),
    m_cmdArgs(nullptr),
    m_window(nullptr),
    m_font_manager(nullptr),
    m_font_manager_window(nullptr),
    m_debug_window(nullptr),
    m_configManager(nullptr),
    m_window_obj(nullptr),
    m_renderer(nullptr),
    m_HeapAlloc(nullptr),
    m_io(nullptr) {
    
    s_instance = this;
}

App::~App() {
    Cleanup();
    s_instance = nullptr;
}

int App::Run(_In_ HINSTANCE hInstance) {
    Initialize(hInstance);
    MainLoop();
    return 0;
}

void App::Initialize(_In_ HINSTANCE hInstance) {
    // Initialize memory management
    m_memory = MemoryManagement::Get_MemoryManagement();
    m_memory->AllocAll();

    m_console = m_memory->Get_OutputConsole();
    m_console->Open();

    m_console->Out << tc::green << "\nHello From console class!\n" << tc::reset;

    m_cmdArgs = m_memory->Get_CommandLineArguments();

    m_console->Out << tc::green << "Memory management initialized" << std::endl;
    m_console->Out << L"=== Application Starting ===" << std::endl << tc::reset;

    m_configManager = m_memory->Get_ConfigManager();

    // Initialize DirectX components
    m_HeapAlloc = m_memory->Get_ExampleDescriptorHeapAllocator();
    m_window = m_memory->Get_WindowManager();
    
    // Get DX12 Renderer from memory management (uses smart pointer)
    m_renderer = m_memory->Get_DX12Renderer();

    // Open window and initialize DirectX
    OpenWindow(hInstance);

    // Setup ImGui
    SetupImGui();
    SetupImGuiBackend();
}

void App::OpenWindow(_In_ HINSTANCE hInstance) {
    m_cmdArgs->Open();
    m_window->WMCreateWindow(hInstance, m_cmdArgs);

    // Initialize Direct3D
    if (!m_renderer->CreateDeviceD3D(m_window->GetHWND(), m_HeapAlloc)) {
        m_renderer->CleanupDeviceD3D();
        ::UnregisterClassW(m_window->GetWc()->lpszClassName, m_window->GetWc()->hInstance);
        throw std::runtime_error("Failed to create D3D12 device");
    }

    // Show the window
    ::ShowWindow(m_window->GetHWND(), SW_SHOWMAXIMIZED);
    ::UpdateWindow(m_window->GetHWND());
}

void App::SetupImGui() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_io = &ImGui::GetIO();
    m_memory->Set_ImGuiIO(m_io);
    
    m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Note: Style configuration is handled by StyleManager in MainLoop()
    // This includes theme selection, scaling, and all visual properties
    // Do NOT set style properties here to avoid conflicts with StyleManager

    ImPlot::CreateContext();
}

void App::SetupImGuiBackend() {
    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(m_window->GetHWND());

    ImGui_ImplDX12_InitInfo init_info = {};
    init_info.Device = m_renderer->GetDevice();
    init_info.CommandQueue = m_renderer->GetCommandQueue();
    init_info.NumFramesInFlight = APP_NUM_FRAMES_IN_FLIGHT;
    init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;

    // Allocating SRV descriptors (for textures)
    init_info.SrvDescriptorHeap = m_renderer->GetSrvDescHeap();
    init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*,
                                        D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle,
                                        D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) {
        return App::GetInstance()->m_HeapAlloc->Alloc(out_cpu_handle, out_gpu_handle);
    };
    init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*,
                                       D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle,
                                       D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) {
        return App::GetInstance()->m_HeapAlloc->Free(cpu_handle, gpu_handle);
    };
    
    ImGui_ImplDX12_Init(&init_info);
}

void App::MainLoop() {
    // Get font manager and initialize
    m_font_manager = m_memory->Get_FontManager();
    m_font_manager->GetIo(m_io);

    m_font_manager_window = m_memory->Get_FontManagerWindow();
    m_font_manager_window->GetAux(m_window->GetHWND(), m_font_manager);

    m_debug_window = m_memory->Get_DebugWindow();
    m_debug_window->GetIo(m_io);

    // Load fonts
    m_font_manager->LoadFonts();
    m_font_manager->SetDefaultFont();

    // Initialize ConfigManager and load saved settings
    m_configManager = m_memory->Get_ConfigManager();
    m_configManager->Open();

    // Initialize StyleManager and load saved style settings
    // Must be called after ImGui::CreateContext() in SetupImGui()
    StyleManager* styleManager = m_memory->Get_StyleManager();
    styleManager->Open();

    // Apply DPI scaling after style is loaded
    // This ensures scaling is applied to the loaded or default style
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(m_window->get_main_scale());
    
    // Load the saved clear color (or use default)
    ImVec4 clear_color = m_configManager->GetClearColorAsImVec4();

    m_console->Out << "Loaded clear color: R=" << clear_color.x << " G=" << clear_color.y
                   << " B=" << clear_color.z << " A=" << clear_color.w << "\n";

    // Create window object for file system browser
    m_window_obj = m_memory->Get_WindowClass();
    m_window_obj->Open();

    // Track if color was modified this frame
    bool colorModified = false;

    // Main loop
    bool done = false;
    while (!done) {
        // Poll and handle Windows messages
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT) {
                done = true;
            }
        }
        if (done) break;

        // Render frame
        if (!RenderFrame(clear_color, colorModified)) {
            break;
        }
    }

    // Save configuration on exit
    m_configManager->Close();
    m_console->Out << tc::green << "Configuration saved on exit\n" << tc::reset;
}

bool App::RenderFrame(ImVec4& clear_color, bool& colorModified) {
    // Handle window occlusion and minimization
    if ((m_renderer->GetSwapChainOccluded() &&
         m_renderer->GetSwapChain()->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) ||
        ::IsIconic(m_window->GetHWND())) {
        ::Sleep(10);
        return true;
    }
    m_renderer->SetSwapChainOccluded(false);

    // Start ImGui frame
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Render UI
    RenderUI(clear_color, colorModified);

    // Rendering
    ImGui::Render();

    // Get frame context and back buffer
    FrameContext* frameCtx = m_renderer->WaitForNextFrameContext();
    UINT backBufferIdx = m_renderer->GetSwapChain()->GetCurrentBackBufferIndex();
    frameCtx->CommandAllocator->Reset();

    // Transition to render target state
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = m_renderer->GetRenderTarget(backBufferIdx);
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

    m_renderer->GetCommandList()->Reset(frameCtx->CommandAllocator.get(), nullptr);
    m_renderer->GetCommandList()->ResourceBarrier(1, &barrier);

    // Convert ImVec4 to float array with alpha premultiplied
    const float clear_color_with_alpha[4] = {
        clear_color.x * clear_color.w,
        clear_color.y * clear_color.w,
        clear_color.z * clear_color.w,
        clear_color.w
    };

    // Clear render target with the configured color
    m_renderer->GetCommandList()->ClearRenderTargetView(
        m_renderer->GetRenderTargetDescriptor(backBufferIdx),
        clear_color_with_alpha, 0, nullptr);

    // Set render target and descriptor heaps
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_renderer->GetRenderTargetDescriptor(backBufferIdx);
    m_renderer->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    ID3D12DescriptorHeap* ppHeaps[] = { m_renderer->GetSrvDescHeap() };
    m_renderer->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    // Render ImGui draw data
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_renderer->GetCommandList());

    // Transition to present state
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    m_renderer->GetCommandList()->ResourceBarrier(1, &barrier);
    m_renderer->GetCommandList()->Close();

    // Execute command list
    ID3D12CommandList* ppCommandLists[] = { m_renderer->GetCommandList() };
    m_renderer->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    m_renderer->GetCommandQueue()->Signal(m_renderer->GetFence(), ++m_renderer->GetFenceLastSignaledValue());
    frameCtx->FenceValue = m_renderer->GetFenceLastSignaledValue();

    // Present
    HRESULT hr = m_renderer->GetSwapChain()->Present(1, 0);
    m_renderer->SetSwapChainOccluded(hr == DXGI_STATUS_OCCLUDED);
    m_renderer->GetFrameIndex()++;

    return true;
}

void App::RenderUI(ImVec4& clear_color, bool& colorModified) {
    // Reset color modification flag for this frame
    colorModified = false;

    // Render optional windows
    if (m_memory->m_bShow_FileSys_window) m_window_obj->Tick();
    if (m_memory->m_bShow_Debug_window) m_debug_window->Tick();
    if (m_memory->m_bShow_FontManager_window) m_font_manager_window->Tick();
    
    // Show style editor window (can be opened from demo window or main menu)
    if (m_memory->m_bShow_styleEditor_window) {
        // Create a manual window for the style editor with proper close button
        ImGui::Begin("Style Editor", &m_memory->m_bShow_styleEditor_window);
        
        // Add custom save/load buttons at the top - Load button first
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.6f, 1.0f));
        if (ImGui::Button("Load Style Configuration", ImVec2(200, 30))) {
            StyleManager* styleManager = m_memory->Get_StyleManager();
            if (styleManager->LoadConfiguration()) {
                styleManager->ApplyStyleToImGui();
                m_console->Out << tc::green << "Style configuration loaded successfully!\n" << tc::reset;
            } else {
                m_console->Out << tc::red << "Failed to load style configuration!\n" << tc::reset;
            }
        }
        ImGui::PopStyleColor(3);
        
        ImGui::SameLine();
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
        if (ImGui::Button("Save Style Configuration", ImVec2(200, 30))) {
            StyleManager* styleManager = m_memory->Get_StyleManager();
            if (styleManager->SaveConfiguration()) {
                m_console->Out << tc::green << "Style configuration saved successfully!\n" << tc::reset;
            } else {
                m_console->Out << tc::red << "Failed to save style configuration!\n" << tc::reset;
            }
        }
        ImGui::PopStyleColor(3);
        
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            StyleManager* styleManager = m_memory->Get_StyleManager();
            ImGui::Text("Style config file location:");
            ImGui::Text("%ls", styleManager->GetConfigFilePath().c_str());
            ImGui::EndTooltip();
        }
        
        ImGui::Separator();
        
        // Add preset theme buttons
        ImGui::Text("Quick Presets:");
        ImGui::SameLine();
        
        if (ImGui::Button("Dark")) {
            StyleManager* styleManager = m_memory->Get_StyleManager();
            styleManager->ApplyPresetDark();
            m_console->Out << "Applied Dark theme preset\n";
        }
        ImGui::SameLine();
        
        if (ImGui::Button("Light")) {
            StyleManager* styleManager = m_memory->Get_StyleManager();
            styleManager->ApplyPresetLight();
            m_console->Out << "Applied Light theme preset\n";
        }
        ImGui::SameLine();
        
        if (ImGui::Button("Classic")) {
            StyleManager* styleManager = m_memory->Get_StyleManager();
            styleManager->ApplyPresetClassic();
            m_console->Out << "Applied Classic theme preset\n";
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(Presets don't auto-save)");
        
        ImGui::Separator();
        ImGui::Spacing();
        
        // Show the built-in ImGui style editor
        ImGui::ShowStyleEditor(nullptr);
        
        ImGui::End();
    }
    
    // Show demo window
    // Note: The demo window has "Tools > Style Editor" menu that will open the style editor
    // To make it work with our flag, we need to detect when it's opened
    if (m_memory->m_bShow_demo_window) {
        ImGui::ShowDemoWindow(&m_memory->m_bShow_demo_window);
        
        // Detect if demo window opened the style editor via its menu
        // When user clicks "Tools > Style Editor" in demo, it creates a window named "Dear ImGui Style Editor"
        if (!m_memory->m_bShow_styleEditor_window && ImGui::FindWindowByName("Dear ImGui Style Editor")) {
            m_memory->m_bShow_styleEditor_window = true;
        }
    }
    
    // If demo window opened the native style editor, we need to manage it separately
    // Close the native one and use our wrapped version instead
    if (ImGui::FindWindowByName("Dear ImGui Style Editor") && !m_memory->m_bShow_styleEditor_window) {
        // Demo window opened it, so set our flag to true
        m_memory->m_bShow_styleEditor_window = true;
    }

    // Main window with color picker and save functionality
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");
        Helpers::ShowExampleAppMainMenuBar();

        ImGui::Separator();
        ImGui::Text("Background Color Settings");
        ImGui::Separator();

        // Color picker
        if (ImGui::ColorEdit3("Background Color", std::bit_cast<float*>(std::ref(clear_color)))) {
            colorModified = true;
        }

        // Show current color values
        ImGui::Text("Current color: R=%.2f G=%.2f B=%.2f A=%.2f", 
                    clear_color.x, clear_color.y, clear_color.z, clear_color.w);

        ImGui::Separator();

        // Manual save button
        if (ImGui::Button("Save Color to Config")) {
            m_configManager->SetClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

            if (m_configManager->SaveConfiguration()) {
                m_console->Out << tc::green << "Background color saved successfully!\n" << tc::reset;
            } else {
                m_console->Out << tc::red << "Failed to save background color!\n" << tc::reset;
            }
        }

        // Show config file location
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Config file location:");
            ImGui::Text("%ls", m_configManager->GetConfigFilePath().c_str());
            ImGui::EndTooltip();
        }

        ImGui::Separator();

        // Auto-save when color changes
        if (colorModified) {
            m_configManager->SetClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            m_configManager->SaveConfiguration();
            m_console->Out << "Color auto-saved: R=" << clear_color.x 
                          << " G=" << clear_color.y << " B=" << clear_color.z << "\n";
        }

        ImGui::Separator();

        // Other settings
        ImGui::Text("Other Settings");

        ImGui::Checkbox("Demo Window", &m_memory->m_bShow_demo_window);
        ImGui::Checkbox("Another Window", &m_memory->m_bShow_another_window);
        ImGui::Checkbox("Style Editor", &m_memory->m_bShow_styleEditor_window);

        ImGui::Separator();

        ImGui::Checkbox("Debug Window", &m_memory->m_bShow_Debug_window);
        ImGui::Checkbox("Font Manager Window", &m_memory->m_bShow_FontManager_window);
        ImGui::Checkbox("File System Window", &m_memory->m_bShow_FileSys_window);

        ImGui::Separator();

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

        if (ImGui::Button("--")) counter--;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);
        ImGui::SameLine();
        if (ImGui::Button("++")) counter++;
        ImGui::SameLine();
        if (ImGui::Button("Reset")) counter = 0;

        ImGui::End();
    }

    // Another window example
    if (m_memory->m_bShow_another_window) {
        ImGui::Begin("Another Window", &m_memory->m_bShow_another_window);
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me")) {
            m_memory->m_bShow_another_window = false;
        }
        ImGui::End();
    }
}

void App::Cleanup() {
// Wait for GPU to finish
if (m_renderer) {
    m_renderer->WaitForPendingOperations();
}

// Shutdown ImGui (only if initialized)
if (ImGui::GetCurrentContext() != nullptr) {
    // Only shutdown backends if they were initialized
    if (ImGui::GetIO().BackendRendererUserData != nullptr) {
        ImGui_ImplDX12_Shutdown();
    }
    if (ImGui::GetIO().BackendPlatformUserData != nullptr) {
        ImGui_ImplWin32_Shutdown();
    }
    ImGui::DestroyContext();
}
    
// Shutdown ImPlot (only if initialized)
if (ImPlot::GetCurrentContext() != nullptr) {
    ImPlot::DestroyContext();
}

    // Cleanup renderer (managed by MemoryManagement's smart pointer - don't delete)
    if (m_renderer) {
        m_renderer->CleanupDeviceD3D();
        m_renderer = nullptr; // Just clear the pointer, MemoryManagement owns it
    }

    // Destroy window
    if (m_window) {
        ::DestroyWindow(m_window->GetHWND());
        ::UnregisterClassW(m_window->GetWc()->lpszClassName, m_window->GetWc()->hInstance);
    }

    // Clear all pointers (all managed by MemoryManagement)
    m_memory = nullptr;
    m_console = nullptr;
    m_cmdArgs = nullptr;
    m_window = nullptr;
    m_font_manager = nullptr;
    m_font_manager_window = nullptr;
    m_debug_window = nullptr;
    m_configManager = nullptr;
    m_window_obj = nullptr;
}

LRESULT WINAPI App::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
        return true;
    }

    App* app = App::GetInstance();
    if (!app) {
        return ::DefWindowProcW(hWnd, msg, wParam, lParam);
    }

    switch (msg) {
    case WM_SIZE:
        if (app->m_renderer && app->m_renderer->GetDevice() != nullptr && wParam != SIZE_MINIMIZED) {
            app->m_renderer->CleanupRenderTarget();
            DXGI_SWAP_CHAIN_DESC1 desc = {};
            app->m_renderer->GetSwapChain()->GetDesc1(&desc);
            HRESULT result = app->m_renderer->GetSwapChain()->ResizeBuffers(
                0,
                static_cast<UINT>(LOWORD(lParam)),
                static_cast<UINT>(HIWORD(lParam)),
                desc.Format,
                desc.Flags);
            IM_ASSERT(!FAILED(result) && "Failed to resize swapchain.");
            app->m_renderer->CreateRenderTarget();
        }
        return 0;
        
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) { // Disable ALT application menu
            return 0;
        }
        break;
        
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

} // namespace app
