#pragma once

#include "PCH.hpp"

namespace app {

// Simple free list based allocator
struct ExampleDescriptorHeapAllocator {

	ExampleDescriptorHeapAllocator();
	virtual ~ExampleDescriptorHeapAllocator();

	ID3D12DescriptorHeap	   *Heap;
	D3D12_DESCRIPTOR_HEAP_TYPE	HeapType;
	D3D12_CPU_DESCRIPTOR_HANDLE HeapStartCpu;
	D3D12_GPU_DESCRIPTOR_HANDLE HeapStartGpu;
	UINT						HeapHandleIncrement;
	ImVector<int>				FreeIndices;

	void Create(ID3D12Device *device, ID3D12DescriptorHeap *heap);
	void Destroy();
	void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE *out_cpu_desc_handle,
			   D3D12_GPU_DESCRIPTOR_HANDLE *out_gpu_desc_handle);
	void Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle,
			  D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle);
};
} // namespace app
