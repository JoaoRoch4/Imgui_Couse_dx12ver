#pragma once

#include <algorithm>
#include <atomic>
#include <bit>
#include <chrono>
#include <exception>
#include <filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include <cctype>
#include <cstdint>
#include <cstdio>

#include <stdint.h>
#include <stdio.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>     // Common Dialogs - for file open/save dialogs
#include <dwmapi.h> // Desktop Window Manager API (for dark mode)
#include <shlobj.h>      // Shell Objects - for folder browser dialog
#include <tchar.h>

#define DX12_ENABLE_DEBUG_LAYER
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi.h>
#include <dxgidebug.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <pix.h>
#include <shellapi.h>

#include <freetype\freetype.h>

#include "imconfig.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_freetype.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"



#include "implot.h"
#include "implot_internal.h"

#include "plutovg.h"
#include "plutosvg.h"
#include "plutosvg-ft.h"

#include <winrt/base.h>

template <typename T>
using ComPtr = winrt::com_ptr<T>;

namespace fs = std::filesystem;
namespace ig = ImGui;

using str = std::string;

template <typename T>
using UPtr = std::unique_ptr<T>;

