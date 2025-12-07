#include "PCH.hpp"
#include "WindowManager.hpp"

#include "DarkMode.hpp"
#include "Main.hpp"

WindowManager::WindowManager()
    : m_wc(nullptr), m_windowRect(nullptr), m_hwnd(nullptr), m_main_scale(0.f)
{
}

WindowManager::~WindowManager()
{
}

void WindowManager::WMCreateWindow(_In_ HINSTANCE &hInstance)
{

    // Make process DPI aware and obtain main monitor scale
    ImGui_ImplWin32_EnableDpiAwareness();
    m_main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(
        ::MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY));
    WNDCLASSEX wc = {};
    m_wc = &wc;
    m_wc->cbSize = sizeof(WNDCLASSEX);
    m_wc->style = CS_HREDRAW | CS_VREDRAW;
    m_wc->lpfnWndProc = WndProc;
    m_wc->hInstance = hInstance;
    m_wc->hCursor = LoadCursor(NULL, IDC_ARROW);
    m_wc->lpszClassName = L"ImGui Example";
    ::RegisterClassEx(m_wc);

    RECT windowRect = {0,
                       0,
                       static_cast<LONG>(2000),
                       static_cast<LONG>(1000)};
    m_windowRect = &windowRect;
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    m_hwnd = ::CreateWindowW(m_wc->lpszClassName,
                             L"Dear ImGui DirectX12 Example",
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT,
                             CW_USEDEFAULT,
                             static_cast<int>(2000 * m_main_scale),
                             static_cast<int>(1000 * m_main_scale),
                             nullptr,
                             nullptr,
                             m_wc->hInstance,
                             nullptr);

    if (!ApplyDarkModeToTitleBar(m_hwnd, true))
    {

        OutputDebugStringA("Warning: Failed to apply dark mode to title bar\n");
        OutputDebugStringA("Window will use default light theme\n");
    }
    else
    {
        OutputDebugStringA("Dark mode successfully applied to title bar\n");
    }
}
