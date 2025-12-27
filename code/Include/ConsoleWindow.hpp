#pragma once

#include "PCH.hpp"
#include "Master.hpp"

//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Console / ShowExampleAppConsole()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple m_console m_window, with scrolling, filtering, completion and
// history. For the m_console example, we are using a more C++ like approach of declaring a class to
// hold both data and functions.
namespace app {
class ConsoleWindow : public Master {
private:
	ImWchar					 InputBuf[256];
	ImVector<ImWchar*>		 Items;
	ImVector<const ImWchar*> Commands;
	ImVector<ImWchar*>		 History;
	int						 HistoryPos; // -1: new line, 0..History.Size-1 browsing history.
	ImGuiTextFilter			 Filter;
	bool					 AutoScroll;
	bool					 ScrollToBottom;
	std::map<std::wstring, uint64_t> m_MyCommmands;
	class MemoryManagement*			 m_memory;
	class OutputConsole*			 m_cmd;

	// File logging
	std::ofstream m_logFile;
	bool		  m_bEnableFileLogging;
	std::wstring  m_logFilePath;

	class CommandLineArguments* m_cmdArgs;
	class ConsoleWindow*		m_consoleWindow;
	class WindowManager*		m_window;
	class FontManager*			m_font_manager;
	class FontManagerWindow*	m_font_manager_window;
	class DebugWindow*			m_debug_window;
	class ConfigManager*		m_configManager;
	class WindowClass*			m_window_obj;

public:
	ConsoleWindow();
	~ConsoleWindow();
	HRESULT Alloc();

	virtual void Open() override;
	virtual void Tick() override;
	virtual void Close() override;

protected:
	// Portable helpers - UTF-8 versions
	static int	 Stricmp(const char* s1, const char* s2);
	static int	 Strnicmp(const char* s1, const char* s2, int n);
	static char* Strdup(const char* s);
	static void	 Strtrim(char* s);

	// Wide character (ImWchar) versions
	static int		Wcsicmp(const ImWchar* s1, const ImWchar* s2);
	static int		Wcsnicmp(const ImWchar* s1, const ImWchar* s2, int n);
	static ImWchar* Wcsdup(const ImWchar* s);
	static void		Wcstrim(ImWchar* s);
	static size_t	Wcslen(const ImWchar* s);


public:
void ClearLog();

void ExecMyCommand(const ImWchar* command_line);

	// Command handlers - each command has its own method
	void CommandExit();
	void CommandQuit();
	void CommandDemo();
	void CommandShowCmd();
	void CommandHideCmd();
	void CommandList();
	void CommandClear();
	void CommandHelp();
	void CommandHistory();
	void CommandStatus();
	
	// Parameterized command handlers (with arguments)
	void CommandEcho(const std::string& args);
	void CommandSet(const std::string& args);
	void CommandLog(const std::string& args);

	// AddLog overloads for different string types
	void AddLog(const char* fmt, ...) IM_FMTARGS(2); // UTF-8 format string
	void AddLog(const ImWchar* fmt, ...);			 // ImWchar format string
	void AddLogW(const wchar_t* fmt, ...);			 // Windows wchar_t format string

	// File logging control
	void				EnableFileLogging(bool enable = true);
	void				SetLogFilePath(const std::wstring& path);
	bool				IsFileLoggingEnabled() const { return m_bEnableFileLogging; }
	const std::wstring& GetLogFilePath() const { return m_logFilePath; }
	void				FlushLogFile();

	void Render(const char* title, bool* p_open);

	// In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
	static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);

	int	 TextEditCallback(ImGuiInputTextCallbackData* data);
	void ShowExampleAppConsole(bool* p_open) { Render("Example: Console", p_open);
    }
};
} // namespace app
