// WindowManager.cpp
// Implementation file for WindowManager class
// Manages m_window creation and configuration with command line support

// Include precompiled header
#include "PCH.hpp"

// Include class headers
#include "Classes.hpp"
#include "WindowManager.hpp"

// Forward declaration of m_window procedure
// This function handles Windows messages for the application m_window
extern LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


namespace app {

/**
 * Constructor - Initializes all member variables
 * 
 * Initializes pointers to nullptr and numeric values to 0
 * Ensures safe state before m_window creation
 */
WindowManager::WindowManager()
: m_wc(nullptr),		 // Window class pointer (not yet allocated)
  m_windowRect(nullptr), // Window rectangle pointer (not yet allocated)
  m_hwnd(nullptr),		 // Window handle (not yet created)
  m_main_scale(0.f),     // DPI scale factor (will be calculated)
  m_windowWidth(),       // Window width in pixels
  m_windowHeigh(),       // Window height in pixels
  m_windowX_pos(),       // Window X position
  m_windowY_pos(),       // Window Y position
  m_windowStyle(),       // Window style flags
  m_scaledWidth(),       // DPI-scaled m_window width
  m_scaledHeight(),      // DPI-scaled m_window height
  m_showCommand(),       // Show m_window command (SW_SHOW, SW_SHOWMAXIMIZED, etc.)
  m_bHasAnyWindowArgs(false) {} // Flag indicating if command line args were provided

/**
* Destructor - Cleans up resources
* 
* Resets all member variables to safe default values.
* Note: HWND and WNDCLASSEX cleanup is typically handled by
* the main application cleanup code, not here
*/
WindowManager::~WindowManager() {

	m_wc		 = nullptr;
	m_windowRect = nullptr;
	m_hwnd		 = nullptr;
	m_main_scale = 0;

	m_windowWidth  = 0;
	m_windowHeigh  = 0;
	m_windowX_pos  = 0;
	m_windowY_pos  = 0;
	m_windowStyle  = 0;
	m_scaledWidth  = 0;
	m_scaledHeight = 0;
	m_showCommand  = 0;

	m_bHasAnyWindowArgs = false;
}

/**
 * Creates the main application m_window with command line arguments support
 * 
 * This version reads command line arguments to configure:
 * - Window size (width and height)
 * - Window position (x and y)
 * - Window state (maximized, fullscreen, windowed)
 * 
 * If NO arguments are passed:
 * - Automatically uses monitor resolution
 * - Starts maximized with borders
 * 
 * @param hInstance The application instance handle
 * @param m_cmdArgs Pointer to CommandLineArguments object with parsed arguments
 * @return true on success, false on failure
 */
bool WindowManager::WMCreateWindow(_In_ HINSTANCE hInstance, CommandLineArguments* m_cmdArgs) {

	// Make process DPI aware so we can handle high-DPI displays correctly
	// This ensures proper scaling on monitors with different DPI settings
	ImGui_ImplWin32_EnableDpiAwareness();

	// Get the DPI scale factor for the primary monitor
	// POINT{0, 0} refers to the top-left corner of the screen
	// MONITOR_DEFAULTTOPRIMARY returns the primary monitor
	m_main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(
		::MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY));

	// Create and initialize the m_window class structure
	// WNDCLASSEX contains m_window class attributes like style and icon
	WNDCLASSEX wc = {};
	m_wc		  = &wc;

	// Size of the structure (required for Windows versioning)
	m_wc->cbSize = sizeof(WNDCLASSEX);

	// Window class style flags
	// CS_HREDRAW: Redraw entire m_window if width changes
	// CS_VREDRAW: Redraw entire m_window if height changes
	m_wc->style = CS_HREDRAW | CS_VREDRAW;

	// Pointer to the m_window procedure function that handles messages
	m_wc->lpfnWndProc = app::App::WndProc;

	// Application instance handle
	m_wc->hInstance = hInstance;

	// Cursor to display when mouse is over m_window (standard arrow)
	m_wc->hCursor = LoadCursor(NULL, IDC_ARROW);

	// Class name for this m_window type (used to create windows of this class)
	m_wc->lpszClassName = L"ImGui Example";

	// Register the m_window class with Windows
	// Must be done before creating windows of this class
	::RegisterClassEx(m_wc);


	// Check if ANY m_window configuration arguments were passed
	// If no arguments, we'll use monitor resolution and maximize
	m_bHasAnyWindowArgs = HasAnyWindowArguments(m_cmdArgs);

	// Get m_window dimensions using WindowManager methods
	// These methods check command line args and use monitor resolution as default
	m_windowWidth = GetWindowWidth(m_cmdArgs);
	m_windowHeigh = GetWindowHeight(m_cmdArgs);

