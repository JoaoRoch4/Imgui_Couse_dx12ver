#include "PCH.hpp"
#include "Classes.hpp"
#include "Source.hpp"

static UPtr<MemoryManagement> memory;


// Data
ExampleDescriptorHeapAllocator *g_pd3dSrvDescHeapAlloc;

ComPtr<ID3D12Device>			   m_Device;
ComPtr<ID3D12DescriptorHeap>	   m_RtvDescHeap;
ComPtr<ID3D12DescriptorHeap>	   m_SrvDescHeap;
ComPtr<ID3D12CommandQueue>		   m_CommandQueue;
ComPtr<ID3D12GraphicsCommandList>  m_CommandList;
ComPtr<ID3D12Fence>				   m_fence;
ComPtr<IDXGISwapChain3>			   m_pSwapChain;
ComPtr<ID3D12Resource>			   m_mainRenderTargetResource[APP_NUM_BACK_BUFFERS];
static D3D12_CPU_DESCRIPTOR_HANDLE m_mainRenderTargetDescriptor[APP_NUM_BACK_BUFFERS] = {};


static FrameContext g_frameContext[APP_NUM_FRAMES_IN_FLIGHT];

int Start(_In_ HINSTANCE hInstance) {

	SetConsoleOutputCP(CP_UTF8);
    std::wcout.imbue(std::locale(""));

    std::wcout << L"=== Application Starting ===" << std::endl;


	memory = std::make_unique<MemoryManagement>();
	memory->AllocAll();

	// Main code
	g_pd3dSrvDescHeapAlloc = memory->Get_ExampleDescriptorHeapAllocator();
	WindowManager *window  = memory->Get_WindowManager();

	CommandLineArguments *cmdArgs = memory->Get_CommandLineArguments();

    std::wcout << L"Memory management initialized" << std::endl;


	OpenWindow(hInstance, cmdArgs, window);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	static_cast<void>(io);
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();

	// Setup scaling
	ImGuiStyle &style = ImGui::GetStyle();
	style.ScaleAllSizes(
		window->get_main_scale()); // Bake a fixed style scale. (until we have a solution for
	// dynamic style scaling, changing this requires resetting
	// Style + calling this again)
	style.FontScaleDpi = window->get_main_scale() + 0.2f; // Set initial font scale. (using
	// io.ConfigDpiScaleFonts=true makes this unnecessary.
	// We leave both here for documentation purpose)

	style.Alpha			  = 0.90f;
	style.FrameRounding	  = 4.0f;
	style.FramePadding	  = ImVec2(4.0f, 3.0f);
	style.FrameBorderSize = 1.0f;
	style.WindowRounding  = 5.0f;

	ImPlot::CreateContext();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(window->GetHWND());

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
	init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo *,
										D3D12_CPU_DESCRIPTOR_HANDLE *out_cpu_handle,
										D3D12_GPU_DESCRIPTOR_HANDLE *out_gpu_handle) {
		return g_pd3dSrvDescHeapAlloc->Alloc(out_cpu_handle, out_gpu_handle);
	};
	init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo *,
									   D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle,
									   D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) {
		return g_pd3dSrvDescHeapAlloc->Free(cpu_handle, gpu_handle);
	};
	ImGui_ImplDX12_Init(&init_info);

	// Before 1.91.6: our signature was using a single descriptor. From 1.92,
	// specifying SrvDescriptorAllocFn/SrvDescriptorFreeFn will be required to
	// benefit from new features.
	// ImGui_ImplDX12_Init(m_Device, APP_NUM_FRAMES_IN_FLIGHT,
	// DXGI_FORMAT_R8G8B8A8_UNORM, m_SrvDescHeap,
	// m_SrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
	// m_SrvDescHeap->GetGPUDescriptorHandleForHeapStart());


	MainLoop(&io, window);

	WaitForPendingOperations();

	// Cleanup
	Cleanup(window);

	return 0;
}


void OpenWindow(_In_ HINSTANCE hInstance, CommandLineArguments *cmdArgs, WindowManager *window) {


	cmdArgs->Open();

	window->WMCreateWindow(hInstance, cmdArgs);

	// Initialize Direct3D
	if (!CreateDeviceD3D(window->GetHWND())) {
		CleanupDeviceD3D();
		::UnregisterClassW(window->GetWc()->lpszClassName, window->GetWc()->hInstance);
		throw std::runtime_error("failed CreateDeviceD3D");
	}

	// Show the window
	::ShowWindow(window->GetHWND(), SW_SHOWMAXIMIZED);
	::UpdateWindow(window->GetHWND());
}

