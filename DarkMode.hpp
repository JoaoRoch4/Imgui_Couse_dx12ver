#pragma once

#include "PCH.hpp"

// Define the DWMWA_USE_IMMERSIVE_DARK_MODE attribute constant
// This is the enum value that controls dark mode on Windows 11
// On Windows 10 (build 19041+), the value was 19
// On Windows 11, the official value is 20
// This value tells DWM which window attribute we want to modify
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

// ApplyDarkModeToTitleBar function
// This function applies the dark theme to the window title bar
// Parameters:
//   hwnd: Handle to the window that will receive dark theme
//   enable: true to enable dark mode, false to disable (use light mode)
// Returns:
//   true: Dark mode was successfully applied
//   false: Failed to apply (invalid handle or unsupported Windows version)
// Technical details:
//   Uses DwmSetWindowAttribute to modify Desktop Window Manager attributes
//   Requires Windows 10 build 19041+ or Windows 11
//   If the Windows version doesn't support it, fails gracefully
bool ApplyDarkModeToTitleBar(HWND hwnd, bool enable = true);
