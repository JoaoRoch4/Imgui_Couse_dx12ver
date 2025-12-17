#pragma once
#include "PCH.hpp"


static UINT	  g_frameIndex = 0;
static HANDLE m_fenceEvent;
static UINT64 m_fenceLastSignaledValue	 = 0;
static bool	  m_SwapChainTearingSupport	 = false;
static bool	  m_SwapChainOccluded		 = false;
static HANDLE m_hSwapChainWaitableObject = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void WaitForPendingOperations();
void ShowExampleAppMainMenuBar();
void ShowExampleMenuFile();
void ShowConsole(class CommandLineArguments* cmdArgs);
void Cleanup(class WindowManager* window);
void OpenWindow(_In_ HINSTANCE hInstance, CommandLineArguments* cmdArgs, WindowManager* window);
void MainLoop(struct ImGuiIO* io, WindowManager* window);
int	 Start(_In_ HINSTANCE hInstance, CommandLineArguments* cmdArgs);

class FrameContext* WaitForNextFrameContext();

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
