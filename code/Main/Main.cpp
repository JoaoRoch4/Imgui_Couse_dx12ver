#include "pch.hpp"
#include "CommandLineArgumments.hpp"
#include "Source.hpp"
#include "Classes.hpp"
#include "Main.hpp"


_Use_decl_annotations_ int WINAPI wWinMain(_In_ HINSTANCE	  hInstance,
										   _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
										   _In_ int nCmdShow) {

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	try {
		Start(hInstance);


	} catch (std::exception& e) { return ErrorMsg(e); } catch (...) {
		return UnkExcpt();
	}

	return 0;
}
