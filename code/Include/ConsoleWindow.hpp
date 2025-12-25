#pragma once

#include "PCH.hpp"

//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Console / ShowExampleAppConsole()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple m_console m_window, with scrolling, filtering, completion and history.
// For the m_console example, we are using a more C++ like approach of declaring a class to hold both data and functions.
namespace app {
class ConsoleWindow {
private:
	char				  InputBuf[256];
	ImVector<char*>		  Items;
	ImVector<const char*> Commands;
	ImVector<char*>		  History;
	int					  HistoryPos; // -1: new line, 0..History.Size-1 browsing history.
	ImGuiTextFilter		  Filter;
	bool				  AutoScroll;
	bool				  ScrollToBottom;

public:
	ConsoleWindow();
	~ConsoleWindow();

protected:
	// Portable helpers
	static int Stricmp(const char* s1, const char* s2);

	static int	 Strnicmp(const char* s1, const char* s2, int n);
	static char* Strdup(const char* s);
	static void	 Strtrim(char* s);


public:
	void ClearLog();


	void AddLog(const char* fmt, ...) IM_FMTARGS(2);

	void Render(const char* title, bool* p_open);

	void ExecCommand(const char* command_line);

	// In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
	static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);

	int	 TextEditCallback(ImGuiInputTextCallbackData* data);
	void ShowExampleAppConsole(bool* p_open) { Render("Example: Console", p_open); }
};
} // namespace app