	// Get m_window position from command line or use Windows default
	// CW_USEDEFAULT lets Windows choose the best position
	m_windowX_pos = GetWindowX(m_cmdArgs);
	m_windowY_pos = GetWindowY(m_cmdArgs);

	// Determine m_window style based on command line arguments
	m_windowStyle = WS_OVERLAPPEDWINDOW; // Default: standard m_window with borders

	// Check if fullscreen mode was requested
	if (ShouldStartFullscreen(m_cmdArgs)) {
		// WS_POPUP creates a borderless m_window (true fullscreen)
		m_windowStyle = WS_POPUP;

		// For fullscreen, use entire screen dimensions
		m_windowWidth = GetMonitorWidth();
		m_windowHeigh = GetMonitorHeight();
		m_windowX_pos = 0;
		m_windowY_pos = 0;
	}
	// Check if explicitly requesting windowed mode
	else if (ShouldStartWindowed(m_cmdArgs)) {
		// Keep default WS_OVERLAPPEDWINDOW style
		m_windowStyle = WS_OVERLAPPEDWINDOW;
	}

	// Apply DPI scaling to m_window dimensions
	// This ensures the m_window is the correct physical size on high-DPI displays
	m_scaledWidth  = static_cast<int>(m_windowWidth * m_main_scale);
	m_scaledHeight = static_cast<int>(m_windowHeigh * m_main_scale);

	// Define the m_window rectangle
	RECT windowRect = {0, 0, static_cast<LONG>(m_scaledWidth), static_cast<LONG>(m_scaledHeight)};
	m_windowRect	= &windowRect;

	// Adjust m_window rectangle to account for borders (if not fullscreen)
	if (m_windowStyle != WS_POPUP) {
		// This increases the dimensions to include title bar, borders, etc.
		AdjustWindowRect(&windowRect, m_windowStyle, FALSE);

		// Update dimensions to include non-client areas
		m_scaledWidth  = windowRect.right - windowRect.left;
		m_scaledHeight = windowRect.bottom - windowRect.top;
	}

	// Lambda function for creating windowed mode
	// This encapsulates the CreateWindowW call for windowed style
	auto windowed = [&]() {
		m_hwnd = ::CreateWindowW(m_wc->lpszClassName,			  // Window class name
								 L"Dear ImGui DirectX12 Example", // Window title
								 WS_OVERLAPPEDWINDOW,			  // Window style (with borders)
								 m_windowX_pos,					  // X position
								 m_windowY_pos,					  // Y position
								 m_scaledWidth,					  // Width (scaled for DPI)
								 m_scaledHeight,				  // Height (scaled for DPI)
								 nullptr,						  // Parent m_window (none)
								 nullptr,						  // Menu (none)
								 m_wc->hInstance,				  // Application instance
								 nullptr);						  // Additional data (none)
	};

	// Lambda function for creating fullscreen mode
	// This encapsulates the CreateWindowW call for fullscreen style
	auto fullscreen = [&]() {
		m_hwnd = ::CreateWindowW(m_wc->lpszClassName,			  // Window class name
								 L"Dear ImGui DirectX12 Example", // Window title
								 WS_POPUP,						  // Window style (no borders)
								 0,								  // X position (top-left)
								 0,								  // Y position (top-left)
								 GetMonitorWidth(),				  // Width (full monitor)
								 GetMonitorHeight(),			  // Height (full monitor)
								 nullptr,						  // Parent m_window (none)
								 nullptr,						  // Menu (none)
								 m_wc->hInstance,				  // Application instance
								 nullptr);						  // Additional data (none)
	};

	// Create the m_window based on the determined style
	if (m_windowStyle == WS_POPUP) {
		// Use fullscreen lambda for borderless fullscreen
		fullscreen();
	} else {
		// Use windowed lambda for standard m_window with borders
		windowed();
	}

	// Check if m_window creation failed
	if (!m_hwnd) {
		// Output error and return failure
		std::cout << ("Error: Failed to create m_window\n");
		return false;
	}

	// Apply dark mode to title bar (if not fullscreen)
	if (m_windowStyle != WS_POPUP) {
		if (!ApplyDarkModeToTitleBar(m_hwnd, true)) {
			std::cout << ("Warning: Failed to apply dark mode to title bar\n");
			std::cout << ("Window will use default light theme\n");
		} else {
			std::cout << ("Dark mode successfully applied to title bar\n");
		}
	}

	// Determine how to show the m_window
	m_showCommand = SW_SHOWDEFAULT; // Default: show in default state

