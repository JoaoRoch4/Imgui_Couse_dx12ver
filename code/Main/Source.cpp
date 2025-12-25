#include "PCH.hpp"
#include "Classes.hpp"
#include "Source.hpp"

using namespace app;

// Application objects
MemoryManagement*	  m_memory;
OutputConsole*		  m_console;
CommandLineArguments* m_cmdArgs;
WindowManager*		  m_window;
FontManager*		  m_font_manager;
FontManagerWindow*	  m_font_manager_window;
DebugWindow*		  m_debug_window;
ConfigManager*		  m_configManager;
WindowClass*		  m_window_obj;

// DirectX 12 Renderer and related objects
DX12Renderer* m_renderer;
app::ExampleDescriptorHeapAllocator* m_HeapAlloc;

int Start(_In_ HINSTANCE hInstance) {

m_memory = MemoryManagement::Get_MemoryManagement();
m_memory->AllocAll();

m_console = m_memory->Get_OutputConsole();

m_console->Open();


m_console->Out << tc::green << "\nHello From m_console class!\n" << tc::reset;

m_cmdArgs = m_memory->Get_CommandLineArguments();

m_console->Out << tc::green << "Memory management initialized" << std::endl;
m_console->Out << L"=== Application Starting ===" << std::endl << tc::reset;

m_configManager = m_memory->Get_ConfigManager();


// Main code
m_HeapAlloc = m_memory->Get_ExampleDescriptorHeapAllocator();
m_window	= m_memory->Get_WindowManager();
	
// Create DX12 Renderer
m_renderer = new DX12Renderer();

OpenWindow(hInstance);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& m_io = ImGui::GetIO();
	static_cast<void>(m_io);
	m_memory->Set_ImGuiIO(&m_io);
	m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();

	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(
		m_window->get_main_scale()); // Bake a fixed style scale. (until we have a solution for
	// dynamic style scaling, changing this requires resetting
	// Style + calling this again)
	style.FontScaleDpi = m_window->get_main_scale() + 0.2f; // Set initial font scale. (using
	// m_io.ConfigDpiScaleFonts=true makes this unnecessary.
	// We leave both here for documentation purpose)

	style.Alpha			  = 0.90f;
	style.FrameRounding	  = 4.0f;
	style.FramePadding	  = ImVec2(4.0f, 3.0f);
	style.FrameBorderSize = 1.0f;
	style.WindowRounding  = 5.0f;

	ImPlot::CreateContext();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_window->GetHWND());

	ImGui_ImplDX12_InitInfo init_info = {};

	init_info.Device			= m_renderer->GetDevice();
	init_info.CommandQueue		= m_renderer->GetCommandQueue();
	init_info.NumFramesInFlight = APP_NUM_FRAMES_IN_FLIGHT;
	init_info.RTVFormat			= DXGI_FORMAT_R8G8B8A8_UNORM;
	init_info.DSVFormat			= DXGI_FORMAT_UNKNOWN;

	// Allocating SRV descriptors (for textures) is up to the application, so we
	// provide callbacks. (current version of the backend will only allocate one
	// descriptor, future versions will need to allocate more)
	init_info.SrvDescriptorHeap	   = m_renderer->GetSrvDescHeap();
	init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*,
										D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle,
										D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) {
		return m_HeapAlloc->Alloc(out_cpu_handle, out_gpu_handle);
	};
	init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*,
									   D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle,
									   D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) {
		return m_HeapAlloc->Free(cpu_handle, gpu_handle);
	};
	ImGui_ImplDX12_Init(&init_info);
	MainLoop(&m_io);
	m_renderer->WaitForPendingOperations();

	// Cleanup
	Cleanup();

	return 0;
}


void OpenWindow(_In_ HINSTANCE hInstance) {


	m_cmdArgs->Open();

	m_window->WMCreateWindow(hInstance, m_cmdArgs);

	// Initialize Direct3D
	if (!m_renderer->CreateDeviceD3D(m_window->GetHWND(), m_HeapAlloc)) {
		m_renderer->CleanupDeviceD3D();
		::UnregisterClassW(m_window->GetWc()->lpszClassName, m_window->GetWc()->hInstance);
		throw std::runtime_error("failed CreateDeviceD3D");
	}

	// Show the m_window
	::ShowWindow(m_window->GetHWND(), SW_SHOWMAXIMIZED);
	::UpdateWindow(m_window->GetHWND());
}

