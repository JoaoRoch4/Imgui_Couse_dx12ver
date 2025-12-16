#pragma once

#include "PCH.hpp"

class DebugWindow {
public:
    DebugWindow(ImGuiIO* io);

    virtual ~DebugWindow();

    void Render();

    

private:

    DebugWindow();
    void openPowershell();

    void openPowershellTherad();

    bool IsProcessRunning();

    ImGuiIO* m_io;

    std::thread tPsTHread;
    std::atomic<bool> bPsOpen;
    HANDLE hProcessHandle; // Armazena o handle do PowerShell

};

