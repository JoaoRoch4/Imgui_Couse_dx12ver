#include "PCH.hpp"
#include "Classes.hpp"

namespace app {
ConsoleWindow::ConsoleWindow() :
InputBuf(),
Items(),
Commands(),
History(),
HistoryPos(),
Filter(),
AutoScroll(),
ScrollToBottom(),
m_MyCommmands{},
m_memory(nullptr),
m_cmd(nullptr),
m_bEnableFileLogging(true),
m_logFilePath(L"console_log.txt"),
m_cmdArgs(nullptr),
m_window(nullptr),
m_font_manager(nullptr),
m_font_manager_window(nullptr),
m_debug_window(nullptr),
m_configManager(nullptr),
m_window_obj(nullptr)


{

	m_memory = MemoryManagement::Get_MemoryManagement();
	memset(InputBuf, 0, sizeof(InputBuf));
	HistoryPos = -1;

	// Initialize file logging
	EnableFileLogging(true);

	// Built-in commands for tab completion - convert to ImWchar
	auto AddCommand = [this](const char* cmd) {
		ImWchar wcmd[64];
		ImTextStrFromUtf8(wcmd, IM_ARRAYSIZE(wcmd), cmd, nullptr);
		Commands.push_back(Wcsdup(wcmd));
	};

	AddCommand("HELP");
	AddCommand("HISTORY");
	AddCommand("CLEAR");
	AddCommand("STATUS");
	AddCommand("DEMO");
	AddCommand("EXIT");
	AddCommand("QUIT");
	AddCommand("COMMANDS");

	AutoScroll	   = true;
	ScrollToBottom = false;
	AddLog("[success] ✨ Welcome to Dear ImGui Console! 🎮\n");
	AddLog("[info] 📋 Type 'help' or 'commands' to get started.\n");
}

ConsoleWindow::~ConsoleWindow() {
	ClearLog();
	for (int i = 0; i < History.Size; i++) {
		if (History[i]) ImGui::MemFree(History[i]);
	}
	for (int i = 0; i < Commands.Size; i++) {
		if (Commands[i]) ImGui::MemFree((void*)Commands[i]);
	}

	// Close log file
	if (m_logFile.is_open()) {
		m_logFile.flush();
		m_logFile.close();
	}
}

HRESULT ConsoleWindow::Alloc() {
	m_cmd = m_memory->Get_OutputConsole();

	m_cmdArgs		= m_memory->Get_CommandLineArguments();
	m_consoleWindow = m_memory->Get_ConsoleWindow();
	m_configManager = m_memory->Get_ConfigManager();
	m_font_manager	= m_memory->Get_FontManager();
	return S_OK;
}

void ConsoleWindow::Open() {

	Alloc();

	// Helper to convert UTF-8 string to wstring
	auto Utf8ToWstring = [](const std::string& str) -> std::wstring {
		if (str.empty()) return std::wstring();
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
		std::wstring wstr(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
		return wstr;
	};

	std::vector<std::wstring> Commands{L"exit",		L"quit",   L"show cmd", L"demo",	L"hide cmd",
									   L"commands", L"status", L"HELP",		L"HISTORY", L"CLEAR"};
	std::sort(Commands.begin(), Commands.end());

	for (uint64_t i = 0; i < Commands.size(); i++) {
		// Convert to lowercase for storage
		std::wstring lower_cmd = Commands.at(i);
		std::transform(lower_cmd.begin(), lower_cmd.end(), lower_cmd.begin(), ::towlower);
		m_MyCommmands.insert(std::make_pair(lower_cmd, i));
	}
}

void ConsoleWindow::Tick() {}

void ConsoleWindow::Close() {}

char* ConsoleWindow::Strdup(const char* s) {
	IM_ASSERT(s);
	size_t len = strlen(s) + 1;
	void*  buf = ImGui::MemAlloc(len);
	IM_ASSERT(buf);
	return (char*)memcpy(buf, (const void*)s, len);
}

ImWchar* ConsoleWindow::Wcsdup(const ImWchar* s) {
	IM_ASSERT(s);
	size_t len = 0;
	while (s[len]) len++;
	len++; // null terminator
	void* buf = ImGui::MemAlloc(len * sizeof(ImWchar));
	IM_ASSERT(buf);
	return (ImWchar*)memcpy(buf, (const void*)s, len * sizeof(ImWchar));
}

void ConsoleWindow::ClearLog() {
	for (int i = 0; i < Items.Size; i++) ImGui::MemFree(Items[i]);
	Items.clear();
}

void ConsoleWindow::ExecMyCommand(const ImWchar* command_line) {
	// Convert ImWchar to UTF-8 for processing
	char utf8_buf[256];
	ImTextStrToUtf8(utf8_buf, sizeof(utf8_buf), command_line, nullptr);
	AddLog("# %s\n", utf8_buf);

	// Insert into history. First find match and delete it so it can be pushed to the back.
	// This isn't trying to be smart or optimal.
	HistoryPos = -1;
	for (int i = History.Size - 1; i >= 0; i--)
		if (Wcsicmp(History[i], command_line) == 0) {
			ImGui::MemFree(History[i]);
			History.erase(History.begin() + i);
			break;
		}
	History.push_back(Wcsdup(command_line));

	// Process command (use utf8_buf for comparisons)
	// Command dispatch table - much more efficient than if-else ladder
	static const std::unordered_map<std::string, std::function<void(ConsoleWindow*)>> commandHandlers = {
		{"exit", [](ConsoleWindow* console) {
			console->AddLog("[warning] 👋 Exiting application...\n");
			std::exit(EXIT_SUCCESS);
		}},
		{"quit", [](ConsoleWindow* console) {
			console->AddLog("[warning] 👋 Exiting application...\n");
			std::exit(EXIT_SUCCESS);
		}},
		{"demo", [](ConsoleWindow* console) {
			console->m_memory->m_bShow_demo_window = !console->m_memory->m_bShow_demo_window;
			console->AddLog("[success] 🎪 Demo window %s\n",
				console->m_memory->m_bShow_demo_window ? "✅ enabled" : "❌ disabled");
		}},
		{"show cmd", [](ConsoleWindow* console) {
			console->m_memory->m_bShowCmd = true;
			console->m_cmd->ShowConsole(true);
			console->AddLog("[success] 👁️ Console window shown\n");
		}},
		{"hide cmd", [](ConsoleWindow* console) {
			console->m_memory->m_bShowCmd = false;
			console->m_cmd->ShowConsole(false);
			console->AddLog("[success] 🙈 Console window hidden\n");
		}},
		{"commands", [](ConsoleWindow* console) {
			console->AddLog("[info] 📜 Available commands:\n");
			for(const auto& it : console->m_MyCommmands) {
				std::string cmd_utf8(it.first.begin(), it.first.end());
				console->AddLog("[cmd]   ▸ %s\n", cmd_utf8.c_str());
			}
		}},
		{"clear", [](ConsoleWindow* console) {
			console->ClearLog();
			//console->AddLog("[success] 🧹 Console cleared\n");
		}},
         {       "cls", [](ConsoleWindow* console) {
            console->ClearLog();
            //console->AddLog("[success] 🧹 Console cleared\n");
            }
    },
		{"help", [](ConsoleWindow* console) {
			console->AddLog("[info] ❓ Available Commands:\n");
			for(const auto& it : console->m_MyCommmands) {
				std::string cmd_utf8(it.first.begin(), it.first.end());
				console->AddLog("[cmd]   ▸ %s\n", cmd_utf8.c_str());
			}
		}},
		{"history", [](ConsoleWindow* console) {
			console->AddLog("[info] 📚 Command History:\n");
			int first = console->History.Size - 10;
			for(int i = first > 0 ? first : 0; i < console->History.Size; i++) {
				char hist_utf8[256];
				ImTextStrToUtf8(hist_utf8, sizeof(hist_utf8), console->History[i], nullptr);
				console->AddLog("[history] 📌 %3d: %s\n", i, hist_utf8);
			}
		}},
		{"status", [](ConsoleWindow* console) {
			console->AddLog("[info] 📊 Generating status report...\n");
			console->m_cmd->Out.ShowSystemStatus();
		}}
	};
	
	// Look up and execute command - O(1) hash lookup instead of O(n) comparisons
	auto it = commandHandlers.find(utf8_buf);
	if(it != commandHandlers.end()) {
		it->second(this);
	} else {
		AddLog("[error] ❌ Unknown command: '%s'\n", utf8_buf);
	}

	// On command input, we scroll to bottom even if AutoScroll==false
	ScrollToBottom = true;
}

void ConsoleWindow::AddLog(const char* fmt, ...) {
	// UTF-8 version - converts to ImWchar for storage
	char	buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);

	// Convert UTF-8 to ImWchar for storage
	ImWchar wbuf[1024];
	ImTextStrFromUtf8(wbuf, IM_ARRAYSIZE(wbuf), buf, nullptr);
	Items.push_back(Wcsdup(wbuf));

	// Write to log file if enabled
	if (m_bEnableFileLogging && m_logFile.is_open()) {
		// Get current timestamp
		auto now  = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);
		auto ms =
			std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

		struct tm timeinfo;
		localtime_s(&timeinfo, &time);

		// Write timestamp and message
		m_logFile << std::put_time(&timeinfo, "[%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0')
				  << std::setw(3) << ms.count() << "] " << buf;
		m_logFile.flush(); // Ensure immediate write
	}
}

void ConsoleWindow::AddLog(const ImWchar* fmt, ...) {
	// Wide character version
	ImWchar buf[1024];
	va_list args;
	va_start(args, fmt);
	// Note: There's no standard vswprintf_s that works with ImWchar
	// So we'll convert format to char, process, then convert back
	char fmt_utf8[256];
	ImTextStrToUtf8(fmt_utf8, sizeof(fmt_utf8), fmt, nullptr);

	char result_utf8[1024];
	vsnprintf(result_utf8, sizeof(result_utf8), fmt_utf8, args);
	result_utf8[sizeof(result_utf8) - 1] = 0;
	va_end(args);

	// Convert result back to ImWchar
	ImTextStrFromUtf8(buf, IM_ARRAYSIZE(buf), result_utf8, nullptr);
	Items.push_back(Wcsdup(buf));

	// Write to log file if enabled
	if (m_bEnableFileLogging && m_logFile.is_open()) {
		// Get current timestamp
		auto now  = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);
		auto ms =
			std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

		struct tm timeinfo;
		localtime_s(&timeinfo, &time);

		// Write timestamp and message
		m_logFile << std::put_time(&timeinfo, "[%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0')
				  << std::setw(3) << ms.count() << "] " << result_utf8;
		m_logFile.flush();
	}
}

void ConsoleWindow::AddLogW(const wchar_t* fmt, ...) {
	// Windows wchar_t convenience version
	wchar_t wbuf[1024];
	va_list args;
	va_start(args, fmt);
	vswprintf_s(wbuf, _countof(wbuf), fmt, args);
	va_end(args);

	// Convert wchar_t to ImWchar
	ImWchar imwbuf[1024];
	size_t	i = 0;
	for (; i < 1023 && wbuf[i]; i++) { imwbuf[i] = (ImWchar)wbuf[i]; }
	imwbuf[i] = 0;

	Items.push_back(Wcsdup(imwbuf));
}

void ConsoleWindow::Render(const char* title, bool* p_open) {
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin(title, p_open)) {
		ImGui::End();
		return;
	}

