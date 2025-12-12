//#pragma once
//#include "PCH.hpp"
//#include "Classes.hpp"
//
//
//
//class DX12Handler {
//
//public:
//	DX12Handler();
//	virtual ~DX12Handler();
//
//	void LoadPipeLine();
//	void GetHardwareAdapter(IDXGIFactory1 *pFactory,
//							IDXGIAdapter1 **ppAdapter,
//							bool requestHighPerformanceAdapter);
//
//	void ThrowIfFailed(HRESULT hr);
//
//private:
//	UINT m_frameIndex;
//	UINT m_rtvDescriptorSize;
//
//	HANDLE m_fenceEvent;
//	UINT64 m_fenceLastSignaledValue;
//	bool m_SwapChainTearingSupport;
//	bool m_SwapChainOccluded;
//	HANDLE m_hSwapChainWaitableObject;
//
//	std::unique_ptr<ExampleDescriptorHeapAllocator> m_SrvDescHeapAlloc;
//
//	ComPtr<ID3D12Device> m_Device;
//	ComPtr<ID3D12DescriptorHeap> m_RtvDescHeap;
//	ComPtr<ID3D12DescriptorHeap> m_SrvDescHeap;
//	ComPtr<ID3D12CommandQueue> m_CommandQueue;
//	ComPtr<ID3D12GraphicsCommandList> m_CommandList;
//	ComPtr<ID3D12Fence> m_fence;
//	ComPtr<IDXGISwapChain3> m_pSwapChain;
//	ComPtr<FrameContext> m_frameContext[APP_NUM_FRAMES_IN_FLIGHT];
//	ComPtr<ID3D12Resource> m_mainRenderTargetResource[APP_NUM_BACK_BUFFERS];
//	D3D12_CPU_DESCRIPTOR_HANDLE
//	m_mainRenderTargetDescriptor[APP_NUM_BACK_BUFFERS];
//
//   std::string HrToString(HRESULT hr);
//
//};
