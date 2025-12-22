#include "PCH.hpp"
#include "DarkMode.hpp"

bool ApplyDarkModeToTitleBar(HWND hwnd, bool enable) {
    // Check if the window handle is valid
    // nullptr indicates an invalid or uninitialized window
    if (hwnd == nullptr) {
        // Invalid handle, return failure immediately
        // Cannot apply theme to a non-existent window
        return false;
    }

    // Convert C++ boolean to Windows BOOL type
    // BOOL is a 32-bit integer type defined by Windows API
    // TRUE (1) = dark mode enabled
    // FALSE (0) = dark mode disabled
    // Note: Windows BOOL is different from C++ bool (4 bytes vs 1 byte)
    BOOL useDarkMode = enable ? TRUE : FALSE;

    // Call the DwmSetWindowAttribute API to set the attribute
    // This function modifies Desktop Window Manager (DWM) properties
    // for the specified window
    //
    // Parameters breakdown:
    //   1. hwnd: Window handle to modify
    //   2. DWMWA_USE_IMMERSIVE_DARK_MODE: The attribute identifier (value: 20)
    //      This tells DWM which property we want to change
    //   3. &useDarkMode: Pointer to the value we want to set (TRUE or FALSE)
    //      Must be a pointer because the API needs to read from memory
    //   4. sizeof(useDarkMode): Size of the value in bytes (4 bytes for BOOL)
    //      Tells the API how many bytes to read from the pointer
    //
    // Return value:
    //   S_OK (0): Success - the attribute was set
    //   Error HRESULT: Failure - may indicate unsupported Windows version
    //                  or other DWM error
    HRESULT hr =
        DwmSetWindowAttribute(hwnd,                          // Window handle to modify
                              DWMWA_USE_IMMERSIVE_DARK_MODE, // Attribute: use immersive dark mode
                              &useDarkMode,                  // Value pointer: TRUE or FALSE
                              sizeof(useDarkMode)            // Value size: 4 bytes (sizeof BOOL)
        );

    // SUCCEEDED is a Windows macro that checks if HRESULT indicates success
    // Definition: #define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
    // Returns true if hr >= 0, false otherwise
    // S_OK = 0x00000000 (success)
    // E_FAIL = 0x80004005 (generic failure)
    // E_INVALIDARG = 0x80070057 (invalid argument)
    return SUCCEEDED(hr);
}
