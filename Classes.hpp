#pragma once
#include "CommandLineArgumments.hpp"
#include "DebugWindow.hpp"
#include "ExampleDescriptorHeapAllocator.hpp"
#include "FontManager.hpp"
#include "FontManagerWindow.hpp"
#include "FrameContext.hpp"
//#include "Globals.hpp"
#include "render.hpp"
#include "WindowManager.hpp"


constexpr static const int APP_NUM_FRAMES_IN_FLIGHT = 2;
constexpr static const int APP_NUM_BACK_BUFFERS = 2;
constexpr static const int APP_SRV_HEAP_SIZE = 64;

auto WINDOW_WIDTH = std::uint32_t{2000};
auto WINDOW_HEIGHT = std::uint32_t{1000};

bool bShowConsole;
