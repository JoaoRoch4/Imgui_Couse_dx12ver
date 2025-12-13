#include "pch.hpp"
#include "Classes.hpp"
#include "Source.hpp"
_Use_decl_annotations_ int WINAPI wWinMain(_In_ HINSTANCE hInstance,
										   _In_opt_ HINSTANCE hPrevInstance,
										   _In_ LPWSTR lpCmdLine,
										   _In_ int nCmdShow) {

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	try {
		return Start(hInstance);
	} catch (std::runtime_error &e) {

		ShowConsole();
		std::cout << e.what();
		return EXIT_FAILURE;
	}
}
