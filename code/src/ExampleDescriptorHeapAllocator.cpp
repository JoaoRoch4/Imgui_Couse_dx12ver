#include "PCH.hpp"

#include "ExampleDescriptorHeapAllocator.hpp"

namespace app {

ExampleDescriptorHeapAllocator::ExampleDescriptorHeapAllocator()
: Heap(nullptr),
  HeapType(D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES),
  HeapStartCpu(),
  HeapStartGpu(),
  HeapHandleIncrement(),
  FreeIndices() {}

ExampleDescriptorHeapAllocator::~ExampleDescriptorHeapAllocator() {}

void ExampleDescriptorHeapAllocator::Create(ID3D12Device *device, ID3D12DescriptorHeap *heap) {

	IM_ASSERT(Heap == nullptr && FreeIndices.empty());
	Heap							= heap;
	D3D12_DESCRIPTOR_HEAP_DESC desc = heap->GetDesc();
	HeapType						= desc.Type;
	HeapStartCpu					= Heap->GetCPUDescriptorHandleForHeapStart();
	HeapStartGpu					= Heap->GetGPUDescriptorHandleForHeapStart();
	HeapHandleIncrement				= device->GetDescriptorHandleIncrementSize(HeapType);
	FreeIndices.reserve((int)desc.NumDescriptors);
	for (int n = desc.NumDescriptors; n > 0; n--) FreeIndices.push_back(n - 1);
}

void ExampleDescriptorHeapAllocator::Destroy() {

	Heap = nullptr;
	FreeIndices.clear();
}

void ExampleDescriptorHeapAllocator::Alloc(D3D12_CPU_DESCRIPTOR_HANDLE *out_cpu_desc_handle,
										   D3D12_GPU_DESCRIPTOR_HANDLE *out_gpu_desc_handle) {

	IM_ASSERT(FreeIndices.Size > 0);
	int idx = FreeIndices.back();
	FreeIndices.pop_back();
	out_cpu_desc_handle->ptr = HeapStartCpu.ptr + static_cast<SIZE_T>(idx * HeapHandleIncrement);
	out_gpu_desc_handle->ptr = HeapStartGpu.ptr + static_cast<SIZE_T>(idx * HeapHandleIncrement);
}

void ExampleDescriptorHeapAllocator::Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle,
										  D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle) {

	int cpu_idx =
		static_cast<int>((out_cpu_desc_handle.ptr - HeapStartCpu.ptr) / HeapHandleIncrement);
	int gpu_idx =
		static_cast<int>((out_gpu_desc_handle.ptr - HeapStartGpu.ptr) / HeapHandleIncrement);
	IM_ASSERT(cpu_idx == gpu_idx);
	FreeIndices.push_back(cpu_idx);
}
} // namespace app
