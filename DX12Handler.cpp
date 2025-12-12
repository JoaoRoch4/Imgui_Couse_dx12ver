//#include "DX12Handler.hpp"
//#include "PCH.hpp"
//
//DX12Handler::DX12Handler() {
//
//	m_SrvDescHeapAlloc = std::make_unique<ExampleDescriptorHeapAllocator>();
//}
//
//DX12Handler::~DX12Handler() {
//}
//
//void DX12Handler::GetHardwareAdapter(IDXGIFactory1 *pFactory,
//									 IDXGIAdapter1 **ppAdapter,
//									 bool requestHighPerformanceAdapter) {
//	*ppAdapter = nullptr;
//
//	ComPtr<IDXGIAdapter1> adapter;
//	ComPtr<IDXGIFactory6> factory6;
//
//	if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6)))) {
//		for (UINT adapterIndex = 0; SUCCEEDED(factory6->EnumAdapterByGpuPreference(
//				 adapterIndex,
//				 requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE
//													   : DXGI_GPU_PREFERENCE_UNSPECIFIED,
//				 IID_PPV_ARGS(&adapter)));
//			 ++adapterIndex) {
//			DXGI_ADAPTER_DESC1 desc;
//			adapter->GetDesc1(&desc);
//
//			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
//				// Don't select the Basic Render Driver adapter.
//				// If you want a software adapter, pass in "/warp" on the command line.
//				continue;
//			}
//
//			// Check to see whether the adapter supports Direct3D 12, but don't create
//			// the actual device yet.
//			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(),
//											D3D_FEATURE_LEVEL_11_0,
//											_uuidof(ID3D12Device),
//											nullptr))) {
//				break;
//			}
//		}
//	}
//
//	if (adapter.Get() == nullptr) {
//		for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter));
//			 ++adapterIndex) {
//			DXGI_ADAPTER_DESC1 desc;
//			adapter->GetDesc1(&desc);
//
//			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
//				// Don't select the Basic Render Driver adapter.
//				// If you want a software adapter, pass in "/warp" on the command line.
//				continue;
//			}
//
//			// Check to see whether the adapter supports Direct3D 12, but don't create
//			// the actual device yet.
//			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(),
//											D3D_FEATURE_LEVEL_11_0,
//											_uuidof(ID3D12Device),
//											nullptr))) {
//				break;
//			}
//		}
//	}
//
//	*ppAdapter = adapter.Detach();
//}
//
//void DX12Handler::ThrowIfFailed(HRESULT hr) {
//	if (FAILED(hr)) {
//		throw std::runtime_error(HrToString(hr));
//	}
//}
//
//std::string DX12Handler::HrToString(HRESULT hr) {
//	char s_str[64] = {};
//	sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
//	return std::string(s_str);
//}
//
//void DX12Handler::LoadPipeLine()
//
//{
//	UINT dxgiFactoryFlags = 0;
//
//#if defined(_DEBUG)
//	// Enable the debug layer (requires the Graphics Tools "optional feature").
//	// NOTE: Enabling the debug layer after device creation will invalidate the
//	// active device.
//	{
//		ComPtr<ID3D12Debug> debugController;
//		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
//			debugController->EnableDebugLayer();
//
//			// Enable additional debug layers.
//			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
//		}
//	}
//#endif
//
//	ComPtr<IDXGIFactory4> factory;
//
//	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
//
//	ComPtr<IDXGIAdapter1> hardwareAdapter;
//	GetHardwareAdapter(factory.Get(), hardwareAdapter.ReleaseAndGetAddressOf(), true);
//
//	ThrowIfFailed(
//		D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)));
//
//	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
//	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
//
//	ThrowIfFailed(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));
//
//	// Describe and create the swap chain.
//	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
//	swapChainDesc.BufferCount = APP_NUM_BACK_BUFFERS;
//	swapChainDesc.Width = WINDOW_WIDTH;
//	swapChainDesc.Height = WINDOW_HEIGHT;
//	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
//	swapChainDesc.SampleDesc.Count = 1;
//
//	ComPtr<IDXGISwapChain1> swapChain;
//	// Swap chain needs the queue so that it can force a flush on it.
//	ThrowIfFailed(factory->CreateSwapChainForHwnd(m_CommandQueue.Get(),
//												  WindowManager().GetHWND(),
//												  &swapChainDesc,
//												  nullptr,
//												  nullptr,
//												  &swapChain));
//
//	// This sample does not support fullscreen transitions.
//	ThrowIfFailed(
//		factory->MakeWindowAssociation(WindowManager().GetHWND(), DXGI_MWA_NO_ALT_ENTER));
//
//	ThrowIfFailed(swapChain.As(&m_pSwapChain));
//	m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
//
//	// Create descriptor heaps.
//	{
//		// Describe and create a render target view (RTV) descriptor heap.
//		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
//		rtvHeapDesc.NumDescriptors = APP_NUM_BACK_BUFFERS;
//		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//		ThrowIfFailed(m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RtvDescHeap)));
//
//		m_rtvDescriptorSize =
//			m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//	}
//
//	// Create frame resources.
//	{
//		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
//			m_RtvDescHeap->GetCPUDescriptorHandleForHeapStart());
//
//		// Create a RTV for each frame.
//		for (UINT n = 0; n < APP_NUM_BACK_BUFFERS; n++) {
//			ThrowIfFailed(m_pSwapChain->GetBuffer(n, IID_PPV_ARGS(&m_mainRenderTargetResource[n])));
//			m_Device->CreateRenderTargetView(m_mainRenderTargetResource[n].Get(),
//											 nullptr,
//											 rtvHandle);
//			rtvHandle.Offset(1, m_rtvDescriptorSize);
//		}
//	}
//
//	ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
//												   IID_PPV_ARGS(&m_CommandQueue)));
//}