void MainLoop(ImGuiIO *io, WindowManager *window) {

	auto font_manager = std::make_unique<FontManager>(io);
	//font_manager->GetIo(io);
	auto font_manager_window =
		std::make_unique<FontManagerWindow>(font_manager.get(), window->GetHWND());
	auto debug_window = std::make_unique<DebugWindow>(io);


	font_manager->LoadFonts();
	font_manager->SetDefaultFont();

	// Our state


	ImVec4 clear_color = ImVec4(0.15f, 0.15f, 0.15f, 1.f);

	WindowClass window_obj;

	// Main loop
	bool done = false;
	while (!done) {
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32
		// backend.
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT) done = true;
		}
		if (done) break;

		// Handle window screen locked
		if ((m_SwapChainOccluded &&
			 m_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) ||
			::IsIconic(window->GetHWND())) {
			::Sleep(10);
			continue;
		}
		m_SwapChainOccluded = false;

		// Start the Dear ImGui frame
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (memory->bShow_FileSys_window) { render(window_obj); }

		if (memory->bShow_Debug_window) debug_window->Tick();

		if (memory->bShow_FontManager_window) font_manager_window->Render();

		if (memory->bShow_styleEditor_window) ImGui::ShowStyleEditor();

		// 1. Show the big demo window (Most of the sample code is in
		// ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
		// ImGui!).
		if (memory->bShow_demo_window) ImGui::ShowDemoWindow(&memory->bShow_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair
		// to create a named window.
		{
			static float f		 = 0.0f;
			static int	 counter = 0;

			ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!"
			ShowExampleAppMainMenuBar();

			// and append into it.
			ig::Separator();

			ImGui::Text("This is some useful text."); // Display some text (you can

			{
				// use a format strings too)
				ig::Separator();
				ImGui::Checkbox("Demo Window", &memory->bShow_demo_window);
				ImGui::Checkbox("Another Window", &memory->bShow_another_window);
				ImGui::Checkbox("Style Editor", &memory->bShow_styleEditor_window);

				ig::Separator();
				ig::Spacing();
				ImGui::Checkbox("Debug Window", &memory->bShow_Debug_window);
				ImGui::Checkbox("Font Manager Window", &memory->bShow_FontManager_window);
				ImGui::Checkbox("File System Window", &memory->bShow_FileSys_window);
				ig::Spacing();
				ig::Separator();
				ig::Spacing();
			}


			// Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

			ig::Separator();

			// Edit 3 floats representing a color
			ImGui::ColorEdit3("clear color", std::bit_cast<float *>(std::ref(clear_color)));

			ig::Separator();

			// Buttons return true when clicked (most
			// widgets return true when edited/activated)
			if (ImGui::Button("--")) counter--;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);
			ImGui::SameLine();
			if (ImGui::Button("++")) counter++;
			ImGui::SameLine();
			if (ImGui::Button("Reset")) counter = 0;

			ImGui::End();
		}

		// 3. Show another simple window.
		if (memory->bShow_another_window) {
			// Pass a pointer to our bool variable (the
			// window will have a closing button that will
			// clear the bool when clicked)
			ImGui::Begin("Another Window", &memory->bShow_another_window);
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me")) memory->bShow_another_window = false;
			ImGui::End();
		}


		// Rendering
		ImGui::Render();

		FrameContext *frameCtx		= WaitForNextFrameContext();
		UINT		  backBufferIdx = m_pSwapChain->GetCurrentBackBufferIndex();
		frameCtx->CommandAllocator->Reset();

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource   = m_mainRenderTargetResource[backBufferIdx].get();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
		m_CommandList->Reset(frameCtx->CommandAllocator.get(), nullptr);
		m_CommandList->ResourceBarrier(1, &barrier);

		// Render Dear ImGui graphics
		const float clear_color_with_alpha[4] = {clear_color.x * clear_color.w,
												 clear_color.y * clear_color.w,
												 clear_color.z * clear_color.w, clear_color.w};
		m_CommandList->ClearRenderTargetView(m_mainRenderTargetDescriptor[backBufferIdx],
											 clear_color_with_alpha, 0, nullptr);
		m_CommandList->OMSetRenderTargets(1, &m_mainRenderTargetDescriptor[backBufferIdx], FALSE,
										  nullptr);
		ID3D12DescriptorHeap *ppHeaps[] = {m_SrvDescHeap.get()};
		m_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_CommandList.get());
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
		m_CommandList->ResourceBarrier(1, &barrier);
		m_CommandList->Close();

		ID3D12CommandList *ppCommandLists[] = {m_CommandList.get()};

		m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		m_CommandQueue->Signal(m_fence.get(), ++m_fenceLastSignaledValue);
		frameCtx->FenceValue = m_fenceLastSignaledValue;

		// Present
		HRESULT hr			= m_pSwapChain->Present(1, 0); // Present with vsync
		// HRESULT hr = m_pSwapChain->Present(0, m_SwapChainTearingSupport ?
		// DXGI_PRESENT_ALLOW_TEARING : 0); // Present without vsync
		m_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
		g_frameIndex++;
	}
}

