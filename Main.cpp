#include "pch.hpp"
#include "CommandLineArgumments.hpp"
#include "Source.hpp"
#include "Classes.hpp"

int ErrorMsg(std::runtime_error& e) {

    auto what = e.what();
   auto LVl =  ConvStatic::strtoWstr(what);
	
	MessageBox(NULL, LVl.c_str(), L"Error", MB_ICONERROR | MB_OK);
	return EXIT_FAILURE;
}

_Use_decl_annotations_ int WINAPI wWinMain(_In_ HINSTANCE	  hInstance,
										   _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
										   _In_ int nCmdShow) {

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	try {
		Start(hInstance);


	} catch (std::runtime_error& e) {
        return ErrorMsg(e);
    }

	return 0;
}



