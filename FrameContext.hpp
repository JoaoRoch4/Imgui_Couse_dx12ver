#pragma once
#include "PCH.hpp"

struct FrameContext {
    ID3D12CommandAllocator *CommandAllocator;
    UINT64 FenceValue;

    FrameContext();
};
