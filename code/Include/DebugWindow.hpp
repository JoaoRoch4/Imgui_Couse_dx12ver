#pragma once

#include "PCH.hpp"
#include "MemoryManagement.hpp"

namespace app {

class DebugWindow : public MemoryManagement {
public:
	DebugWindow();

	void GetIo(ImGuiIO* io);

	virtual ~DebugWindow();


	virtual void Open() override;
	virtual void Tick() override;
	virtual void Close() override;

private:
	void Render();
	void openPowershell();
	void openPy();

	void openPowershellTherad();
	void openPyTherad();

	bool IsPsProcessRunning();

	ImGuiIO* m_io;

	std::thread		  tPsTHread;
	std::thread		  tPyTHread;
	std::atomic<bool> bPsOpen;
	std::atomic<bool> bPyOpen;
	HANDLE			  hPsProcessHandle; // Armazena o handle do PowerShell
	HANDLE			  hPyProcessHandle;

    MemoryManagement* m_memory;
};
} // namespace app
