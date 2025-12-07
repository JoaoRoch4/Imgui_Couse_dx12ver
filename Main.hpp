#pragma once
#include "pch.hpp"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

_Use_decl_annotations_ int WINAPI wWinMain(_In_ HINSTANCE hInstance,
                                           _In_opt_ HINSTANCE hPrevInstance,
                                           _In_ LPWSTR lpCmdLine,
                                           _In_ int nCmdShow);