void Cleanup(WindowManager *window) {

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(window->GetHWND());
	::UnregisterClassW(window->GetWc()->lpszClassName, window->GetWc()->hInstance);

	ImPlot::DestroyContext();
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

		// Width = 0 and Height = 0 means "use the window size automatically"
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

		// DXGI_ALPHA_MODE_UNSPECIFIED means we're not using window transparency
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
		// - Stretches the image to fill the entire window
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
	ID3D12Debug *pdx12Debug = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
		pdx12Debug->EnableDebugLayer();
#endif

	// Create device
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	if (D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&m_Device)) != S_OK) return false;

	// [DEBUG] Setup debug interface to break on any warnings/errors
#ifdef DX12_ENABLE_DEBUG_LAYER
	if (pdx12Debug != nullptr) {
		ID3D12InfoQueue *pInfoQueue = nullptr;
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
		g_pd3dSrvDescHeapAlloc->Create(m_Device.get(), m_SrvDescHeap.get());
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
											 IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) !=
			S_OK)
			return false;

	if (m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
									g_frameContext[0].CommandAllocator.get(), nullptr,
									IID_PPV_ARGS(&m_CommandList)) != S_OK ||
		m_CommandList->Close() != S_OK)
		return false;

	if (m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)) != S_OK)
		return false;

	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr) return false;

	{
		IDXGIFactory5	*dxgiFactory = nullptr;
		IDXGISwapChain1 *swapChain1	 = nullptr;
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

FrameContext *WaitForNextFrameContext() {
	FrameContext *frame_context = &g_frameContext[g_frameIndex % APP_NUM_FRAMES_IN_FLIGHT];
	if (m_fence->GetCompletedValue() < frame_context->FenceValue) {
		m_fence->SetEventOnCompletion(frame_context->FenceValue, m_fenceEvent);
		HANDLE waitableObjects[] = {m_hSwapChainWaitableObject, m_fenceEvent};
		::WaitForMultipleObjects(2, waitableObjects, TRUE, INFINITE);
	} else ::WaitForSingleObject(m_hSwapChainWaitableObject, INFINITE);

	return frame_context;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam,
															 LPARAM lParam);

// dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your
// main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to
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

void ShowExampleAppMainMenuBar() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			ShowExampleMenuFile();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
			if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {} // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "Ctrl+X")) {}
			if (ImGui::MenuItem("Copy", "Ctrl+C")) {}
			if (ImGui::MenuItem("Paste", "Ctrl+V")) {}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void ShowExampleMenuFile() {
	ImGui::MenuItem("(demo menu)", NULL, false, false);
	if (ImGui::MenuItem("New")) {}
	if (ImGui::MenuItem("Open", "Ctrl+O")) {}
	if (ImGui::BeginMenu("Open Recent")) {
		ImGui::MenuItem("fish_hat.c");
		ImGui::MenuItem("fish_hat.inl");
		ImGui::MenuItem("fish_hat.h");
		if (ImGui::BeginMenu("More..")) {
			ImGui::MenuItem("Hello");
			ImGui::MenuItem("Sailor");
			if (ImGui::BeginMenu("Recurse..")) {
				ShowExampleMenuFile();
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("Save", "Ctrl+S")) {}
	if (ImGui::MenuItem("Save As..")) {}

	ImGui::Separator();
	if (ImGui::BeginMenu("Options")) {
		static bool enabled = true;
		ImGui::MenuItem("Enabled", "", &enabled);
		ImGui::BeginChild("child", ImVec2(0, 60), ImGuiChildFlags_Borders);
		for (int i = 0; i < 10; i++) ImGui::Text("Scrolling Text %d", i);
		ImGui::EndChild();
		static float f = 0.5f;
		static int	 n = 0;
		ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
		ImGui::InputFloat("Input", &f, 0.1f);
		ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Colors")) {
		float sz = ImGui::GetTextLineHeight();
		for (int i = 0; i < ImGuiCol_COUNT; i++) {
			const char *name = ImGui::GetStyleColorName((ImGuiCol)i);
			ImVec2		p	 = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz),
													  ImGui::GetColorU32((ImGuiCol)i));
			ImGui::Dummy(ImVec2(sz, sz));
			ImGui::SameLine();
			ImGui::MenuItem(name);
		}
		ImGui::EndMenu();
	}

	// Here we demonstrate appending again to the "Options" menu (which we already created above)
	// Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
	// In a real code-base using it would make senses to use this feature from very different code locations.
	if (ImGui::BeginMenu("Options")) // <-- Append!
	{
		static bool b = true;
		ImGui::Checkbox("SomeOption", &b);
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Disabled", false)) // Disabled
	{
		IM_ASSERT(0);
	}
	if (ImGui::MenuItem("Checked", NULL, true)) {}
	ImGui::Separator();
	if (ImGui::MenuItem("Quit", "Alt+F4")) { exit(EXIT_SUCCESS); }
}
