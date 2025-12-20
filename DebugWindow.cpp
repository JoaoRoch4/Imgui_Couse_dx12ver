#include "PCH.hpp"

#include "DebugWindow.hpp"


DebugWindow::DebugWindow()
    : m_io(nullptr), tPsTHread{}, tPyTHread{}, bPsOpen(false),bPyOpen(false), hPsProcessHandle(nullptr),hPyProcessHandle(nullptr) {}

void DebugWindow::GetIo(ImGuiIO* io) {

    if(io) m_io = io;
    else throw std::runtime_error("io is nullptr");
}


DebugWindow::~DebugWindow() {

	if (tPsTHread.joinable()) { tPsTHread.detach(); }
    if (tPyTHread.joinable()) { tPyTHread.detach(); }
	bPsOpen			 = false;
	bPyOpen			 = false;
	hPsProcessHandle = nullptr;
	hPyProcessHandle = nullptr;
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
		if (ImGui::Button("Close PowerShell")) { TerminateProcess(hPsProcessHandle, 0); }
	} else {
		if (ImGui::Button("Open PowerShell")) { openPowershellTherad(); }
	}

	if (bPyOpen) {
		ig::SameLine();
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "Python open");

		// Botão para forçar o encerramento (Kill Signal)
		if (ImGui::Button("Close Python")) { TerminateProcess(hPyProcessHandle, 0); }
	} else {
		if (ImGui::Button("Open Python")) { openPyTherad(); }
	}

	ImGui::End();
}

void DebugWindow::Open() { Render(); }

void DebugWindow::Tick() {
	Render();
	if (tPsTHread.joinable()) { tPsTHread.detach(); }
	if (tPyTHread.joinable()) { tPyTHread.detach(); }
}

void DebugWindow::Close() { this->~DebugWindow(); }

void DebugWindow::openPowershell() {


	STARTUPINFO			si = {sizeof(si)};
	PROCESS_INFORMATION pi = {0};

	TCHAR szCmdLine[] = TEXT("pwsh.exe");

	if (CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si,
					  &pi)) {
		this->hPsProcessHandle = pi.hProcess; // Salva o handle para consulta
		this->bPsOpen		   = true;

		// Aguarda o sinal de encerramento do Windows
		WaitForSingleObject(pi.hProcess, INFINITE);

		// Limpeza
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		this->hPsProcessHandle = NULL;
		this->bPsOpen		   = false;
	}
}

void DebugWindow::openPy() {

	STARTUPINFO			si = {sizeof(si)};
	PROCESS_INFORMATION pi = {0};

	TCHAR szCmdLine[] = TEXT("python.exe");

	if (CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si,
					  &pi)) {
		this->hPyProcessHandle = pi.hProcess; // Salva o handle para consulta
		this->bPyOpen		   = true;

		// Aguarda o sinal de encerramento do Windows
		WaitForSingleObject(pi.hProcess, INFINITE);

		// Limpeza
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		this->hPsProcessHandle = NULL;
		this->bPyOpen		   = false;
	}
}

void DebugWindow::openPowershellTherad() {

	// 1. Verifica se a thread já existe e está rodando
	if (tPsTHread.joinable()) {
		// Opcional: Você pode dar join() se quiser esperar a anterior terminar
		// ou simplesmente retornar se não quiser abrir duas janelas ao mesmo tempo.
		tPsTHread.detach();
		bPsOpen = false;
	}

	// 2. Cria a thread passando:
	// &DebugWindow::openPowershell -> O endereço do método
	// this -> A instância atual da classe para a qual o método será chamado
	if (!tPsTHread.joinable() && !bPsOpen) {
		tPsTHread = std::thread(&DebugWindow::openPowershell, this);
		bPsOpen	  = true;
	}
}

void DebugWindow::openPyTherad() {

	// 1. Verifica se a thread já existe e está rodando
	if (tPyTHread.joinable()) {
		// Opcional: Você pode dar join() se quiser esperar a anterior terminar
		// ou simplesmente retornar se não quiser abrir duas janelas ao mesmo tempo.
		tPyTHread.detach();
	}

	// 2. Cria a thread passando:
	// &DebugWindow::openPowershell -> O endereço do método
	// this -> A instância atual da classe para a qual o método será chamado
	if (!tPyTHread.joinable() && !bPyOpen) {
		tPyTHread = std::thread(&DebugWindow::openPy, this);
		bPyOpen	  = true;
	}
}

bool DebugWindow::IsPsProcessRunning() {

	if (hPsProcessHandle == NULL) return false;

	DWORD exitCode = 0;
	if (GetExitCodeProcess(hPsProcessHandle, &exitCode)) { return (exitCode == STILL_ACTIVE); }
	return false;
}
