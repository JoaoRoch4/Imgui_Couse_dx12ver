// WindowManager.hpp
// Header file for WindowManager class
// This class manages window creation and configuration

#pragma once

#include "PCH.hpp"

// Forward declaration to avoid circular dependency
class CommandLineArguments;

/**
 * @brief Class that manages the application window
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


	bool WMCreateWindow(_In_ HINSTANCE hInstance, CommandLineArguments* cmdArgs);


	bool WMCreateWindowWithArgs(_In_ HINSTANCE hInstance, CommandLineArguments* cmdArgs);


	inline WNDCLASSEX* GetWc() const { return m_wc; }


	inline RECT* GetWindowRect() const { return m_windowRect; }


	inline HWND GetHWND() const { return m_hwnd; }


	inline float get_main_scale() const { return m_main_scale; }


	int GetWindowWidth(CommandLineArguments* cmdArgs);


	int GetWindowHeight(CommandLineArguments* cmdArgs);


	int GetWindowX(CommandLineArguments* cmdArgs);


	int GetWindowY(CommandLineArguments* cmdArgs);

	bool ShouldStartMaximized(CommandLineArguments* cmdArgs);


	bool ShouldStartFullscreen(CommandLineArguments* cmdArgs);


	bool ShouldStartWindowed(CommandLineArguments* cmdArgs);


	bool HasAnyWindowArguments(CommandLineArguments* cmdArgs);


	int GetMonitorWidth();


	int GetMonitorHeight();

private:
	bool ApplyDarkModeToTitleBar(HWND hwnd, bool enabled);

	// Member variables
	WNDCLASSEX* m_wc; // Pointer to window class structure

	RECT* m_windowRect; // Pointer to window rectangle
	HWND  m_hwnd;		// Handle to the created window

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
