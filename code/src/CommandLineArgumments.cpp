#include "PCH.hpp"
#include "Classes.hpp"

namespace app {

CommandLineArguments::CommandLineArguments()
: Args{}, szArgList(nullptr), argCount(0), bConsoleLaunched(false), m_memory(nullptr){

    m_memory = MemoryManagement::Get_MemoryManagement_Singleton(); //m_cmd = m_memory->Get_OutputConsole();
}

CommandLineArguments::~CommandLineArguments() { LocalFree(szArgList); }

void CommandLineArguments::Open() {
	GetInitArgs();
	if (HasArgument(L"-cmd")) m_bShowCmd = true;
	if (m_bShowCmd && (HasArgument(L"-help"))) m_bShowHelp = true;
	if (m_bShowCmd && (HasArgument(L"-Args"))) m_bShowArgs = true;

    m_memory->m_bShowCmd = this->m_bShowCmd;
    m_memory->m_bShowHelp = this->m_bShowHelp;
    m_memory->m_bShowArgs = this->m_bShowArgs;

	//if (m_bShowCmd) ShowCmd();
}

void CommandLineArguments::Tick() {}

void CommandLineArguments::Close() { }

/**
 * Checks if a specific argument exists in the command line
 * 
 * Example usage:
 *   if (m_cmdArgs->HasArgument(L"-maximized")) { ... }
 * 
 * @param arg The argument to check (automatically converted to lowercase)
 * @return true if the argument was found, false otherwise
 */
bool CommandLineArguments::HasArgument(const std::wstring& arg) {

	// Convert argument to lowercase for case-insensitive comparison
	std::wstring lowerArg = Helpers::strtoLowerW(arg);

	// Check if the argument exists in the Args map
	// find() returns an iterator; if it equals end(), the argument wasn't found
	return Args.find(lowerArg) != Args.end();
}
/**
 * Gets the value following a command line argument
 * 
 * Example usage:
 *   std::wstring title = m_cmdArgs->GetArgumentValue(L"-title", L"Default Title");
 *   // If command line is: program.exe -title "My Window"
 *   // Returns: "My Window"
 * 
 * @param arg The argument name to search for
 * @param defaultValue Value to return if argument not found or has no value
 * @return The value following the argument, or defaultValue
 */
std::wstring CommandLineArguments::GetArgumentValue(const std::wstring& arg,
													const std::wstring& defaultValue) {

	// Convert argument to lowercase
	std::wstring lowerArg = Helpers::strtoLowerW(arg);

	// Try to find the argument in the map
	auto it = Args.find(lowerArg);

	// If argument not found, return default value
	if (it == Args.end()) { return defaultValue; }

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

	for (int i = 0; i < argCount; i++) { Args.insert(std::make_pair(Helpers::strtoLowerW(szArgList[i]), i)); }


	return 0;
}

/**
 * Gets an integer value from a command line argument
 * 
 * Example usage:
 *   int width = m_cmdArgs->GetArgumentValueInt(L"-width", 1280);
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
	if (value.empty()) { return defaultValue; }

	// Try to convert string to integer
	// wcstol converts wide string to long integer
	// nullptr = don't need end pointer, 10 = base 10 (decimal)
	try {
		// Use std::stoi for safer conversion with exception handling
		return std::stoi(value);
	} catch (const std::exception&) {
		// If conversion fails, return default value
		return defaultValue;
	}
}

/**
 * Prints all available command line options to m_console
 * This is useful for showing help to users
 * 
 * Command line usage: program.exe -help or program.exe -?
 */
void CommandLineArguments::PrintHelp() {

    if(!m_bShowCmd) return;


	OutputConsole* cmd = m_memory->Get_OutputConsole();
	// Print header
	cmd->Out << tc::bright_blue;
	cmd->Out << L"\n=== Command Line Arguments ===" << std::endl;
	cmd->Out << L"\nWindow Configuration:" << std::endl;

	// Print m_window size options
	cmd->Out << L"  -width <pixels>  or -w <pixels>  : Set m_window width" << std::endl;
	cmd->Out << L"  -height <pixels> or -h <pixels>  : Set m_window height" << std::endl;

	// Print m_window position options
	cmd->Out << L"  -x <pixels>                       : Set m_window X position" << std::endl;
	cmd->Out << L"  -y <pixels>                       : Set m_window Y position" << std::endl;

	// Print m_window state options
	cmd->Out << L"  -maximized or -maximize           : Start maximized" << std::endl;
	cmd->Out << L"  -fullscreen or -fs                : Start fullscreen (no borders)" << std::endl;
	cmd->Out << L"  -windowed or -m_window              : Start in windowed mode" << std::endl;

	// Print other options
	cmd->Out << L"\nOther Options:" << std::endl;
	cmd->Out << L"  -cmd                              : Show m_console m_window" << std::endl;
	cmd->Out << L"  -help                       : Show this help message" << std::endl;

	// Print examples
	cmd->Out << L"\nExamples:" << std::endl;
	cmd->Out << L"  program.exe -maximized" << std::endl;
	cmd->Out << L"  program.exe -width 1920 -height 1080" << std::endl;
	cmd->Out << L"  program.exe -fullscreen" << std::endl;
	cmd->Out << L"  program.exe -x 100 -y 100 -width 800 -height 600" << std::endl;

	cmd->Out << L"\n==============================\n" << std::endl;

	cmd->Out << tc::reset;
}

void CommandLineArguments::ShowCmd() {

    if(!m_bShowCmd) return;

	ShowConsole();

	std::cout << "\nThis message appears in the new m_console m_window.\n" << std::endl;


	if (m_bShowHelp) PrintHelp();

	std::wcout << std::endl;
}




void CommandLineArguments::ShowConsole() {}
} // namespace app
