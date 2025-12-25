// CommandLineArguments.hpp
// Header file for command line argument parsing
// This class now supports m_window configuration via command line arguments

#pragma once

// Include precompiled header with all necessary dependencies
#include "PCH.hpp"

// Include base class definition
#include "MemoryManagement.hpp"
namespace app {
/**
 * @brief Class that handles command line argument parsing
 * 
 * This class parses command line arguments and provides methods to query them.
 * Now includes support for m_window configuration arguments like:
 * - Window size (-width, -height)
 * - Window position (-x, -y)
 * - Window state (-maximized, -fullscreen, -windowed)
 * - Console mode (-cmd)
 */

class CommandLineArguments : public MemoryManagement {
public:
	// Constructor - Initializes all member variables
	CommandLineArguments();

	// Virtual destructor - Ensures proper cleanup in derived classes
	virtual ~CommandLineArguments();

	// Opens and initializes the command line parser
	virtual void Open() override;

	// Tick function (currently unused but required by Master interface)
	virtual void Tick() override;

	// Closes and cleans up resources
	virtual void Close() override;

	/**
     * @brief Gets the map of parsed command line arguments
     * @return Reference to the map containing argument name -> index pairs
     */
	std::map<std::wstring, uint64_t>& GetMap() { return Args; }

	// ===== NEW: Window Configuration Methods =====

	/**
     * @brief Checks if a specific argument exists in command line
     * @param arg The argument to check (case-insensitive)
     * @return true if argument exists, false otherwise
     */
	bool HasArgument(const std::wstring& arg);

	/**
     * @brief Gets the value following a command line argument
     * @param arg The argument name to search for
     * @param defaultValue Default value if argument not found or has no value
     * @return The value following the argument, or defaultValue
     */
	std::wstring GetArgumentValue(const std::wstring& arg, const std::wstring& defaultValue = L"");

	/**
     * @brief Gets an integer value from command line argument
     * @param arg The argument name to search for
     * @param defaultValue Default value if argument not found or invalid
     * @return The integer value, or defaultValue
     */
	int GetArgumentValueInt(const std::wstring& arg, int defaultValue = 0);

	/**
     * @brief Prints all available command line options to m_console
     * This is useful for showing help to users
     */
	void PrintHelp();

	void ShowCmd();

private:
	/**
     * @brief Parses the command line arguments into the Args map
     * @return 0 on success, EXIT_FAILURE on error
     */
	int GetInitArgs();

	/**
     * @brief Converts a wide string to lowercase
     * @param s The string to convert
     * @return The lowercase version of the string
     */
	std::wstring toLower(std::wstring s);

	// Map storing argument name (lowercase) -> index in szArgList
	std::map<std::wstring, uint64_t> Args;

	// Pointer to array of argument strings (allocated by CommandLineToArgvW)
	LPWSTR* szArgList;

	// Number of arguments parsed
	int argCount;

	// Flag indicating if m_console was launched/allocated
	bool bConsoleLaunched;

	void ShowConsole();

public:
	// Getter for m_console launched flag
	inline bool GetbConsoleLaunched() const { return bConsoleLaunched; }

	// Setter for m_console launched flag
	inline void SetbConsoleLaunched(bool bisConsoleLaunched) {
		bConsoleLaunched = bisConsoleLaunched;
	}

private:
	bool m_bShowCmd;
	bool m_bShowHelp;
	bool m_bShowArgs;
};
} // namespace app
