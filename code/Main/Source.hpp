
#pragma once
#include "PCH.hpp"


static UINT	  g_frameIndex = 0;
static HANDLE m_fenceEvent;
static UINT64 m_fenceLastSignaledValue	 = 0;
static bool	  m_SwapChainTearingSupport	 = false;
static bool	  m_SwapChainOccluded		 = false;
static HANDLE m_hSwapChainWaitableObject = nullptr;



/**
 * @brief Main application entry point
 * @param hInstance Application instance handle from WinMain
 * @return EXIT_SUCCESS on successful execution, error code otherwise
 *
 * Initializes all subsystems, creates the DirectX device and m_window,
 * sets up ImGui, runs the main loop, and performs cleanup.
 */
int Start(_In_ HINSTANCE hInstance);

/**
 * @brief Creates and displays the application m_window
 * @param hInstance Application instance handle
 *
 * Creates the Win32 m_window, initializes DirectX 12 device and resources,
 * and displays the m_window in maximized state.
 */
void OpenWindow(_In_ HINSTANCE hInstance);

/**
 * @brief Main rendering loop
 * @param m_io Pointer to ImGui IO context
 *
 * Loads fonts, initializes configuration, and runs the main message loop.
 * Handles m_window messages, renders ImGui windows, and manages frame presentation.
 * Saves configuration on exit.
 */
void MainLoop(ImGuiIO* m_io);

/**
 * @brief Cleanup function called before application exit
 *
 * Shuts down ImGui, destroys DirectX resources, and unregisters m_window class.
 */
void Cleanup();

/**
 * @brief Creates DirectX 12 device and swap chain
 * @param hWnd Window handle to associate with swap chain
 * @return true if device creation successful, false otherwise
 *
 * Creates D3D12 device, command queue, command allocators, descriptor heaps,
 * swap chain, and fence objects. Configures swap chain scaling mode to prevent
 * m_window stretching issues.
 */
bool CreateDeviceD3D(HWND hWnd);

/**
 * @brief Releases all DirectX 12 resources
 *
 * Cleans up render targets, closes handles, and releases DirectX objects.
 */
void CleanupDeviceD3D();

/**
 * @brief Creates render target views for swap chain back buffers
 *
 * Gets back buffer resources from swap chain and creates render target views
 * in the RTV descriptor heap.
 */
void CreateRenderTarget();

/**
 * @brief Releases render target resources
 *
 * Waits for GPU to complete all operations then releases back buffer resources.
 */
void CleanupRenderTarget();

/**
 * @brief Blocks until GPU completes all pending operations
 *
 * Signals fence with incremented value and waits for GPU to reach that value.
 * Used during cleanup and resource recreation.
 */
void WaitForPendingOperations();

/**
 * @brief Window procedure callback for handling Windows messages
 * @param hWnd Window handle
 * @param msg Message identifier
 * @param wParam Additional message parameter
 * @param lParam Additional message parameter
 * @return Result of message processing
 *
 * Forwards messages to ImGui, handles m_window resize, and processes
 * system commands and destruction messages.
 */
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/**
 * @brief Displays ImGui main menu bar
 *
 * Creates File and Edit menus with standard menu items.
 */
void ShowExampleAppMainMenuBar();

/**
 * @brief Displays File menu contents
 *
 * Shows New, Open, Save, and other file-related menu items with submenus.
 */
void ShowExampleMenuFile();

/**
 * @brief Displays error message dialog for exceptions
 * @param e Exception object containing error information
 * @return EXIT_FAILURE
 *
 * Converts exception message to wide string and displays in MessageBox.
 */
int ErrorMsg(std::exception& e);

/**
 * @brief Displays error dialog for unknown exceptions
 * @return EXIT_FAILURE
 *
 * Shows generic "unknown exception" message in MessageBox.
 */
int UnkExcpt();



LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
