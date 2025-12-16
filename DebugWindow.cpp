#include "PCH.hpp"

#include "DebugWindow.hpp"


DebugWindow::DebugWindow() : m_io(nullptr), tPsTHread(), bPsOpen(false), hProcessHandle(nullptr) {}

DebugWindow::DebugWindow(ImGuiIO* io) : DebugWindow() {

	if (io) m_io = io;
	else throw std::runtime_error("io is nullptr");
}

DebugWindow::~DebugWindow() {

	if (tPsTHread.joinable()) {
		tPsTHread
			.detach(); // Permite que a thread termine sozinha após o fechamento do app principal
	}
}

void DebugWindow::Render() {

	ImGui::Begin("Debug Window!"); // Create a window called "Hello, world!"
	// and append into it.

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / m_io->Framerate,
				m_io->Framerate);

	if (ImGui::Button("Debug Break")) __debugbreak();


    if (bPsOpen) {
        ig::SameLine();
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "PowerShell open");
        
        // Botão para forçar o encerramento (Kill Signal)
        if (ImGui::Button("Close PowerShell")) {
            TerminateProcess(hProcessHandle, 0);
        }
    } else {
        	if (ImGui::Button("Open PowerShell")) { openPowershellTherad(); }

    }



	ImGui::End();
}

void DebugWindow::openPowershell() {


	STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };

    TCHAR szCmdLine[] = TEXT("pwsh.exe");

    if (CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, 
                      CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) 
    {
        this->hProcessHandle = pi.hProcess; // Salva o handle para consulta
        this->bPsOpen = true;

        // Aguarda o sinal de encerramento do Windows
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Limpeza
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        this->hProcessHandle = NULL; 
        this->bPsOpen = false;
    }
}

void DebugWindow::openPowershellTherad() {

	// 1. Verifica se a thread já existe e está rodando
	if (tPsTHread.joinable()) {
		// Opcional: Você pode dar join() se quiser esperar a anterior terminar
		// ou simplesmente retornar se não quiser abrir duas janelas ao mesmo tempo.
		tPsTHread.join();
	}

	// 2. Cria a thread passando:
	// &DebugWindow::openPowershell -> O endereço do método
	// this -> A instância atual da classe para a qual o método será chamado
	if (!tPsTHread.joinable() && !bPsOpen) {
		tPsTHread = std::thread(&DebugWindow::openPowershell, this);
		bPsOpen	  = true;
	}
}

bool DebugWindow::IsProcessRunning() {

    if (hProcessHandle == NULL) return false;

    DWORD exitCode = 0;
    if (GetExitCodeProcess(hProcessHandle, &exitCode)) {
        return (exitCode == STILL_ACTIVE);
    }
    return false;
}
