// Main.cpp
// Application entry point for Windows GUI application
// Handles initialization and exception management

#include "pch.hpp"
#include "Classes.hpp"
#include "Helpers.hpp"

/**
 * @brief Windows application entry point
 *
 * This is the main entry point for a Windows GUI application (not m_console).
 * It initializes the application and handles top-level exception management.
 *
 * The function is marked with _Use_decl_annotations_ to enable SAL (Source Code
 * Annotation Language) for parameter validation and analysis.
 *
 * @param hInstance Handle to the current instance of the application
 * @param hPrevInstance Always NULL in Win32 (legacy parameter, unused)
 * @param lpCmdLine Command line arguments as a Unicode string (unused - parsed elsewhere)
 * @param nCmdShow Controls how the m_window is shown (unused - controlled internally)
 *
 * @return 0 on successful execution
 * @return Non-zero error code if an exception occurs
 */
_Use_decl_annotations_ int WINAPI wWinMain(_In_ HINSTANCE	  hInstance,
										   _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
										   _In_ int nCmdShow) {

	// Mark unused parameters to suppress compiler warnings
	// These parameters are required by the WinMain signature but not used
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

    int i = 0;

    app::MemoryManagement* memory = app::MemoryManagement::Get_MemoryManagement_Singleton();
    if(memory) memory->AllocAll();
    app::App* m_app = memory->Get_App();
    if(m_app) m_app->Run(hInstance);
    memory->Destroy_All();

	try {
		

		// Handle known exceptions (std::exception and derived classes)
	} catch (std::exception& e) {
		// Display error message to user and return error code
		return Helpers::ErrorMsg(e);

		// Handle unknown/unexpected exceptions
	} catch (...) {
		// Display generic error message and return error code
		return Helpers::UnkExcpt();
	}

	// Successful execution - normal termination
	return 0;
}
