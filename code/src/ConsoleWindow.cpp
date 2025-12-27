/**
 * @file ConsoleWindow.cpp
 * @brief Implementation of the ConsoleWindow class for an
 * ImGui-based debug console.
 *
 * This file contains the implementation of a feature-rich console
 * window with command processing,
 * logging capabilities, history management, and file output. The
 * console supports UTF-8 and wide
 * character strings, provides color-coded output, and integrates
 * with ImGui's debug logging system.
 *
 * @author JoaoRoch4
 * @date 2024
 */

#include "PCH.hpp"
#include "Classes.hpp"
#include "ConsoleWindow.hpp"

namespace app {

/**
 * @brief Default constructor for ConsoleWindow.
 *
 * Initializes all member variables to their
 * default values. Memory allocation
 * and full initialization occur in the Start() and Alloc()
 * methods.
 */
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
m_LastDebugLogPos(),
m_bEnableFileLogging(),
m_logFile(),
m_logFilePath(),
m_memory(nullptr),
m_cmd(nullptr),
m_cmdArgs(nullptr),
m_window(nullptr),
m_font_manager(nullptr),
m_font_manager_window(nullptr),
m_debug_window(nullptr),
m_configManager(nullptr),
m_window_obj(nullptr),
m_App(nullptr),
m_ConsoleInputHandler(nullptr),
m_ConfigManager(nullptr),
m_StyleManager(nullptr),
m_DxDemos(nullptr),
m_DX12Renderer(nullptr),
m_FrameContext(nullptr)

{}

/**
 * @brief Destructor for ConsoleWindow.
 *
 * Cleans up all allocated resources including log
 * items, command history,
 * and closes the log file if open. Sets all pointers to nullptr for
 * safety.
 */
ConsoleWindow::~ConsoleWindow() {
	ClearLog();
	for (int i = 0; i < History.Size; i++) ImGui::MemFree(History[i]);
	for (int i = 0; i < Commands.Size; i++) {
		if (Commands[i]) ImGui::MemFree((void*)Commands[i]);
	}

	// Close log file
	if (m_logFile.is_open()) {
		m_logFile.flush();
		m_logFile.close();
	}

	m_cmd				  = nullptr;
	m_cmdArgs			  = nullptr;
	m_configManager		  = nullptr;
	m_font_manager		  = nullptr;
	m_font_manager_window = nullptr;
	m_debug_window		  = nullptr;
	m_window			  = nullptr;
	m_window_obj		  = nullptr;
	m_App				  = nullptr;
	m_ConsoleInputHandler = nullptr;
	m_ConfigManager		  = nullptr;
	m_StyleManager		  = nullptr;
	m_DX12Renderer		  = nullptr;
}

/**
 * @brief Allocates and retrieves all required dependencies from MemoryManagement.
 *
 *
 * Retrieves pointers to all necessary subsystems (OutputConsole, ConfigManager,
 * FontManager,
 * etc.) from the MemoryManagement singleton. If any required object
 * is not available, logs an
 * error and returns E_FAIL.
 *
 * @return S_OK on success, E_FAIL if any dependency cannot be
 * allocated.
 * @throws std::runtime_error if a required object is not found in MemoryManagement.

 */
HRESULT ConsoleWindow::Alloc() {
	// Retrieve all required objects from MemoryManagement singleton
	try {
		m_cmd				  = m_memory->Get_OutputConsole();
		m_cmdArgs			  = m_memory->Get_CommandLineArguments();
		m_configManager		  = m_memory->Get_ConfigManager();
		m_font_manager		  = m_memory->Get_FontManager();
		m_font_manager_window = m_memory->Get_FontManagerWindow();
		m_debug_window		  = m_memory->Get_DebugWindow();
		m_window			  = m_memory->Get_WindowManager();
		m_window_obj		  = m_memory->Get_WindowClass();
		m_App				  = m_memory->Get_App();
		m_ConsoleInputHandler = m_memory->Get_ConsoleInputHandler();
		m_ConfigManager		  = m_memory->Get_ConfigManager();
		m_StyleManager		  = m_memory->Get_StyleManager();
		m_DX12Renderer		  = m_memory->Get_DX12Renderer();
	} catch (const std::runtime_error& e) {
		// Log error if any object is not allocated
		AddLog("[error] ❌ Failed to allocate ConsoleWindow dependencies: %s\n", e.what());
		return E_FAIL;
	}

	return S_OK;
}

/**
 * @brief Initializes the console window and sets up built-in commands.
 *
 * Performs initial
 * setup including:
 * - Retrieving the MemoryManagement singleton
 * - Clearing the input buffer
 *
 * - Enabling file logging
 * - Registering built-in commands for tab completion
 * - Displaying
 * welcome messages
 *
 * This method should be called before Open() to ensure proper
 * initialization.
 */
void ConsoleWindow::Start() {

	m_memory = MemoryManagement::Get_MemoryManagement_Singleton();
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
	AddCommand("ECHO");
	AddCommand("SET");
	AddCommand("LOG");
	AddCommand("SHOW");
	AddCommand("HIDE");
	AddCommand("BREAK");
	AddCommand("FONTS");

	AutoScroll	   = true;
	ScrollToBottom = false;
	AddLog("[success] ✨ Welcome to Dear ImGui Console! 🎮\n");
	AddLog("[info] 📋 Type 'help' or 'commands' to get started.\n");
}

/**
 * @brief Opens and fully initializes the console window.
 *
 * Calls Start() to perform basic
 * initialization, then Alloc() to retrieve
 * dependencies. Finally, builds the command map for
 * command lookup and
 * auto-completion.
 *
 * @note This is an override of the Master class
 * virtual method.
 */
void ConsoleWindow::Open() {
	Start();
	Alloc();

	// Helper to convert UTF-8 string to wstring
	auto Utf8ToWstring = [](const std::string& str) -> std::wstring {
		if (str.empty()) return std::wstring();
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
		std::wstring wstr(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
		return wstr;
	};

	std::vector<std::wstring> Commands{L"exit",		L"quit",   L"show", L"hide",	L"demo",
									   L"commands", L"status", L"HELP", L"HISTORY", L"CLEAR",
									   L"echo",		L"set",	   L"log",	L"break",	L"fonts"};
	std::sort(Commands.begin(), Commands.end());

	for (uint64_t i = 0; i < Commands.size(); i++) {
		// Convert to lowercase for storage
		std::wstring lower_cmd = Commands.at(i);
		std::transform(lower_cmd.begin(), lower_cmd.end(), lower_cmd.begin(), ::towlower);
		m_MyCommmands.insert(std::make_pair(lower_cmd, i));
	}
}

/**
 * @brief Per-frame update method for the console window.
 *
 * Called every frame to:
 * -
 * Update debug logs from ImGui context
 * - Track new log entries for auto-scroll behavior
 * -
 * Periodically flush the log file to ensure data persistence
 *
 * @note This is an override of the
 * Master class virtual method.
 */
void ConsoleWindow::Tick() {
	// Update debug logs from ImGui context (capture logs every frame)
	UpdateDebugLog();

	// Track new log entries for auto-scroll
	static int last_item_count = 0;
	if (Items.Size > last_item_count) {
		if (AutoScroll) { ScrollToBottom = true; }
		last_item_count = Items.Size;
	}

	// Periodically flush log file to ensure data persistence
	static int flush_counter = 0;
	if (m_bEnableFileLogging && m_logFile.is_open()) {
		if (++flush_counter >= 60) { // Once per second at 60 FPS
			m_logFile.flush();
			flush_counter = 0;
		}
	}
}

/**
 * @brief Closes the console window.
 *
 * Currently a no-op implementation. Cleanup is handled
 * by the destructor.
 *
 * @note This is an override of the Master class virtual method.
 */
void ConsoleWindow::Close() {}

/**
 * @brief Duplicates a UTF-8 string using ImGui's memory allocator.
 *
 * @param s The
 * null-terminated UTF-8 string to duplicate.
 * @return A newly allocated copy of the string.
 * Caller must free with ImGui::MemFree().
 * @note Asserts if s is nullptr.
 */
char* ConsoleWindow::Strdup(const char* s) {
	IM_ASSERT(s);
	size_t len = strlen(s) + 1;
	void*  buf = ImGui::MemAlloc(len);
	IM_ASSERT(buf);
	return (char*)memcpy(buf, (const void*)s, len);
}

/**
 * @brief Duplicates a wide character string using ImGui's memory allocator.
 *
 * @param s The
 * null-terminated ImWchar string to duplicate.
 * @return A newly allocated copy of the string.
 * Caller must free with ImGui::MemFree().
 * @note Asserts if s is nullptr.
 */
ImWchar* ConsoleWindow::Wcsdup(const ImWchar* s) {
	IM_ASSERT(s);
	size_t len = 0;
	while (s[len]) len++;
	len++; // null terminator
	void* buf = ImGui::MemAlloc(len * sizeof(ImWchar));
	IM_ASSERT(buf);
	return (ImWchar*)memcpy(buf, (const void*)s, len * sizeof(ImWchar));
}

/**
 * @brief Clears all log entries from the console.
 *
 * Clears all log item strings from the
 * Items vector.
 * This operation cannot be undone.
 */
void ConsoleWindow::ClearLog() {
	for (int i = 0; i < Items.Size; i++) ImGui::MemFree(Items[i]);
	Items.clear();
}

/**
 * @brief Executes a console command.
 *
 * Parses the command line into command name and
 * arguments, then dispatches
 * to the appropriate command handler using a hash map lookup.
 * Supports both
 * simple commands (no arguments) and parameterized commands (with arguments).
 *
 * Commands are case-insensitive.
 *
 * @param command_line The wide character command string to
 * execute.
 *
 * @note Command history is automatically updated with each execution.
 * @note Uses
 * std::variant for type-safe command dispatch.
 */
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

	// Parse command and arguments
	std::string full_command = utf8_buf;
	std::string command_name;
	std::string args;

	// Split command and arguments
	size_t first_space = full_command.find_first_of(" \t");
	if (first_space != std::string::npos) {
		command_name	 = full_command.substr(0, first_space);
		args			 = full_command.substr(first_space + 1);
		// Trim leading whitespace from args
		size_t arg_start = args.find_first_not_of(" \t");
		if (arg_start != std::string::npos) {
			args = args.substr(arg_start);
		} else {
			args.clear();
		}
	} else {
		command_name = full_command;
	}

	// Convert command to lowercase for case-insensitive comparison
	std::string command_lower = command_name;
	std::transform(command_lower.begin(), command_lower.end(), command_lower.begin(), ::tolower);

	// Command types - using variant for type-safe command representation
	struct SimpleCommand {
		std::function<void(ConsoleWindow*)> handler;
	};

	struct ParameterizedCommand {
		std::function<void(ConsoleWindow*, const std::string&)> handler;
	};

	using CommandVariant = std::variant<SimpleCommand, ParameterizedCommand>;

	// Command dispatch table using variant for extensibility and type safety
	static const std::unordered_map<std::string, CommandVariant> commandHandlers = {
		// Simple commands (no arguments)
		{"exit", SimpleCommand{&ConsoleWindow::CommandExit}},
		{"quit", SimpleCommand{&ConsoleWindow::CommandQuit}},
		{"demo", SimpleCommand{&ConsoleWindow::CommandDemo}},
		{"show", SimpleCommand{&ConsoleWindow::CommandShowCmd}},
		{"hide", SimpleCommand{&ConsoleWindow::CommandHideCmd}},
		{"commands", SimpleCommand{&ConsoleWindow::CommandList}},
		{"clear", SimpleCommand{&ConsoleWindow::CommandClear}},
		{"cls", SimpleCommand{&ConsoleWindow::CommandClear}},
		{"help", SimpleCommand{&ConsoleWindow::CommandHelp}},
		{"history", SimpleCommand{&ConsoleWindow::CommandHistory}},
		{"status", SimpleCommand{&ConsoleWindow::CommandStatus}},
		{"break", SimpleCommand{&ConsoleWindow::CommandBreak}},
		{"fonts", SimpleCommand{&ConsoleWindow::CommandFonts}},

		// Parameterized commands (with arguments)
		{"echo", ParameterizedCommand{&ConsoleWindow::CommandEcho}},
		{"set", ParameterizedCommand{&ConsoleWindow::CommandSet}},
		{"log", ParameterizedCommand{&ConsoleWindow::CommandLog}}};


	// Look up and execute command - O(1) hash lookup with variant visitation
	auto it = commandHandlers.find(command_lower);
	if (it != commandHandlers.end()) {
		std::visit(
			[this, &args](auto&& cmd) {
				using T = std::decay_t<decltype(cmd)>;
				if constexpr (std::is_same_v<T, SimpleCommand>) {
					cmd.handler(this);
				} else if constexpr (std::is_same_v<T, ParameterizedCommand>) {
					cmd.handler(this, args);
				}
			},
			it->second);
	} else {
		AddLog("[error] ❌ Unknown command: '%s'\n", command_name.c_str());
	}

	// On command input, we scroll to bottom even if AutoScroll==false
	ScrollToBottom = true;
}

// Command handler implementations

/**
 * @brief Handler for the 'exit' command.
 *
 * Logs an exit message and terminates the
 * application with EXIT_SUCCESS.
 */
void ConsoleWindow::CommandExit() {
	AddLog("[warning] 👋 Exiting application...\n");
	std::exit(EXIT_SUCCESS);
}

/**
 * @brief Handler for the 'quit' command.
 *
 * Logs a quit message and terminates the
 * application with EXIT_SUCCESS.
 * Functionally equivalent to CommandExit().
 */
void ConsoleWindow::CommandQuit() {
	AddLog("[warning] 👋 Quitting application...\n");
	std::exit(EXIT_SUCCESS);
	// blah blah
}

/**
 * @brief Handler for the 'demo' command.
 *
 * Toggles the ImGui demo window visibility.
 */
void ConsoleWindow::CommandDemo() {
	m_memory->m_bShow_demo_window = !m_memory->m_bShow_demo_window;
	AddLog("[success] 🎪 Demo window %s\n",
		   m_memory->m_bShow_demo_window ? "✅ enabled" : "❌ disabled");
}

/**
 * @brief Handler for the 'show' command.
 *
 * Shows the native console window (if available).

 */
void ConsoleWindow::CommandShowCmd() {
	m_memory->m_bShowCmd = true;
	m_cmd->ShowConsole(true);
	AddLog("[success] 👁️ Console window shown\n");
}

/**
 * @brief Handler for the 'hide' command.
 *
 * Hides the native console window (if available).

 */
void ConsoleWindow::CommandHideCmd() {
	m_memory->m_bShowCmd = false;
	m_cmd->ShowConsole(false);
	AddLog("[success] 🙈 Console window hidden\n");
}

/**
 * @brief Handler for the 'commands' command.
 *
 * Lists all available console commands.
 */
void ConsoleWindow::CommandList() {
	AddLog("[info] 📜 Available commands:\n");
	for (const auto& it : m_MyCommmands) {
		std::string cmd_utf8(it.first.begin(), it.first.end());
		AddLog("[cmd]   ▸ %s\n", cmd_utf8.c_str());
	}
}

/**
 * @brief Handler for the 'clear' and 'cls' commands.
 *
 * Clears all log entries from the
 * console display.
 */
void ConsoleWindow::CommandClear() { ClearLog(); }

/**
 * @brief Handler for the 'help' command.
 *
 * Displays help information by listing all
 * available commands.
 */
void ConsoleWindow::CommandHelp() {
	AddLog("[info] ❓ Available Commands:\n");
	for (const auto& it : m_MyCommmands) {
		std::string cmd_utf8(it.first.begin(), it.first.end());
		AddLog("[cmd]   ▸ %s\n", cmd_utf8.c_str());
	}
}

/**
 * @brief Handler for the 'history' command.
 *
 * Displays the command history (last 10
 * commands or all if fewer).
 */
void ConsoleWindow::CommandHistory() {
	AddLog("[info] 📚 Command History:\n");
	int first = History.Size - 10;
	for (int i = first > 0 ? first : 0; i < History.Size; i++) {
		char hist_utf8[256];
		ImTextStrToUtf8(hist_utf8, sizeof(hist_utf8), History[i], nullptr);
		AddLog("[history] 📌 %3d: %s\n", i, hist_utf8);
	}
}

/**
 * @brief Handler for the 'status' command.
 *
 * Generates and displays a system status
 * report.
 */
void ConsoleWindow::CommandStatus() {
	AddLog("[info] 📊 Generating status report...\n");
	m_cmd->Out.ShowSystemStatus();
}

/**
 * @brief Handler for the 'break' command.
 *
 * Triggers a debugger breakpoint for debugging
 * purposes.
 *
 * @warning Only use this when a debugger is attached.
 */
void ConsoleWindow::CommandBreak() {
	AddLog("[warning] 🔴 Triggering debugger breakpoint...\n");
	__debugbreak();
	AddLog("[info] ▶️ Resumed from breakpoint\n");
}

/**
 * @brief Handler for the 'fonts' command.
 *
 * Lists all loaded fonts from the FontManager.

 */
void ConsoleWindow::CommandFonts() {
	AddLog("[info] 🔤 Available Fonts:\n");

	if (m_font_manager) {
		const auto& fontMap = m_font_manager->GetFontMap();

		if (fontMap.empty()) {
			AddLog("[warning] ⚠️ No fonts loaded\n");
		} else {
			AddLog("[success] ✓ Total fonts loaded: %zu\n", fontMap.size());
			AddLog("[info] ═══════════════════════════════════════\n");

			for (const auto& [fontName, fontPtr] : fontMap) {
				if (fontPtr) { AddLog("[cmd]   ▸ %s\n", fontName.c_str()); }
			}
			AddLog("[info] ═══════════════════════════════════════\n");
		}
	} else {
		AddLog("[error] ❌ FontManager not available\n");
	}
}

// Parameterized command implementations

/**
 * @brief Handler for the 'echo' command.
 *
 * Echoes the provided message to the console.
 *

 * * @param args The message to echo.
 */
void ConsoleWindow::CommandEcho(const std::string& args) {
	if (args.empty()) {
		AddLog("[warning] ⚠️ Usage: echo <message>\n");
		return;
	}
	AddLog("[info] %s\n", args.c_str());
}

/**
 * @brief Handler for the 'set' command.
 *
 * Sets configuration values. Currently supports:
 *
 * - 'autoscroll' (true/false/on/off/1/0)
 * - 'logging' (true/false/on/off/1/0)
 *
 * @param args
 * The key-value pair in format "<key> <value>".
 */
void ConsoleWindow::CommandSet(const std::string& args) {
	if (args.empty()) {
		AddLog("[warning] ⚠️ Usage: set <key> <value>\n");
		return;
	}

	// Parse key and value
	size_t space_pos = args.find_first_of(" \t");
	if (space_pos == std::string::npos) {
		AddLog("[error] ❌ Missing value. Usage: set <key> <value>\n");
		return;
	}

	std::string key	  = args.substr(0, space_pos);
	std::string value = args.substr(space_pos + 1);

	// Trim leading whitespace from value
	size_t val_start = value.find_first_not_of(" \t");
	if (val_start != std::string::npos) { value = value.substr(val_start); }

	AddLog("[success] ✅ Set '%s' = '%s'\n", key.c_str(), value.c_str());

	// Example: Handle specific settings
	if (key == "autoscroll") {
		if (value == "true" || value == "1" || value == "on") {
			AutoScroll = true;
			AddLog("[info] Auto-scroll enabled\n");
		} else if (value == "false" || value == "0" || value == "off") {
			AutoScroll = false;
			AddLog("[info] Auto-scroll disabled\n");
		}
	} else if (key == "logging") {
		if (value == "true" || value == "1" || value == "on") {
			EnableFileLogging(true);
			AddLog("[info] File logging enabled\n");
		} else if (value == "false" || value == "0" || value == "off") {
			EnableFileLogging(false);
			AddLog("[info] File logging disabled\n");
		}
	}
}

/**
 * @brief Handler for the 'log' command.
 *
 * Logs a message with a specified severity level.

 * * Available levels: info, warning, error, success.
 *
 * @param args The log level and message in
 * format "<level> <message>".
 */
void ConsoleWindow::CommandLog(const std::string& args) {
	if (args.empty()) {
		AddLog("[warning] ⚠️ Usage: log <level> <message>\n");
		AddLog("[info] Available levels: info, warning, error, success\n");
		return;
	}

	// Parse log level and message
	size_t space_pos = args.find_first_of(" \t");
	if (space_pos == std::string::npos) {
		// No level specified, default to info
		AddLog("[info] %s\n", args.c_str());
		return;
	}

	std::string level	= args.substr(0, space_pos);
	std::string message = args.substr(space_pos + 1);

	// Trim leading whitespace from message
	size_t msg_start = message.find_first_not_of(" \t");
	if (msg_start != std::string::npos) { message = message.substr(msg_start); }

	// Convert level to lowercase
	std::transform(level.begin(), level.end(), level.begin(), ::tolower);

	// Log with appropriate level
	if (level == "info") {
		AddLog("[info] %s\n", message.c_str());
	} else if (level == "warning" || level == "warn") {
		AddLog("[warning] %s\n", message.c_str());
	} else if (level == "error" || level == "err") {
		AddLog("[error] %s\n", message.c_str());
	} else if (level == "success") {
		AddLog("[success] %s\n", message.c_str());
	} else {
		AddLog("[error] ❌ Unknown log level: '%s'\n", level.c_str());
		AddLog("[info] Available levels: info, warning, error, success\n");
	}
}

/**
 * @brief Adds a formatted log message to the console (UTF-8 version).
 *
 * Converts the UTF-8
 * format string to ImWchar for internal storage and display.
 * If file logging is enabled, also
 * writes the message with a timestamp to the log file.
 *
 * @param fmt The printf-style format
 * string (UTF-8 encoded).
 * @param ... Variable arguments for the format string.
 *
 * @note
 * Supports color tags like [error], [warning], [success], [info], etc.
 */
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

/**
 * @brief Adds a formatted log message to the console (ImWchar version).
 *
 * Processes wide
 * character format strings. Converts to UTF-8 internally for formatting,
 * then back to ImWchar
 * for storage.
 *
 * @param fmt The printf-style format string (ImWchar encoded).
 * @param ...
 * Variable arguments for the format string.
 *
 * @note If file logging is enabled, writes to the
 * log file with a timestamp.
 */
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

/**
 * @brief Adds a formatted log message to the console (Windows wchar_t version).
 *
 *
 * Convenience method for Windows-specific wide character strings.
 * Converts wchar_t to ImWchar
 * for internal storage.
 *
 * @param fmt The wprintf-style format string (Windows wchar_t).
 *
 * @param ... Variable arguments for the format string.
 */
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

/**
 * @brief Displays a checkbox for an ImGui debug log flag.
 *
 * Creates a checkbox to toggle a
 * specific debug log flag. When SHIFT is held,
 * clicking will enable only that flag (exclusive
 * mode).
 *
 * @param name The display name of the debug flag.
 * @param flag The
 * ImGuiDebugLogFlags value to control.
 */
void ConsoleWindow::ShowDebugLogFlag(const char* name, ImGuiDebugLogFlags flag) {
	ImGuiContext& g = *GImGui;
	if (ImGui::CheckboxFlags(name, &g.DebugLogFlags, flag) && g.IO.KeyShift) {
		g.DebugLogFlags = (g.DebugLogFlags & flag) ? flag : 0;
	}
	if (ImGui::IsItemHovered() && g.IO.KeyShift) {
		ImGui::SetTooltip("Hold SHIFT when clicking to only enable this logging category.");
	}
}

/**
 * @brief Updates the console with new entries from ImGui's debug log.
 *
 * Checks for new
 * debug log entries since the last update and adds them to
 * the console with a [DEBUG] tag.
 * Should be called every frame.
 */
void ConsoleWindow::UpdateDebugLog() {
	ImGuiContext& g = *GImGui;

	// Check if there are new debug log entries
	if (g.DebugLogBuf.size() > m_LastDebugLogPos) {
		// Get the new log entries
		const char* log_start = g.DebugLogBuf.c_str() + m_LastDebugLogPos;
		const char* log_end	  = g.DebugLogBuf.c_str() + g.DebugLogBuf.size();

		// Split by lines and add to console
		const char* line_start = log_start;
		for (const char* p = log_start; p < log_end; p++) {
			if (*p == '\n') {
				// Found a complete line
				size_t line_len = p - line_start;
				if (line_len > 0) {
					char   line_buf[1024];
					size_t copy_len =
						(line_len < sizeof(line_buf) - 1) ? line_len : sizeof(line_buf) - 1;
					memcpy(line_buf, line_start, copy_len);
					line_buf[copy_len] = '\0';

					// Add to console with debug tag
					AddLog("[grey][DEBUG] %s\n", line_buf);
				}
				line_start = p + 1;
			}
		}

		// Update position
		m_LastDebugLogPos = g.DebugLogBuf.size();
	}
}

/**
 * @brief Renders the console window using ImGui.
 *
 * Creates and displays the full console
 * interface including:
 * - Title bar with context menu
 * - Control buttons (Clear, Copy,
 * Options)
 * - Filter input
 * - Scrollable log display with color-coded entries
 * - Command
 * input with history and auto-completion
 *
 * @param title The window title to display.
 * @param
 * p_open Pointer to bool controlling window visibility (can be nullptr).
 *
 * @note Uses
 * ImGuiListClipper for efficient rendering of large log histories.
 */
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

