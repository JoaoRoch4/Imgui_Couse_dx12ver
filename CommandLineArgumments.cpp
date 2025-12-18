#include "PCH.hpp"
#include "Classes.hpp"

CommandLineArguments::CommandLineArguments() : Args{}, szArgList(nullptr), argCount(0), bConsoleLaunched(false) {
}

CommandLineArguments::~CommandLineArguments() {

	LocalFree(szArgList);
}

void CommandLineArguments::Open() {
    GetInitArgs();
    if(HasArgument(L"-cmd")) m_bShowCmd = true;
    if(m_bShowCmd && (HasArgument(L"-help"))) m_bShowHelp = true;
    if(m_bShowCmd && (HasArgument(L"-Args"))) m_bShowArgs = true;

    if(m_bShowCmd) ShowCmd();
}

void CommandLineArguments::Tick() {}

void CommandLineArguments::Close() {

    this->~CommandLineArguments();
}

/**
 * Checks if a specific argument exists in the command line
 * 
 * Example usage:
 *   if (cmdArgs->HasArgument(L"-maximized")) { ... }
 * 
 * @param arg The argument to check (automatically converted to lowercase)
 * @return true if the argument was found, false otherwise
 */
bool CommandLineArguments::HasArgument(const std::wstring& arg) {

  // Convert argument to lowercase for case-insensitive comparison
    std::wstring lowerArg = toLower(arg);
    
    // Check if the argument exists in the Args map
    // find() returns an iterator; if it equals end(), the argument wasn't found
    return Args.find(lowerArg) != Args.end();

}
/**
 * Gets the value following a command line argument
 * 
 * Example usage:
 *   std::wstring title = cmdArgs->GetArgumentValue(L"-title", L"Default Title");
 *   // If command line is: program.exe -title "My Window"
 *   // Returns: "My Window"
 * 
 * @param arg The argument name to search for
 * @param defaultValue Value to return if argument not found or has no value
 * @return The value following the argument, or defaultValue
 */
std::wstring CommandLineArguments::GetArgumentValue(const std::wstring& arg, const std::wstring& defaultValue) {

   // Convert argument to lowercase
    std::wstring lowerArg = toLower(arg);
    
    // Try to find the argument in the map
    auto it = Args.find(lowerArg);
    
    // If argument not found, return default value
    if (it == Args.end()) {
        return defaultValue;
    }
    
    // Get the index of this argument
    uint64_t index = it->second;
    
    // Check if there's a next argument (the value we want)
    // index + 1 must be less than argCount to be valid
    if (index + 1 < static_cast<uint64_t>(argCount)) {
        // Return the next argument as the value
        return szArgList[index + 1];
    }
    
    // No value found after the argument, return default
    return defaultValue;
}

int CommandLineArguments::GetInitArgs() {

	szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);

	if (szArgList == NULL) {
		MessageBox(NULL, L"Unable to parse command line", L"Error", MB_OK);
		throw std::runtime_error("Unable to parse command line");
		return EXIT_FAILURE;
	}

	std::cout << "arguments passed: " << argCount;

	for (int i = 0; i < argCount; i++) {


		Args.insert(std::make_pair(toLower(szArgList[i]), i));
	}


	return 0;
}

/**
 * Gets an integer value from a command line argument
 * 
 * Example usage:
 *   int width = cmdArgs->GetArgumentValueInt(L"-width", 1280);
 *   // If command line is: program.exe -width 1920
 *   // Returns: 1920
 * 
 * @param arg The argument name to search for
 * @param defaultValue Value to return if argument not found or invalid
 * @return The integer value, or defaultValue
 */
int CommandLineArguments::GetArgumentValueInt(const std::wstring& arg, int defaultValue) {
    // Get the string value first
    std::wstring value = GetArgumentValue(arg, L"");
    
    // If no value found, return default
    if (value.empty()) {
        return defaultValue;
    }
    
    // Try to convert string to integer
    // wcstol converts wide string to long integer
    // nullptr = don't need end pointer, 10 = base 10 (decimal)
    try {
        // Use std::stoi for safer conversion with exception handling
        return std::stoi(value);
    }
    catch (const std::exception&) {
        // If conversion fails, return default value
        return defaultValue;
    }
}

/**
 * Prints all available command line options to console
 * This is useful for showing help to users
 * 
 * Command line usage: program.exe -help or program.exe -?
 */
void CommandLineArguments::PrintHelp() {
    // Print header
    std::wcout << L"\n=== Command Line Arguments ===" << std::endl;
    std::wcout << L"\nWindow Configuration:" << std::endl;
    
    // Print window size options
    std::wcout << L"  -width <pixels>  or -w <pixels>  : Set window width" << std::endl;
    std::wcout << L"  -height <pixels> or -h <pixels>  : Set window height" << std::endl;
    
    // Print window position options
    std::wcout << L"  -x <pixels>                       : Set window X position" << std::endl;
    std::wcout << L"  -y <pixels>                       : Set window Y position" << std::endl;
    
    // Print window state options
    std::wcout << L"  -maximized or -maximize           : Start maximized" << std::endl;
    std::wcout << L"  -fullscreen or -fs                : Start fullscreen (no borders)" << std::endl;
    std::wcout << L"  -windowed or -window              : Start in windowed mode" << std::endl;
    
    // Print other options
    std::wcout << L"\nOther Options:" << std::endl;
    std::wcout << L"  -cmd                              : Show console window" << std::endl;
    std::wcout << L"  -help                       : Show this help message" << std::endl;
    
    // Print examples
    std::wcout << L"\nExamples:" << std::endl;
    std::wcout << L"  program.exe -maximized" << std::endl;
    std::wcout << L"  program.exe -width 1920 -height 1080" << std::endl;
    std::wcout << L"  program.exe -fullscreen" << std::endl;
    std::wcout << L"  program.exe -x 100 -y 100 -width 800 -height 600" << std::endl;
    
    std::wcout << L"\n==============================\n" << std::endl;
}

void CommandLineArguments::ShowCmd() {

        ShowConsole();

		std::cout << "\nThis message appears in the new console window.\n" << std::endl;

   
        if(m_bShowHelp) PrintHelp();

		std::wcout << std::endl;
	

}


std::wstring CommandLineArguments::toLower(std::wstring s) {
	std::transform(s.begin(), s.end(), s.begin(), [](wchar_t c) { return std::tolower(c); });
	return s;
}

void CommandLineArguments::ShowConsole()  {

	if (!AllocConsole()) return;

    bConsoleLaunched = true;

	HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE hConsoleInput  = GetStdHandle(STD_INPUT_HANDLE);

	// Redirect standard I/O streams to the new console
	// C-style I/O
	FILE *fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stderr);

	// C++-style I/O
	std::ios_base::sync_with_stdio();

    
	std::setlocale(LC_ALL, "");
    SetConsoleOutputCP(CP_UTF8);

	// Clear iostream error flags
	std::cout.clear();
	std::cerr.clear();
	std::cin.clear();
}
