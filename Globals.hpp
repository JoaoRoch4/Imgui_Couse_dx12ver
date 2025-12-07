#pragma once
#include "PCH.hpp"

// Config for example app
constexpr static const int APP_NUM_FRAMES_IN_FLIGHT = 2;
constexpr static const int APP_NUM_BACK_BUFFERS = 2;
constexpr static const int APP_SRV_HEAP_SIZE = 64;

 auto WINDOW_WIDTH = std::uint32_t{2000};
 auto WINDOW_HEIGHT = std::uint32_t{1000};

class Globals {};

inline std::string HrToString(HRESULT hr)
{
    char s_str[64] = {};
    sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
    return std::string(s_str);
}