		ImGui::Separator();
		ImGui::Text("ImGui Debug Log Flags:");
		ImGui::Spacing();

		ShowDebugLogFlag("Errors", ImGuiDebugLogFlags_EventError);
		ShowDebugLogFlag("ActiveId", ImGuiDebugLogFlags_EventActiveId);
		ShowDebugLogFlag("Clipper", ImGuiDebugLogFlags_EventClipper);
		ShowDebugLogFlag("Focus", ImGuiDebugLogFlags_EventFocus);
		ShowDebugLogFlag("IO", ImGuiDebugLogFlags_EventIO);
		ShowDebugLogFlag("Font", ImGuiDebugLogFlags_EventFont);
		ShowDebugLogFlag("Nav", ImGuiDebugLogFlags_EventNav);
		ShowDebugLogFlag("Popup", ImGuiDebugLogFlags_EventPopup);
		ShowDebugLogFlag("Selection", ImGuiDebugLogFlags_EventSelection);
		ShowDebugLogFlag("InputRouting", ImGuiDebugLogFlags_EventInputRouting);

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

		for (int i = 0; i < Items.Size; i++) {
			const ImWchar* item = Items[i];
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

/**
 * @brief Static callback stub for ImGui text input.
 *
 * Forwards the callback to the instance
 * method.
 *
 * @param data The ImGui input text callback data.
 * @return Result of the instance
 * TextEditCallback method.
 */
int ConsoleWindow::TextEditCallbackStub(ImGuiInputTextCallbackData* data) {
	ConsoleWindow* m_console = (ConsoleWindow*)data->UserData;
	return m_console->TextEditCallback(data);
}

/**
 * @brief Handles text input callbacks for command auto-completion and history.
 *
 *
 * Implements:
 * - Tab completion: Suggests and completes commands based on partial input
 * -
 * History navigation: Up/Down arrows to cycle through command history
 *
 * @param data The ImGui
 * input text callback data containing event information.
 * @return 0 to continue processing,
 * non-zero to stop.
 *
 * @note Tab completion supports multiple matches and shows possibilities.

 * * @note History navigation preserves the current input when returning to it.
 */
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

/**
 * @brief Case-insensitive string comparison (UTF-8).
 *
 * @param s1 First string to compare.

 * * @param s2 Second string to compare.
 * @return 0 if equal, negative if s1 < s2, positive if s1
 * > s2.
 */
int ConsoleWindow::Stricmp(const char* s1, const char* s2) {
	int d;
	while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
		s1++;
		s2++;
	}
	return d;
}

/**
 * @brief Case-insensitive string comparison (ImWchar).
 *
 * @param s1 First wide string to
 * compare.
 * @param s2 Second wide string to compare.
 * @return 0 if equal, negative if s1 < s2,
 * positive if s1 > s2.
 */
int ConsoleWindow::Wcsicmp(const ImWchar* s1, const ImWchar* s2) {
	int d;
	while ((d = towupper(*s2) - towupper(*s1)) == 0 && *s1) {
		s1++;
		s2++;
	}
	return d;
}

/**
 * @brief Case-insensitive string comparison with length limit (UTF-8).
 *
 * @param s1 First
 * string to compare.
 * @param s2 Second string to compare.
 * @param n Maximum number of
 * characters to compare.
 * @return 0 if equal, negative if s1 < s2, positive if s1 > s2.
 */
int ConsoleWindow::Strnicmp(const char* s1, const char* s2, int n) {
	int d = 0;
	while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
		s1++;
		s2++;
		n--;
	}
	return d;
}