	// Title bar context menu
	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Close Console")) *p_open = false;
		ImGui::EndPopup();
	}

	// Clear button
	if (ImGui::Button("Clear")) { ClearLog(); }
	ImGui::SameLine();
	bool copy_to_clipboard = ImGui::Button("Copy");

	ImGui::Separator();

	// Options menu
	if (ImGui::BeginPopup("Options")) {
		ImGui::Checkbox("Auto-scroll", &AutoScroll);
		ImGui::EndPopup();
	}

	// Options, Filter
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O, ImGuiInputFlags_Tooltip);
	if (ImGui::Button("Options")) ImGui::OpenPopup("Options");
	ImGui::SameLine();
	Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
	ImGui::Separator();

	// Reserve enough left-over height for 1 separator + 1 input text
	const float footer_height_to_reserve =
		ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve),
						  ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar)) {
		if (ImGui::BeginPopupContextWindow()) {
			if (ImGui::Selectable("Clear")) ClearLog();
			ImGui::EndPopup();
		}

		// Display every line as a separate entry so we can change their color or add custom
		// widgets. If you only want raw text you can use ImGui::TextUnformatted(log.begin(),
		// log.end()); NB- if you have thousands of entries this approach may be too inefficient and
		// may require user-side clipping to only process visible items. The clipper will
		// automatically measure the height of your first item and then "seek" to display only items
		// in the visible area. To use the clipper we can replace your standard loop:
		//      for (int i = 0; i < Items.Size; i++)
		//   With:
		//      ImGuiListClipper clipper;
		//      clipper.Begin(Items.Size);
		//      while (clipper.Step())
		//         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
		// - That your items are evenly spaced (same height)
		// - That you have cheap random access to your elements (you can access them given their
		// index,
		//   without processing all the ones before)
		// You cannot this code as-is if a filter is active because it breaks the 'cheap
		// random-access' property. We would need random-access on the post-filtered list. A typical
		// application wanting coarse clipping and filtering may want to pre-compute an array of
		// indices or offsets of items that passed the filtering test, recomputing this array when
		// user changes the filter, and appending newly elements as they are inserted. This is left
		// as a task to the user until we can manage to improve this example code! If your items are
		// of variable height:
		// - Split them into same height items would be simpler and facilitate random-seeking into
		// your list.
		// - Consider using manual call to IsRectVisible() and skipping extraneous decoration from
		// your items.
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
		if (copy_to_clipboard) ImGui::LogToClipboard();

		// Persistent color state - maintains color across lines until reset
		ImVec4 currentColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		bool   colorActive	= false;

		for (const ImWchar* item : Items) {
			// Convert ImWchar to UTF-8 for display and filtering
			char item_utf8[1024];
			ImTextStrToUtf8(item_utf8, sizeof(item_utf8), item, nullptr);

			if (!Filter.PassFilter(item_utf8)) continue;

			// Check for color control tags
			bool colorChanged = false;

			// Reset tag - clears color
			if (strstr(item_utf8, "[reset]")) {
				if (colorActive) {
					ImGui::PopStyleColor();
					colorActive = false;
				}
				currentColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
				colorChanged = true;
			}
			// Check for color tags
			else if (strstr(item_utf8, "[error]") || strstr(item_utf8, "[red]")) {
				currentColor = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
				colorChanged = true;
			} else if (strstr(item_utf8, "[warning]") || strstr(item_utf8, "[yellow]")) {
				currentColor = ImVec4(1.0f, 0.85f, 0.2f, 1.0f);
				colorChanged = true;
			} else if (strstr(item_utf8, "[success]") || strstr(item_utf8, "[green]")) {
				currentColor = ImVec4(0.3f, 1.0f, 0.3f, 1.0f);
				colorChanged = true;
			} else if (strstr(item_utf8, "[info]") || strstr(item_utf8, "[blue]") ||
					   strstr(item_utf8, "[cyan]")) {
				currentColor = ImVec4(0.4f, 0.8f, 1.0f, 1.0f);
				colorChanged = true;
			} else if (strstr(item_utf8, "[cmd]")) {
				currentColor = ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
				colorChanged = true;
			} else if (strstr(item_utf8, "[history]") || strstr(item_utf8, "[magenta]")) {
				currentColor = ImVec4(0.8f, 0.6f, 1.0f, 1.0f);
				colorChanged = true;
			} else if (strstr(item_utf8, "[grey]")) {
				currentColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
				colorChanged = true;
			} else if (strstr(item_utf8, "[white]")) {
				currentColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
				colorChanged = true;
			} else if (strstr(item_utf8, "[bright_red]")) {
				currentColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
				colorChanged = true;
			} else if (strstr(item_utf8, "[bright_green]")) {
				currentColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
				colorChanged = true;
			} else if (strstr(item_utf8, "[bright_yellow]")) {
				currentColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
				colorChanged = true;
			} else if (strstr(item_utf8, "[bright_blue]")) {
				currentColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f);
				colorChanged = true;
			} else if (strstr(item_utf8, "[bright_magenta]")) {
				currentColor = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);
				colorChanged = true;
			} else if (strstr(item_utf8, "[bright_cyan]")) {
				currentColor = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
				colorChanged = true;
			} else if (strstr(item_utf8, "[bright_white]")) {
				currentColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
				colorChanged = true;
			} else if (strncmp(item_utf8, "# ", 2) == 0) {
				currentColor = ImVec4(1.0f, 0.8f, 0.4f, 1.0f);
				colorChanged = true;
			}

			// Apply new color if changed
			if (colorChanged && !strstr(item_utf8, "[reset]")) {
				if (colorActive) { ImGui::PopStyleColor(); }
				ImGui::PushStyleColor(ImGuiCol_Text, currentColor);
				colorActive = true;
			}

			ImGui::TextUnformatted(item_utf8);
		}

		// Pop color if still active at end of rendering
		if (colorActive) { ImGui::PopStyleColor(); }

		if (copy_to_clipboard) ImGui::LogFinish();
		if (copy_to_clipboard) ImGui::LogFinish();
		// Keep up at the bottom of the scroll region if we were already at the bottom at the
		// beginning of the frame. Using a scrollbar or mouse-wheel will take away from the bottom
		// edge.
		if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			ImGui::SetScrollHereY(1.0f);
		ScrollToBottom = false;

		ImGui::PopStyleVar();
	}
	ImGui::EndChild();
	ImGui::Separator();

	// Command-line
	bool				reclaim_focus = false;
	ImGuiInputTextFlags input_text_flags =
		ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll |
		ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;

	// Use ImGui's wide character input
	char utf8_input[256 * 4]; // UTF-8 can be up to 4 bytes per character
	ImTextStrToUtf8(utf8_input, sizeof(utf8_input), InputBuf, nullptr);

	if (ImGui::InputTextWithHint("##Input", "Type a command...", utf8_input,
								 IM_ARRAYSIZE(utf8_input), input_text_flags, &TextEditCallbackStub,
								 (void*)this)) {
		// Convert back to ImWchar
		ImTextStrFromUtf8(InputBuf, IM_ARRAYSIZE(InputBuf), utf8_input, nullptr);

		ImWchar* s = InputBuf;
		Wcstrim(s);
		if (s[0]) {
			// Convert to UTF-8 for lowercase conversion
			char temp_utf8[256];
			ImTextStrToUtf8(temp_utf8, sizeof(temp_utf8), s, nullptr);
			Helpers::charToLower(temp_utf8);
			ImTextStrFromUtf8(InputBuf, IM_ARRAYSIZE(InputBuf), temp_utf8, nullptr);
			ExecMyCommand(InputBuf);
		}
		InputBuf[0]	  = 0;
		reclaim_focus = true;
	}

	// Auto-focus on m_window apparition
	ImGui::SetItemDefaultFocus();
	if (reclaim_focus) ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

	ImGui::End();
}