	// If NO arguments were passed, automatically start maximized
	// This provides a better default experience using full monitor resolution
	if (!m_bHasAnyWindowArgs) {
		m_showCommand = SW_SHOWMAXIMIZED;
		std::cout
			<< ("No m_window arguments provided - starting maximized with monitor resolution\n");
	}
	// If -maximized was specified explicitly, start maximized
	else if (ShouldStartMaximized(m_cmdArgs) && m_windowStyle != WS_POPUP) {
		m_showCommand = SW_SHOWMAXIMIZED;
	}
	// If -fullscreen, just show normally (already sized correctly)
	else if (ShouldStartFullscreen(m_cmdArgs)) {
		m_showCommand = SW_SHOW;
	}

	// Show the m_window with the determined show command
	::ShowWindow(m_hwnd, m_showCommand);

	// Force m_window update to ensure it's drawn
	::UpdateWindow(m_hwnd);

	// Return success
	return true;
}


// ===== Window Configuration Helper Methods =====

/**
 * Gets the requested m_window width from command line arguments
 * Uses monitor width as default if not specified
 * 
 * @param m_cmdArgs Pointer to CommandLineArguments object
 * @return Window width in pixels
 */
int WindowManager::GetWindowWidth(CommandLineArguments* m_cmdArgs) {
	// If no m_cmdArgs provided, use monitor width
	if (!m_cmdArgs) { return GetMonitorWidth(); }

	// Get monitor width as default
	int defaultWidth = GetMonitorWidth();

	// Try -width first, then -w, then default
	int width = m_cmdArgs->GetArgumentValueInt(L"-width", -1);
	if (width > 0) return width;

	width = m_cmdArgs->GetArgumentValueInt(L"-w", -1);
	if (width > 0) return width;

	// No valid width specified, return monitor width
	return defaultWidth;
}

/**
 * Gets the requested m_window height from command line arguments
 * Uses monitor height as default if not specified
 * 
 * @param m_cmdArgs Pointer to CommandLineArguments object
 * @return Window height in pixels
 */
int WindowManager::GetWindowHeight(CommandLineArguments* m_cmdArgs) {
	// If no m_cmdArgs provided, use monitor height
	if (!m_cmdArgs) { return GetMonitorHeight(); }

	// Get monitor height as default
	int defaultHeight = GetMonitorHeight();

	// Try -height first, then -h, then default
	int height = m_cmdArgs->GetArgumentValueInt(L"-height", -1);
	if (height > 0) return height;

	height = m_cmdArgs->GetArgumentValueInt(L"-h", -1);
	if (height > 0) return height;

	// No valid height specified, return monitor height
	return defaultHeight;
}

/**
 * Gets the requested m_window X position from command line
 * 
 * @param m_cmdArgs Pointer to CommandLineArguments object
 * @return Window X position in pixels, or CW_USEDEFAULT
 */
int WindowManager::GetWindowX(CommandLineArguments* m_cmdArgs) {
	// If no m_cmdArgs provided, use default
	if (!m_cmdArgs) { return CW_USEDEFAULT; }

	// Get X position, use CW_USEDEFAULT if not specified
	return m_cmdArgs->GetArgumentValueInt(L"-x", CW_USEDEFAULT);
}

/**
 * Gets the requested m_window Y position from command line
 * 
 * @param m_cmdArgs Pointer to CommandLineArguments object
 * @return Window Y position in pixels, or CW_USEDEFAULT
 */
int WindowManager::GetWindowY(CommandLineArguments* m_cmdArgs) {
	// If no m_cmdArgs provided, use default
	if (!m_cmdArgs) { return CW_USEDEFAULT; }

	// Get Y position, use CW_USEDEFAULT if not specified
	return m_cmdArgs->GetArgumentValueInt(L"-y", CW_USEDEFAULT);
}

/**
 * Checks if m_window should start maximized
 * 
 * @param m_cmdArgs Pointer to CommandLineArguments object
 * @return true if should start maximized, false otherwise
 */
bool WindowManager::ShouldStartMaximized(CommandLineArguments* m_cmdArgs) {
	// If no m_cmdArgs provided, return false
	if (!m_cmdArgs) { return false; }

	// Check if -maximized or -maximize argument exists
	return m_cmdArgs->HasArgument(L"-maximized") || m_cmdArgs->HasArgument(L"-maximize");
}

/**
 * Checks if m_window should start in fullscreen mode
 * 
 * @param m_cmdArgs Pointer to CommandLineArguments object
 * @return true if should start fullscreen, false otherwise
 */
bool WindowManager::ShouldStartFullscreen(CommandLineArguments* m_cmdArgs) {
	// If no m_cmdArgs provided, return false
	if (!m_cmdArgs) { return false; }

	// Check if -fullscreen or -fs argument exists
	return m_cmdArgs->HasArgument(L"-fullscreen") || m_cmdArgs->HasArgument(L"-fs");
}

/**
 * Checks if m_window should start in windowed mode
 * 
 * @param m_cmdArgs Pointer to CommandLineArguments object
 * @return true if should start windowed, false otherwise
 */