/**
 * @brief Case-insensitive string comparison with length limit (ImWchar).
 *
 * @param s1 First
 * wide string to compare.
 * @param s2 Second wide string to compare.
 * @param n Maximum number of
 * characters to compare.
 * @return 0 if equal, negative if s1 < s2, positive if s1 > s2.
 */
int ConsoleWindow::Wcsnicmp(const ImWchar* s1, const ImWchar* s2, int n) {
	int d = 0;
	while (n > 0 && (d = towupper(*s2) - towupper(*s1)) == 0 && *s1) {
		s1++;
		s2++;
		n--;
	}
	return d;
}

/**
 * @brief Trims trailing whitespace from a UTF-8 string.
 *
 * @param s The string to trim
 * (modified in place).
 */
void ConsoleWindow::Strtrim(char* s) {
	char* str_end = s + strlen(s);
	while (str_end > s && str_end[-1] == ' ') str_end--;
	*str_end = 0;
}

/**
 * @brief Trims trailing whitespace from a wide character string.
 *
 * @param s The wide string
 * to trim (modified in place).
 */
void ConsoleWindow::Wcstrim(ImWchar* s) {
	ImWchar* str_end = s;
	while (*str_end) str_end++;
	while (str_end > s && str_end[-1] == ' ') str_end--;
	*str_end = 0;
}

