#pragma once
#include "PCH.hpp"
namespace app {

struct FrameContext {
	ComPtr<ID3D12CommandAllocator> CommandAllocator;
	UINT64						   FenceValue;

	FrameContext();
};
} // namespace app