int ConsoleWindow::TextEditCallbackStub(ImGuiInputTextCallbackData* data) {
	ConsoleWindow* m_console = (ConsoleWindow*)data->UserData;
	return m_console->TextEditCallback(data);
}

int ConsoleWindow::TextEditCallback(ImGuiInputTextCallbackData* data) {
	// AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart,
	// data->SelectionEnd);
	switch (data->EventFlag) {
		case ImGuiInputTextFlags_CallbackCompletion: {
			// Example of TEXT COMPLETION

			// Locate beginning of current word
			const char* word_end   = data->Buf + data->CursorPos;
			const char* word_start = word_end;
			while (word_start > data->Buf) {
				const char c = word_start[-1];
				if (c == ' ' || c == '\t' || c == ',' || c == ';') break;
				word_start--;
			}

			// Build a list of candidates - convert ImWchar commands to UTF-8 for comparison
			ImVector<const char*> candidates;
			ImVector<char*>		  candidate_utf8_storage; // Store converted strings temporarily

			for (int i = 0; i < Commands.Size; i++) {
				char* cmd_utf8 = (char*)ImGui::MemAlloc(256);
				ImTextStrToUtf8(cmd_utf8, 256, Commands[i], nullptr);

				if (Strnicmp(cmd_utf8, word_start, (int)(word_end - word_start)) == 0) {
					candidates.push_back(cmd_utf8);
					candidate_utf8_storage.push_back(cmd_utf8);
				} else {
					ImGui::MemFree(cmd_utf8);
				}
			}

			if (candidates.Size == 0) {
				// No match
				AddLog("[warning] ⚠️ No match for \"%.*s\"!\n", (int)(word_end - word_start),
					   word_start);
			} else if (candidates.Size == 1) {
				// Single match. Delete the beginning of the word and replace it entirely so we've
				// got nice casing.
				data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
				data->InsertChars(data->CursorPos, candidates[0]);
				data->InsertChars(data->CursorPos, " ");
			} else {
				// Multiple matches. Complete as much as we can..
				// So inputting "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as
				// matches.
				int match_len = (int)(word_end - word_start);
				for (;;) {
					int	 c						= 0;
					bool all_candidates_matches = true;
					for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
						if (i == 0) c = toupper(candidates[i][match_len]);
						else if (c == 0 || c != toupper(candidates[i][match_len]))
							all_candidates_matches = false;
					if (!all_candidates_matches) break;
					match_len++;
				}

				if (match_len > 0) {
					data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
					data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
				}

				// List matches
				AddLog("[info] 💡 Possible matches:\n");
				for (int i = 0; i < candidates.Size; i++) AddLog("[cmd]   ▸ %s\n", candidates[i]);
			}

			// Clean up temporary UTF-8 strings
			for (int i = 0; i < candidate_utf8_storage.Size; i++) {
				ImGui::MemFree(candidate_utf8_storage[i]);
			}

			break;
		}
		case ImGuiInputTextFlags_CallbackHistory: {
			// Example of HISTORY
			const int prev_history_pos = HistoryPos;
			if (data->EventKey == ImGuiKey_UpArrow) {
				if (HistoryPos == -1) HistoryPos = History.Size - 1;
				else if (HistoryPos > 0) HistoryPos--;
			} else if (data->EventKey == ImGuiKey_DownArrow) {
				if (HistoryPos != -1)
					if (++HistoryPos >= History.Size) HistoryPos = -1;
			}

			// A better implementation would preserve the data on the current input line along with
			// cursor position.
			if (prev_history_pos != HistoryPos) {
				static const ImWchar empty_str[1] = {0};
				const ImWchar* history_str = (HistoryPos >= 0) ? History[HistoryPos] : empty_str;
				// Convert ImWchar history to UTF-8 for ImGui input
				char utf8_hist[256];
				ImTextStrToUtf8(utf8_hist, sizeof(utf8_hist), history_str, nullptr);
				data->DeleteChars(0, data->BufTextLen);
				data->InsertChars(0, utf8_hist);
			}
		}
	}
	return 0;
}

