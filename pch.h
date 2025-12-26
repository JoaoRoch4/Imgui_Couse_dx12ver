#pragma once

// Standard C++ headers
#include <string>
#include <memory>
#include <stdexcept>

// Windows headers
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <unknwn.h>

// WinRT base headers (only if available)
#if __has_include(<winrt/Windows.Foundation.h>)
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#define WINRT_AVAILABLE
#endif

// WinUI headers (only if available)
#if __has_include(<winrt/Microsoft.UI.Xaml.h>)
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#define WINUI_AVAILABLE
#endif

