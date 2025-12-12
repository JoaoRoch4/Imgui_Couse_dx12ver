#pragma once

#include "PCH.hpp"

class DebugWindow {
public:
    DebugWindow(ImGuiIO* io);

    virtual ~DebugWindow();

    void Render();

private:

    DebugWindow();


    ImGuiIO* m_io;

};