bool WindowManager::ShouldStartWindowed(CommandLineArguments* m_cmdArgs) {
	// If no m_cmdArgs provided, return true (default is windowed)
	if (!m_cmdArgs) { return true; }

	// Check if -windowed or -m_window argument exists
	return m_cmdArgs->HasArgument(L"-windowed") || m_cmdArgs->HasArgument(L"-m_window");
}

/**
 * Checks if any m_window configuration arguments were passed
 * 
 * This determines if we should use default behavior (maximized)
 * or custom configuration from command line
 * 
 * @param m_cmdArgs Pointer to CommandLineArguments object
 * @return true if any m_window arguments exist, false otherwise
 */
bool WindowManager::HasAnyWindowArguments(CommandLineArguments* m_cmdArgs) {
	// If no m_cmdArgs provided, no arguments exist
	if (!m_cmdArgs) { return false; }

	// Check for any m_window-related arguments
	return m_cmdArgs->HasArgument(L"-width") || m_cmdArgs->HasArgument(L"-w") ||
		   m_cmdArgs->HasArgument(L"-height") || m_cmdArgs->HasArgument(L"-h") ||
		   m_cmdArgs->HasArgument(L"-x") || m_cmdArgs->HasArgument(L"-y") ||
		   m_cmdArgs->HasArgument(L"-maximized") || m_cmdArgs->HasArgument(L"-maximize") ||
		   m_cmdArgs->HasArgument(L"-fullscreen") || m_cmdArgs->HasArgument(L"-fs") ||
		   m_cmdArgs->HasArgument(L"-windowed") || m_cmdArgs->HasArgument(L"-m_window");
}

/**
 * Gets the primary monitor width dynamically
 * 
 * Uses GetSystemMetrics to query the current monitor resolution
 * 
 * @return Width of primary monitor in pixels
 */
int WindowManager::GetMonitorWidth() {
	// SM_CXSCREEN returns the width of the primary display monitor in pixels
	return GetSystemMetrics(SM_CXSCREEN);
}

/**
 * Gets the primary monitor height dynamically
 * 
 * Uses GetSystemMetrics to query the current monitor resolution
 * 
 * @return Height of primary monitor in pixels
 */
int WindowManager::GetMonitorHeight() {
	// SM_CYSCREEN returns the height of the primary display monitor in pixels
	return GetSystemMetrics(SM_CYSCREEN);
}

bool WindowManager::ApplyDarkModeToTitleBar(HWND hwnd, bool enabled) {
	// Check if the m_window handle is valid
	// nullptr indicates an invalid or uninitialized m_window
	if (hwnd == nullptr) {
		// Invalid handle, return failure immediately
		// Cannot apply theme to a non-existent m_window
		return false;
	}

	// Convert C++ boolean to Windows BOOL type
	// BOOL is a 32-bit integer type defined by Windows API
	// TRUE (1) = dark mode enabled
	// FALSE (0) = dark mode disabled
	// Note: Windows BOOL is different from C++ bool (4 bytes vs 1 byte)
	BOOL useDarkMode = enabled ? TRUE : FALSE;

	// Call the DwmSetWindowAttribute API to set the attribute
	// This function modifies Desktop Window Manager (DWM) properties
	// for the specified m_window
	//
	// Parameters breakdown:
	//   1. hwnd: Window handle to modify
	//   2. DWMWA_USE_IMMERSIVE_DARK_MODE: The attribute identifier (value: 20)
	//      This tells DWM which property we want to change
	//   3. &useDarkMode: Pointer to the value we want to set (TRUE or FALSE)
	//      Must be a pointer because the API needs to read from m_memory
	//   4. sizeof(useDarkMode): Size of the value in bytes (4 bytes for BOOL)
	//      Tells the API how many bytes to read from the pointer
	//
	// Return value:
	//   S_OK (0): Success - the attribute was set
	//   Error HRESULT: Failure - may indicate unsupported Windows version
	//                  or other DWM error
	HRESULT hr =
		DwmSetWindowAttribute(hwnd,							 // Window handle to modify
							  DWMWA_USE_IMMERSIVE_DARK_MODE, // Attribute: use immersive dark mode
							  &useDarkMode,					 // Value pointer: TRUE or FALSE
							  sizeof(useDarkMode)			 // Value size: 4 bytes (sizeof BOOL)
		);

	// SUCCEEDED is a Windows macro that checks if HRESULT indicates success
	// Definition: #define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
	// Returns true if hr >= 0, false otherwise
	// S_OK = 0x00000000 (success)
	// E_FAIL = 0x80004005 (generic failure)
	// E_INVALIDARG = 0x80070057 (invalid argument)
	return SUCCEEDED(hr);
}
}
