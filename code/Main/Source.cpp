#include "PCH.hpp"
#include "Classes.hpp"
#include "Source.hpp"

using namespace app;
// Data
ExampleDescriptorHeapAllocator*	   m_HeapAlloc;
ComPtr<ID3D12Device>			   m_Device;
ComPtr<ID3D12DescriptorHeap>	   m_RtvDescHeap;
ComPtr<ID3D12DescriptorHeap>	   m_SrvDescHeap;
ComPtr<ID3D12CommandQueue>		   m_CommandQueue;
ComPtr<ID3D12GraphicsCommandList>  m_CommandList;
ComPtr<ID3D12Fence>				   m_fence;
ComPtr<IDXGISwapChain3>			   m_pSwapChain;
ComPtr<ID3D12Resource>			   m_mainRenderTargetResource[APP_NUM_BACK_BUFFERS];
static D3D12_CPU_DESCRIPTOR_HANDLE m_mainRenderTargetDescriptor[APP_NUM_BACK_BUFFERS] = {};
static FrameContext				   m_frameContext[APP_NUM_FRAMES_IN_FLIGHT];

MemoryManagement*	  m_memory;
OutputConsole*		  m_console;
CommandLineArguments* m_cmdArgs;
WindowManager*		  m_window;
FontManager*		  m_font_manager;
FontManagerWindow*	  m_font_manager_window;
DebugWindow*		  m_debug_window;
ConfigManager*		  m_configManager;
WindowClass*		  m_window_obj;

/**
 * @brief Gets next available frame context for rendering
 * @return Pointer to frame context that is ready for recording
 *
 * Waits for the frame to become available (GPU finished processing it),
 * then waits for the swap chain waitable object for frame latency control.
 */
FrameContext* WaitForNextFrameContext();


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

	init_info.Device			= m_Device.get();
	init_info.CommandQueue		= m_CommandQueue.get();
	init_info.NumFramesInFlight = APP_NUM_FRAMES_IN_FLIGHT;
	init_info.RTVFormat			= DXGI_FORMAT_R8G8B8A8_UNORM;
	init_info.DSVFormat			= DXGI_FORMAT_UNKNOWN;

	// Allocating SRV descriptors (for textures) is up to the application, so we
	// provide callbacks. (current version of the backend will only allocate one
	// descriptor, future versions will need to allocate more)
	init_info.SrvDescriptorHeap	   = m_SrvDescHeap.get();
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

	// Before 1.91.6: our signature was using a single descriptor. From 1.92,
	// specifying SrvDescriptorAllocFn/SrvDescriptorFreeFn will be required to
	// benefit from new features.
	// ImGui_ImplDX12_Init(m_Device, APP_NUM_FRAMES_IN_FLIGHT,
	// DXGI_FORMAT_R8G8B8A8_UNORM, m_SrvDescHeap,
	// m_SrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
	// m_SrvDescHeap->GetGPUDescriptorHandleForHeapStart());


	MainLoop(&m_io);

	WaitForPendingOperations();

	// Cleanup
	Cleanup();

	return 0;
}


