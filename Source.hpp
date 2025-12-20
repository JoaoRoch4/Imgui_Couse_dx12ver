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
void Cleanup();
void OpenWindow(_In_ HINSTANCE hInstance);
void MainLoop(struct ImGuiIO* m_io);
int	 Start(_In_ HINSTANCE hInstance);

class FrameContext* WaitForNextFrameContext();

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
