#include "PCH.hpp"
#include "Classes.hpp"

CommandLineArgumments::CommandLineArgumments() : szArgList(nullptr), argCount() {
}

CommandLineArgumments::~CommandLineArgumments() {

	LocalFree(szArgList);
}

int CommandLineArgumments::GetInitArgs() {

	szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);

	if (szArgList == NULL) {
		MessageBox(NULL, L"Unable to parse command line", L"Error", MB_OK);
		throw std::runtime_error("Unable to parse command line");
		return EXIT_FAILURE;
	}

	std::cout << "arguments passed: " << argCount;

	for (int i = 0; i < argCount; i++) {

		ToPrint + L"\n"+ L"[" + std::to_wstring((i)) + L"] " +=  szArgList[i];

		Args.insert(std::make_pair(toLower(szArgList[i]), i));
	}

	ToPrint += L"\n";

    return 0;
}

std::wstring CommandLineArgumments::toLower(std::wstring s) {
	std::transform(s.begin(), s.end(), s.begin(), [](wchar_t c) { return std::tolower(c); });
	return s;
}
