#pragma once
#include "pch.hpp"
#include "Classes.hpp"

namespace app {

// Forward declaration
class ConsoleWindow;

class CustomOutput{
private:
	ConsoleWindow* m_consoleWindow;
	std::string m_buffer; // Buffer to accumulate text until newline

public:
	CustomOutput();
	void SetConsoleWindow(ConsoleWindow* consoleWindow);

	// Stream manipulator overload (for std::endl, std::flush, etc.)
	template <class elem = char, class _Traits = std::char_traits<char>>
	CustomOutput& operator<<(
		std::basic_ostream<char, std::char_traits<char>>&(__cdecl* _Pfn)(
			std::basic_ostream<char, std::char_traits<char>>&)) {
		_Pfn(std::cout);
		
		// If this is std::endl or std::flush, flush to ConsoleWindow
		if (_Pfn == static_cast<std::ostream&(*)(std::ostream&)>(std::endl) ||
		    _Pfn == static_cast<std::ostream&(*)(std::ostream&)>(std::flush)) {
			m_buffer += '\n';
			FlushToConsoleWindow();
		}
		
		return *this;
	}

	// Catch-all template for IO manipulators (hex, dec, termcolor, etc.)
	template<typename T>
	CustomOutput& operator<<(T val) {
		std::cout << val;
		return *this;
	}

	// Specific overloads for data types (take precedence over template)
	CustomOutput& operator<<(const long long& valor);
	CustomOutput& operator<<(const int& valor);
	CustomOutput& operator<<(const long double& valor);
	CustomOutput& operator<<(const float& valor);
	CustomOutput& operator<<(const char* dado);
	CustomOutput& operator<<(const wchar_t* dado);
	CustomOutput& operator<<(const std::string& dado);
	CustomOutput& operator<<(const std::wstring& dado);

	// Guaranteed output methods - always appear in both consoles
	void WriteLine(const std::string& message);
	void WriteLine(const std::wstring& message);
	void Write(const std::string& message);
	void Write(const std::wstring& message);
	
	// Status and information methods
	void ShowSystemStatus();

private:
	void FlushToConsoleWindow();
	void ForceFlush(); // Force immediate flush regardless of newline
};

class OutputConsole : public Master {
public:
	CustomOutput Out;
	virtual void Open() override;
	virtual void Tick() override;
	virtual void Close() override;
	OutputConsole();
	virtual ~OutputConsole();

	void ShowConsole(bool bShow);
	void SetConsoleWindow(ConsoleWindow* consoleWindow);
	
	// Show comprehensive application status
	void ShowStatus() { Out.ShowSystemStatus(); }

private:
	void CreateConsole();

	void setConsoleFontSize(int size);
	bool m_bWasConsoleCreated;
	bool m_bShouldCreateConsole;
	bool m_bShowConsole;
	HWND m_hWnd_console;
    MemoryManagement* m_memory;
    CommandLineArguments* m_args;
};

} // namespace app
