// WindowManager.hpp
// Header file for WindowManager class
// This class manages m_window creation and configuration

#pragma once

#include "PCH.hpp"

namespace app {

// Forward declaration to avoid circular dependency
class CommandLineArguments;

/**
 * @brief Class that manages the application m_window
 * 
 * This class handles:
 * - Window creation and registration
 * - DPI awareness and scaling
 * - Window configuration from command line arguments
 * - Dark mode application
 */
class WindowManager {
public:
	WindowManager();

	~WindowManager();


	bool WMCreateWindow(_In_ HINSTANCE hInstance, CommandLineArguments* m_cmdArgs);


	bool WMCreateWindowWithArgs(_In_ HINSTANCE hInstance, CommandLineArguments* m_cmdArgs);


	inline WNDCLASSEX* GetWc() { return &m_wc; }


	inline RECT* GetWindowRect() const { return m_windowRect; }


	inline HWND GetHWND() const { return m_hwnd; }


	inline float get_main_scale() const { return m_main_scale; }


	int GetWindowWidth(CommandLineArguments* m_cmdArgs);


	int GetWindowHeight(CommandLineArguments* m_cmdArgs);


	int GetWindowX(CommandLineArguments* m_cmdArgs);


	int GetWindowY(CommandLineArguments* m_cmdArgs);

	bool ShouldStartMaximized(CommandLineArguments* m_cmdArgs);


	bool ShouldStartFullscreen(CommandLineArguments* m_cmdArgs);


	bool ShouldStartWindowed(CommandLineArguments* m_cmdArgs);


	bool HasAnyWindowArguments(CommandLineArguments* m_cmdArgs);


	int GetMonitorWidth();


	int GetMonitorHeight();

private:
bool ApplyDarkModeToTitleBar(HWND hwnd, bool enabled);

// Member variables
WNDCLASSEX m_wc; // Window class structure

	RECT* m_windowRect; // Pointer to m_window rectangle
	HWND  m_hwnd;		// Handle to the created m_window

	float m_main_scale; // DPI scale factor for the main monitor

	int m_windowWidth;
	int m_windowHeigh;
	int m_windowX_pos;
	int m_windowY_pos;
	int m_windowStyle;
	int m_scaledWidth;
	int m_scaledHeight;
	int m_showCommand;

	bool m_bHasAnyWindowArgs;
};

} // namespace app
