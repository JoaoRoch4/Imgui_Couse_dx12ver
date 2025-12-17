#pragma once

#include "PCH.hpp"
#include "Master.hpp"

class DebugWindow : public Master {
public:
    DebugWindow(ImGuiIO* io);

    virtual ~DebugWindow();

  

     virtual void Open()     override;
     virtual void Tick()     override;
     virtual void Close()    override;

private:
    void Render();
    DebugWindow();
    void openPowershell();
    void openPy();

    void openPowershellTherad();
    void openPyTherad();

    bool IsPsProcessRunning();

    ImGuiIO* m_io;

    std::thread tPsTHread;
    std::thread tPyTHread;
    std::atomic<bool> bPsOpen;
    std::atomic<bool> bPyOpen;
    HANDLE hPsProcessHandle; // Armazena o handle do PowerShell
    HANDLE hPyProcessHandle;

};