void OpenWindow(_In_ HINSTANCE hInstance) {


	m_cmdArgs->Open();

	m_window->WMCreateWindow(hInstance, m_cmdArgs);

	// Initialize Direct3D
	if (!CreateDeviceD3D(m_window->GetHWND())) {
		CleanupDeviceD3D();
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
		if ((m_SwapChainOccluded &&
			 m_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) ||
			::IsIconic(m_window->GetHWND())) {
			::Sleep(10);
			continue;
		}
		m_SwapChainOccluded = false;

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
		FrameContext* frameCtx		= WaitForNextFrameContext();
		UINT		  backBufferIdx = m_pSwapChain->GetCurrentBackBufferIndex();
		frameCtx->CommandAllocator->Reset();

		// Transition to render target state
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource   = m_mainRenderTargetResource[backBufferIdx].get();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;

		m_CommandList->Reset(frameCtx->CommandAllocator.get(), nullptr);
		m_CommandList->ResourceBarrier(1, &barrier);

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
		m_CommandList->ClearRenderTargetView(m_mainRenderTargetDescriptor[backBufferIdx],
											 clear_color_with_alpha, 0, nullptr);

		// Set render target and descriptor heaps
		m_CommandList->OMSetRenderTargets(1, &m_mainRenderTargetDescriptor[backBufferIdx], FALSE,
										  nullptr);

		ID3D12DescriptorHeap* ppHeaps[] = {m_SrvDescHeap.get()};
		m_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		// Render ImGui draw data
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_CommandList.get());

		// Transition to present state
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
		m_CommandList->ResourceBarrier(1, &barrier);
		m_CommandList->Close();

		// Execute command list
		ID3D12CommandList* ppCommandLists[] = {m_CommandList.get()};
		m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		m_CommandQueue->Signal(m_fence.get(), ++m_fenceLastSignaledValue);
		frameCtx->FenceValue = m_fenceLastSignaledValue;

		// Present
		HRESULT hr			= m_pSwapChain->Present(1, 0);
		m_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
		g_frameIndex++;
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

	CleanupDeviceD3D();
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

// Helper functions

bool CreateDeviceD3D(HWND hWnd) {
	// Setup swap chain
	// This is a basic setup. Optimally could handle fullscreen mode differently.
	// See #8979 for suggestions.
	DXGI_SWAP_CHAIN_DESC1 sd;
	{
		// Zero out the structure to ensure clean values
		ZeroMemory(&sd, sizeof(sd));

		// Number of back buffers (double or triple buffering)
		sd.BufferCount = APP_NUM_BACK_BUFFERS;

		// Width = 0 and Height = 0 means "use the m_window size automatically"
		// DirectX will detect the correct client area size
		sd.Width  = 0;
		sd.Height = 0;

		// Pixel format: RGBA with 8 bits per channel, unnormalized
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		// Flag that allows using waitable object for frame latency synchronization
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

		// Buffer usage: as render target (rendering destination)
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		// Sample Count = 1 means no multisampling anti-aliasing (MSAA)
		// Quality = 0 is the default quality level
		sd.SampleDesc.Count	  = 1;
		sd.SampleDesc.Quality = 0;

		// DXGI_SWAP_EFFECT_FLIP_DISCARD is the most modern and efficient mode
		// It discards the previous buffer content when flipping
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		// DXGI_ALPHA_MODE_UNSPECIFIED means we're not using m_window transparency
		sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

		// *** FIX FOR WINDOW STRETCHING PROBLEM ***
		// BEFORE (original code that causes the problem):
		// sd.Scaling = DXGI_SCALING_STRETCH;  // ❌ Causes stretching/distortion

		// AFTER (applied fix):
		// Option 1: No scaling (most accurate, recommended)
		sd.Scaling = DXGI_SCALING_NONE;

		// Option 2: Alternative - maintains aspect ratio (if you prefer)
		// sd.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;

		// EXPLANATION OF SCALING MODES:
		//
		// DXGI_SCALING_STRETCH:
		// - Stretches the image to fill the entire m_window
		// - Can distort the image if proportions change
		// - Causes the problem you were experiencing
		//
		// DXGI_SCALING_NONE:
		// - Does not apply any scaling
		// - Image is rendered at 1:1 pixel ratio
		// - Most accurate and without distortions
		// - Recommended for most cases
		//
		// DXGI_SCALING_ASPECT_RATIO_STRETCH:
		// - Stretches the image while maintaining original aspect ratio
		// - Adds black bars if necessary
		// - Useful if you want to maintain fixed aspect ratio

		// Not using stereo mode (stereoscopic 3D)
		sd.Stereo = FALSE;
	}

	// [DEBUG] Enable debug interface
#ifdef DX12_ENABLE_DEBUG_LAYER
	ID3D12Debug* pdx12Debug = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
		pdx12Debug->EnableDebugLayer();
#endif

	// Create device
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	if (D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&m_Device)) != S_OK) return false;

	// [DEBUG] Setup debug interface to break on any warnings/errors
#ifdef DX12_ENABLE_DEBUG_LAYER
	if (pdx12Debug != nullptr) {
		ID3D12InfoQueue* pInfoQueue = nullptr;
		m_Device->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		// Disable breaking on this warning because of a suspected bug in the D3D12
		// SDK layer, see #9084 for details.
		const int D3D12_MESSAGE_ID_FENCE_ZERO_WAIT_ = 1424; // not in all copies of d3d12sdklayers.h
		D3D12_MESSAGE_ID disabledMessages[]			= {
			static_cast<D3D12_MESSAGE_ID>(D3D12_MESSAGE_ID_FENCE_ZERO_WAIT_)};
		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs		   = 1;
		filter.DenyList.pIDList		   = disabledMessages;
		pInfoQueue->AddStorageFilterEntries(&filter);

		pInfoQueue->Release();
		pdx12Debug->Release();
	}
