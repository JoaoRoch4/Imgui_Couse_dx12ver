#pragma once

#include "PCH.hpp"

class WindowManager {

public:
  WindowManager();
  ~WindowManager();

  void WMCreateWindow(_In_ HINSTANCE &hInstance);

  HWND GetHWND() const { return m_hwnd; }
  WNDCLASSEX *GetWc() const { return m_wc; }
  float get_main_scale() const { return m_main_scale; }

private:
  WNDCLASSEX *m_wc;
  RECT *m_windowRect;
  HWND m_hwnd;
  float m_main_scale;
};