void MainLoop(ImGuiIO* m_io) {
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

	// ========================================================================
	// STEP 1: Initialize ConfigManager and load saved settings
	// ========================================================================

	// Get ConfigManager instance from MemoryManagement
	m_configManager = m_memory->Get_ConfigManager();

	// Call Open() to load configuration from disk
	// This will load config.json if it exists
	m_configManager->Open();

	// ========================================================================
	// STEP 2: Load the saved clear color (or use default)
	// ========================================================================

	// Get the clear color from configuration
	// If config.json exists, this will be the saved value
	// If not, it will be the default value from constructor (0.15, 0.15, 0.15, 1.0)
	ImVec4 clear_color = m_configManager->GetClearColorAsImVec4();

	// Optional: Print the loaded color to m_console
	m_console->Out << "Loaded clear color: R=" << clear_color.x << " G=" << clear_color.y
				   << " B=" << clear_color.z << " A=" << clear_color.w << "\n";

	// Create m_window object for file system browser
	m_window_obj = m_memory->Get_WindowClass();
	m_window_obj->Open();


	// Track if color was modified this frame
	bool colorModified = false;

	// ========================================================================
	// MAIN LOOP
	// ========================================================================
	bool done = false;
	while (!done) {
		// Poll and handle Windows messages
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT) done = true;
		}
		if (done) break;

		// Handle m_window occlusion and minimization
	if ((m_renderer->GetSwapChainOccluded() &&
		 m_renderer->GetSwapChain()->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) ||
			::IsIconic(m_window->GetHWND())) {
			::Sleep(10);
			continue;
		}
	m_renderer->SetSwapChainOccluded(false);

		// Start ImGui frame
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// Render optional windows
		if (m_memory->m_bShow_FileSys_window) m_window_obj->Tick();
		if (m_memory->m_bShow_Debug_window) m_debug_window->Tick();

		if (m_memory->m_bShow_FontManager_window) m_font_manager_window->Tick();
		if (m_memory->m_bShow_styleEditor_window) ImGui::ShowStyleEditor();
		if (m_memory->m_bShow_demo_window) ImGui::ShowDemoWindow(&m_memory->m_bShow_demo_window);

		// ====================================================================
		// MAIN WINDOW WITH COLOR PICKER AND SAVE FUNCTIONALITY
		// ====================================================================
		{
			static float f		 = 0.0f;
			static int	 counter = 0;

			// Reset color modification flag for this frame
			colorModified = false;

			ImGui::Begin("Hello, world!");
			Helpers::ShowExampleAppMainMenuBar();

			ig::Separator();
			ImGui::Text("Background Color Settings");
			ig::Separator();

			// ================================================================
			// STEP 3: Color picker that modifies clear_color
			// ================================================================

			// ColorEdit3 returns true when the color is modified
			// We use this to detect when to save the configuration
			if (ImGui::ColorEdit3("Background Color",
								  std::bit_cast<float*>(std::ref(clear_color)))) {
				// Color was modified by user
				colorModified = true;
			}

			// Show current color values
			ImGui::Text("Current color: R=%.2f G=%.2f B=%.2f A=%.2f", clear_color.x, clear_color.y,
						clear_color.z, clear_color.w);

			ig::Separator();

			// ================================================================
			// STEP 4: Manual save button (optional)
			// ================================================================

			// Provide a manual save button as alternative
			if (ImGui::Button("Save Color to Config")) {
				// Update ConfigManager with current color
				m_configManager->SetClearColor(clear_color.x, clear_color.y, clear_color.z,
											   clear_color.w);

				// Save to disk
				if (m_configManager->SaveConfiguration()) {
					m_console->Out << tc::green << "Background color saved successfully!\n"
								   << tc::reset;
				} else {
					m_console->Out << tc::red << "Failed to save background color!\n" << tc::reset;
				}
			}

			// Show when file was last saved
			ImGui::SameLine();
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::Text("Config file location:");
				ImGui::Text("%ls", m_configManager->GetConfigFilePath().c_str());
				ImGui::EndTooltip();
			}

			ig::Separator();

			// ================================================================
			// STEP 5: Auto-save when color changes (recommended approach)
			// ================================================================

			// If color was modified this frame, automatically save it
			if (colorModified) {
				// Update ConfigManager with new color
				m_configManager->SetClearColor(clear_color.x, clear_color.y, clear_color.z,
											   clear_color.w);

				// Auto-save to disk
				m_configManager->SaveConfiguration();


				// Optional: Show feedback to user
				m_console->Out << "Color auto-saved: R=" << clear_color.x << " G=" << clear_color.y
							   << " B=" << clear_color.z << "\n";
			}

			ig::Separator();

			// Rest of your UI code...
			ImGui::Text("Other Settings");

			ImGui::Checkbox("Demo Window", &m_memory->m_bShow_demo_window);
			ImGui::Checkbox("Another Window", &m_memory->m_bShow_another_window);
			ImGui::Checkbox("Style Editor", &m_memory->m_bShow_styleEditor_window);

			ig::Separator();

			ImGui::Checkbox("Debug Window", &m_memory->m_bShow_Debug_window);
			ImGui::Checkbox("Font Manager Window", &m_memory->m_bShow_FontManager_window);
			ImGui::Checkbox("File System Window", &m_memory->m_bShow_FileSys_window);

			ig::Separator();

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

		// Another m_window example
		if (m_memory->m_bShow_another_window) {
			ImGui::Begin("Another Window", &m_memory->m_bShow_another_window);
			ImGui::Text("Hello from another m_window!");
			if (ImGui::Button("Close Me")) m_memory->m_bShow_another_window = false;
			ImGui::End();
		}

		// Rendering
		ImGui::Render();

		// Get frame context and back buffer
		FrameContext* frameCtx		= m_renderer->WaitForNextFrameContext();
		UINT		  backBufferIdx = m_renderer->GetSwapChain()->GetCurrentBackBufferIndex();
		frameCtx->CommandAllocator->Reset();

		// Transition to render target state
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource   = m_renderer->GetRenderTarget(backBufferIdx);
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;

		m_renderer->GetCommandList()->Reset(frameCtx->CommandAllocator.get(), nullptr);
		m_renderer->GetCommandList()->ResourceBarrier(1, &barrier);

		// ====================================================================
		// STEP 6: Use clear_color for rendering
		// ====================================================================

		// Convert ImVec4 to float array with alpha premultiplied
		const float clear_color_with_alpha[4] = {
			clear_color.x * clear_color.w, // Red * Alpha
			clear_color.y * clear_color.w, // Green * Alpha
			clear_color.z * clear_color.w, // Blue * Alpha
			clear_color.w				   // Alpha
		};

		// Clear render target with the configured color
		m_renderer->GetCommandList()->ClearRenderTargetView(m_renderer->GetRenderTargetDescriptor(backBufferIdx),
													 clear_color_with_alpha, 0, nullptr);

		// Set render target and descriptor heaps
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_renderer->GetRenderTargetDescriptor(backBufferIdx);
		m_renderer->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE,
											  nullptr);

		ID3D12DescriptorHeap* ppHeaps[] = {m_renderer->GetSrvDescHeap()};
		m_renderer->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		// Render ImGui draw data
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_renderer->GetCommandList());

		// Transition to present state
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
		m_renderer->GetCommandList()->ResourceBarrier(1, &barrier);
		m_renderer->GetCommandList()->Close();

		// Execute command list
		ID3D12CommandList* ppCommandLists[] = {m_renderer->GetCommandList()};
		m_renderer->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		m_renderer->GetCommandQueue()->Signal(m_renderer->GetFence(), ++m_renderer->GetFenceLastSignaledValue());
		frameCtx->FenceValue = m_renderer->GetFenceLastSignaledValue();

		// Present
		HRESULT hr			= m_renderer->GetSwapChain()->Present(1, 0);
		m_renderer->SetSwapChainOccluded(hr == DXGI_STATUS_OCCLUDED);
		m_renderer->GetFrameIndex()++;
	}

	// ========================================================================
	// STEP 7: Save configuration on exit
	// ========================================================================

	// Save final configuration before closing
	// This ensures any unsaved changes are persisted
	m_configManager->Close();

	m_console->Out << tc::green << "Configuration saved on exit\n" << tc::reset;
}

