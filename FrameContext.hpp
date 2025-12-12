#pragma once
#include "PCH.hpp"

struct FrameContext {
    ComPtr<ID3D12CommandAllocator> CommandAllocator;
    UINT64 FenceValue;

    FrameContext();
};
