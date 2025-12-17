#include "pch.hpp"
#include "CommandLineArgumments.hpp"
#include "Source.hpp"

_Use_decl_annotations_ int WINAPI wWinMain(_In_ HINSTANCE hInstance,
										   _In_opt_ HINSTANCE hPrevInstance,
										   _In_ LPWSTR lpCmdLine,
										   _In_ int nCmdShow) {

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

    auto cmdArgs = std::make_unique<CommandLineArguments>();

	try {
		return Start(hInstance, cmdArgs.get());
	} catch (std::runtime_error &e) {

        if(!cmdArgs->GetbConsoleLaunched()) {
            if(!AllocConsole());
        }
         std::cout << e.what(); 
		return EXIT_FAILURE;
	}
}