void Cleanup() {

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (m_renderer) {
		m_renderer->CleanupDeviceD3D();
		delete m_renderer;
		m_renderer = nullptr;
	}
	
	::DestroyWindow(m_window->GetHWND());
	::UnregisterClassW(m_window->GetWc()->lpszClassName, m_window->GetWc()->hInstance);

	ImPlot::DestroyContext();

	m_memory			  = nullptr;
	m_console			  = nullptr;
	m_cmdArgs			  = nullptr;
	m_window			  = nullptr;
	m_font_manager		  = nullptr;
	m_font_manager_window = nullptr;
	m_debug_window		  = nullptr;
	m_configManager		  = nullptr;
	m_window_obj		  = nullptr;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam,
															 LPARAM lParam);

// dear imgui wants to use your inputs.
// - When m_io.WantCaptureMouse is true, do not dispatch mouse input data to your
// main application, or clear/overwrite your copy of the mouse data.
// - When m_io.WantCaptureKeyboard is true, do not dispatch keyboard input data to
// your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from
// your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;

	switch (msg) {
		case WM_SIZE:
		if (m_renderer && m_renderer->GetDevice() != nullptr && wParam != SIZE_MINIMIZED) {
			m_renderer->CleanupRenderTarget();
			DXGI_SWAP_CHAIN_DESC1 desc = {};
			m_renderer->GetSwapChain()->GetDesc1(&desc);
			HRESULT result = m_renderer->GetSwapChain()->ResizeBuffers(0, static_cast<UINT> LOWORD(lParam),
														 static_cast<UINT> HIWORD(lParam),
														 desc.Format, desc.Flags);
			IM_ASSERT(FAILED(result) && "Failed to resize swapchain.");
			m_renderer->CreateRenderTarget();
		}
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY: ::PostQuitMessage(0); return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