int ConsoleWindow::Stricmp(const char* s1, const char* s2) {
	int d;
	while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
		s1++;
		s2++;
	}
	return d;
}

int ConsoleWindow::Wcsicmp(const ImWchar* s1, const ImWchar* s2) {
	int d;
	while ((d = towupper(*s2) - towupper(*s1)) == 0 && *s1) {
		s1++;
		s2++;
	}
	return d;
}

int ConsoleWindow::Strnicmp(const char* s1, const char* s2, int n) {
	int d = 0;
	while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
		s1++;
		s2++;
		n--;
	}
	return d;
}

int ConsoleWindow::Wcsnicmp(const ImWchar* s1, const ImWchar* s2, int n) {
	int d = 0;
	while (n > 0 && (d = towupper(*s2) - towupper(*s1)) == 0 && *s1) {
		s1++;
		s2++;
		n--;
	}
	return d;
}

void ConsoleWindow::Strtrim(char* s) {
	char* str_end = s + strlen(s);
	while (str_end > s && str_end[-1] == ' ') str_end--;
	*str_end = 0;
}

void ConsoleWindow::Wcstrim(ImWchar* s) {
	ImWchar* str_end = s;
	while (*str_end) str_end++;
	while (str_end > s && str_end[-1] == ' ') str_end--;
	*str_end = 0;
}

