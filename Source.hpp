#pragma once
#include "PCH.hpp"
#include "Classes.hpp"

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void WaitForPendingOperations();
FrameContext *WaitForNextFrameContext();
void ShowConsole();

int Start(_In_ HINSTANCE hInstance);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
