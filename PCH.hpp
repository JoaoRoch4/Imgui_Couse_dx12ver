#pragma once

#include <iostream>
#include <algorithm>
#include <bit>
#include <chrono>
#include <exception>
#include <map>
#include <memory>
#include <filesystem>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>
#include <type_traits>
#include <thread>

#include <cstdint>
#include <cstdio>
#include <cctype>

#include <stdint.h>
#include <stdio.h>

#include <windows.h>
#include <tchar.h>
#include <dwmapi.h> // Desktop Window Manager API (for dark mode)
#include <commdlg.h>     // Common Dialogs - for file open/save dialogs
#include <shlobj.h>      // Shell Objects - for folder browser dialog

#define DX12_ENABLE_DEBUG_LAYER
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi.h>
#include <dxgidebug.h>
#include <dxgi1_4.h>
#include <dxgi1_5.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <pix.h>
#include <shellapi.h>

#include <freetype\freetype.h>

#include "imconfig.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_freetype.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "imgui_stdlib.h"

#include "implot.h"
#include "implot_internal.h"

#include "plutovg.h"
#include "plutosvg.h"
#include "plutosvg-ft.h"



using namespace Microsoft::WRL;
namespace fs = std::filesystem;
namespace ig = ImGui;

using str = std::string;
