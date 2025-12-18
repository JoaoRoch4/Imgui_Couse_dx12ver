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

	std::setlocale(LC_ALL, "");
	try {
		Start(hInstance);


	} catch (std::runtime_error& e) {
        return ErrorMsg(e);
    }

	return 0;
}

int ErrorMsg(std::runtime_error& e) {

	const char*	 err	  = e.what();
	size_t		 charSize = std::strlen(err) + 1;
	std::wstring wcharErr(charSize, L'#');
	size_t		 convertedChars = std::mbstowcs(&wcharErr[0], err, charSize);
	if (convertedChars != static_cast<size_t>(-1)) {
		wcharErr.resize(convertedChars);
	} else {
		MessageBox(NULL, L"Conversion error!", L"Error", MB_ICONERROR | MB_OK);
		return 1;
	}
	MessageBox(NULL, wcharErr.c_str(), L"Error", MB_ICONERROR | MB_OK);
	return EXIT_FAILURE;
}