#endif

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type						= D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors				= APP_NUM_BACK_BUFFERS;
		desc.Flags						= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask					= 1;
		if (m_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_RtvDescHeap)) != S_OK)
			return false;

		SIZE_T rtvDescriptorSize =
			m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RtvDescHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < APP_NUM_BACK_BUFFERS; i++) {
			m_mainRenderTargetDescriptor[i] = rtvHandle;
			rtvHandle.ptr += rtvDescriptorSize;
		}
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type						= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors				= APP_SRV_HEAP_SIZE;
		desc.Flags						= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (m_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_SrvDescHeap.put())) != S_OK)
			return false;
		m_HeapAlloc->Create(m_Device.get(), m_SrvDescHeap.get());
	}

	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type					  = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Flags					  = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask				  = 1;
		if (m_Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_CommandQueue)) != S_OK)
			return false;
	}

	for (UINT i = 0; i < APP_NUM_FRAMES_IN_FLIGHT; i++)
		if (m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
											 IID_PPV_ARGS(&m_frameContext[i].CommandAllocator)) !=
			S_OK)
			return false;

	if (m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
									m_frameContext[0].CommandAllocator.get(), nullptr,
									IID_PPV_ARGS(&m_CommandList)) != S_OK ||
		m_CommandList->Close() != S_OK)
		return false;

	if (m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)) != S_OK)
		return false;

	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr) return false;

	{
		IDXGIFactory5*	 dxgiFactory = nullptr;
		IDXGISwapChain1* swapChain1	 = nullptr;
		if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK) return false;

		BOOL allow_tearing = FALSE;
		dxgiFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allow_tearing,
										 sizeof(allow_tearing));
		m_SwapChainTearingSupport = (allow_tearing == TRUE);
		if (m_SwapChainTearingSupport) sd.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

		if (dxgiFactory->CreateSwapChainForHwnd(m_CommandQueue.get(), hWnd, &sd, nullptr, nullptr,
												&swapChain1) != S_OK)
			return false;
		if (swapChain1->QueryInterface(IID_PPV_ARGS(&m_pSwapChain)) != S_OK) return false;
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

void CleanupDeviceD3D() {
	CleanupRenderTarget();

	if (m_hSwapChainWaitableObject != nullptr) {
		CloseHandle(m_hSwapChainWaitableObject);
		m_hSwapChainWaitableObject = nullptr;
	}


	if (m_fenceEvent) { CloseHandle(m_fenceEvent); }
}

void CreateRenderTarget() {
	for (UINT i = 0; i < APP_NUM_BACK_BUFFERS; i++) {
		ComPtr<ID3D12Resource> pBackBuffer;
		m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(pBackBuffer.put()));
		m_Device->CreateRenderTargetView(pBackBuffer.get(), nullptr,
										 m_mainRenderTargetDescriptor[i]);
		m_mainRenderTargetResource[i] = pBackBuffer;
	}
}

void CleanupRenderTarget() {
	WaitForPendingOperations();

	for (UINT i = 0; i < APP_NUM_BACK_BUFFERS; i++)
		if (m_mainRenderTargetResource[i]) { m_mainRenderTargetResource[i].detach(); }
}

void WaitForPendingOperations() {
	m_CommandQueue->Signal(m_fence.get(), ++m_fenceLastSignaledValue);

	m_fence->SetEventOnCompletion(m_fenceLastSignaledValue, m_fenceEvent);
	::WaitForSingleObject(m_fenceEvent, INFINITE);
}

FrameContext* WaitForNextFrameContext() {
	FrameContext* m_frame_context = &m_frameContext[g_frameIndex % APP_NUM_FRAMES_IN_FLIGHT];
	if (m_fence->GetCompletedValue() < m_frame_context->FenceValue) {
		m_fence->SetEventOnCompletion(m_frame_context->FenceValue, m_fenceEvent);
		HANDLE waitableObjects[] = {m_hSwapChainWaitableObject, m_fenceEvent};
		::WaitForMultipleObjects(2, waitableObjects, TRUE, INFINITE);
	} else ::WaitForSingleObject(m_hSwapChainWaitableObject, INFINITE);

	return m_frame_context;
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
			if (m_Device != nullptr && wParam != SIZE_MINIMIZED) {
				CleanupRenderTarget();
				DXGI_SWAP_CHAIN_DESC1 desc = {};
				m_pSwapChain->GetDesc1(&desc);
				HRESULT result = m_pSwapChain->ResizeBuffers(0, static_cast<UINT> LOWORD(lParam),
															 static_cast<UINT> HIWORD(lParam),
															 desc.Format, desc.Flags);
				IM_ASSERT(FAILED(result) && "Failed to resize swapchain.");
				CreateRenderTarget();
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