size_t ConsoleWindow::Wcslen(const ImWchar* s) {
	size_t len = 0;
	while (s[len]) len++;
	return len;
}

void ConsoleWindow::EnableFileLogging(bool enable) {
	m_bEnableFileLogging = enable;

	if (enable && !m_logFile.is_open()) {
		// Open log file in append mode with UTF-8 encoding
		m_logFile.open(m_logFilePath, std::ios::out | std::ios::app);

		if (m_logFile.is_open()) {
			// Write session start marker
			auto	  now  = std::chrono::system_clock::now();
			auto	  time = std::chrono::system_clock::to_time_t(now);
			struct tm timeinfo;
			localtime_s(&timeinfo, &time);

			m_logFile << "\n" << std::string(80, '=') << "\n";
			m_logFile << "Console Log Session Started: "
					  << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << "\n";
			m_logFile << std::string(80, '=') << "\n";
			m_logFile.flush();
		}
	} else if (!enable && m_logFile.is_open()) {
		// Write session end marker
		auto	  now  = std::chrono::system_clock::now();
		auto	  time = std::chrono::system_clock::to_time_t(now);
		struct tm timeinfo;
		localtime_s(&timeinfo, &time);

		m_logFile << std::string(80, '=') << "\n";
		m_logFile << "Console Log Session Ended: " << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S")
				  << "\n";
		m_logFile << std::string(80, '=') << "\n\n";
		m_logFile.flush();
		m_logFile.close();
	}
}

void ConsoleWindow::SetLogFilePath(const std::wstring& path) {
	bool wasEnabled = m_bEnableFileLogging;

	// Close current log file if open
	if (m_logFile.is_open()) { EnableFileLogging(false); }

	m_logFilePath = path;

	// Reopen with new path if it was enabled
	if (wasEnabled) { EnableFileLogging(true); }
}

void ConsoleWindow::FlushLogFile() {
	if (m_logFile.is_open()) { m_logFile.flush(); }
}
} // namespace app
