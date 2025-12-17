#include "pch.hpp"
#include "CommandLineArgumments.hpp"
#include "Source.hpp"
#include "Classes.hpp"


_Use_decl_annotations_ int WINAPI wWinMain(_In_ HINSTANCE	  hInstance,
										   _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
										   _In_ int nCmdShow) {

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	//auto cmdArgs = std::make_unique<CommandLineArguments>();

	try {
		 Start(hInstance);
        


	} catch (std::runtime_error &e) {

        const char* err = e.what();

        throw std::runtime_error(err);
		return EXIT_FAILURE;
	}

    return 0;
}