/**
 * @brief Calculates the length of a wide character string.
 *
 * @param s The null-terminated
 * wide string.
 * @return The number of characters (excluding null terminator).
 */
size_t ConsoleWindow::Wcslen(const ImWchar* s) {
	size_t len = 0;
	while (s[len]) len++;
	return len;
}

/**
 * @brief Enables or disables file logging.
 *
 * When enabled, opens the log file in append
 * mode and writes a session start marker.
 * When disabled, writes a session end marker and closes
 * the log file.
 *
 * @param enable True to enable file logging, false to disable.
 *
 * @note Log
 * entries are written with timestamps in the format [YYYY-MM-DD HH:MM:SS.mmm].
 */
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

/**
 * @brief Sets the path for the log file.
 *
 * If logging is currently enabled, closes the
 * current log file and reopens
 * with the new path.
 *
 * @param path The filesystem path for the
 * log file (wide string).
 */
void ConsoleWindow::SetLogFilePath(const std::wstring& path) {
	bool wasEnabled = m_bEnableFileLogging;

	// Close current log file if open
	if (m_logFile.is_open()) { EnableFileLogging(false); }

	m_logFilePath = path;

	// Reopen with new path if it was enabled
	if (wasEnabled) { EnableFileLogging(true); }
}

/**
 * @brief Manually flushes the log file buffer to disk.
 *
 * Ensures all buffered log data is
 * written to the file immediately.
 * Useful before application shutdown or when data persistence
 * is critical.
 */
void ConsoleWindow::FlushLogFile() {
	if (m_logFile.is_open()) { m_logFile.flush(); }
}
} // namespace app
