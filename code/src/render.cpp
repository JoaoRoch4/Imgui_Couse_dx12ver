/**
 * @file render.cpp
 * @brief Implementation of the WindowClass for the ImGui File Explorer.
 *
 * This file contains the complete implementation of the WindowClass which provides
 * a graphical file system browser interface using Dear ImGui. It supports directory
 * navigation, file selection, renaming, deletion, and filtering operations.
 *
 * @author JoaoRocha
 * @date 2024
 * @see WindowClass
 * @see render.hpp
 */

#include "PCH.hpp"
#include "render.hpp"
#include "Classes.hpp"
#include "MemoryManagement.hpp"

namespace app {

/**
 * @brief Opens the m_window.
 * 
 * This function is called when the m_window is first opened.
 * Currently a placeholder
 * for initialization logic that should occur when the m_window becomes
 * visible.
 */
void WindowClass::Open() {}

/**
 * @brief Tick function called each frame.
 * 
 * This function is called once per frame and
 * delegates to the Draw function
 * to render the m_window content.
 * 
 * @see Draw()
 */
void WindowClass::Tick() { Draw("Label"); }

/**
 * @brief Closes the m_window.
 * 
 * This function is called when the m_window is closed. Currently
 * a placeholder
 * for cleanup logic that should occur when the m_window is hidden or destroyed.
 */
void WindowClass::Close() {}

/**
 * @brief Construct a new Window Class object.
 *
 * Initializes all member variables to their default states:
 * - m_currentPath is set to the application's current working directory
 * - m_selectedEntry is set to an empty path
 * - m_renameDialogOpen and m_deleteDialogOpen are set to false
 * - m_memory is initialized with a reference to the MemoryManagement singleton
 *
 * @see MemoryManagement::Get_MemoryManagement()
 */
WindowClass::WindowClass() :
m_currentPath(fs::current_path()),
m_selectedEntry(fs::path{}),
m_memory(nullptr),
m_renameDialogOpen(false),
m_deleteDialogOpen(false) {
	m_memory = MemoryManagement::Get_MemoryManagement();
}

/**
 * @brief Destroy the Window Class object.
 *
 * Performs cleanup by:
 * - Clearing the m_currentPath filesystem path
 * - Clearing the m_selectedEntry filesystem path
 * - Resetting the m_memory pointer to nullptr
 *
 * @note This destructor does not delete the MemoryManagement instance as it is
 *       a singleton managed elsewhere in the application.
 */
WindowClass::~WindowClass() {
	m_currentPath.clear();
	m_selectedEntry.clear();
	m_memory = nullptr;
}

/**
 * @brief Main rendering loop for the m_window.
 *
 * Orchestrates the drawing of all UI components in the following order:
 * 1. Menu bar with navigation controls
 * 2. Directory content listing
 * 3. Action buttons for file operations
 * 4. File extension filters
 *
 * The m_window is created with horizontal scrollbar and auto-resize flags, and
 * is set to a default size of 1000x800 pixels.
 *
 * @param label The m_window title displayed in the ImGui title bar.
 *
 * @note The m_window visibility is controlled by m_memory->m_bShow_FileSys_window
 * @see DrawMenu(), DrawContent(), DrawActions(), DrawFilters()
 */
void WindowClass::Draw(std::string_view label) {
	constexpr static auto window_flags{
		ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysHorizontalScrollbar |
		ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize};

	ig::SetNextWindowSize(ImVec2(1000.f, 800.f));




	ImGui::Begin(label.data(), &m_memory->m_bShow_FileSys_window, window_flags);
	{
		DrawMenu();
		ig::Separator();
		DrawContent();
		ig::Separator();
		DrawActions();
		ig::Spacing();
		ig::Separator();
		DrawFilters();
	}
	ImGui::End();
}

/**
 * @brief Renders the navigation menu.
 *
 * Displays:
 * - A "Go up" button to navigate to the parent directory (disabled if at root)
 * - The current directory path as text
 *
 * @note The "Go up" button only functions when the current path has a parent directory.
 * @see DrawContent()
 */
void WindowClass::DrawMenu() {
	ImGui::Text("Menu");


	if (ImGui::Button("Go up")) {
		if (m_currentPath.has_parent_path()) { m_currentPath = m_currentPath.parent_path(); }
	}

	ImGui::SameLine();
	ImGui::Text("Current directory: %s", m_currentPath.string().c_str());
}

/**
 * @brief Renders the directory listing.
 *
 * Iterates through the current directory and displays all entries as selectable items.
 * Each entry is prefixed with:
 * - [D] for directories
 * - [F] for regular files
 *
 * User interactions:
 * - Clicking on a directory navigates into it by updating m_currentPath
 * - Clicking on any entry (file or directory) selects it and updates m_selectedEntry
 *
 * @note Directory navigation occurs immediately upon selection.
 * @note Each entry is separated by an ImGui separator line.
 * @see m_currentPath, m_selectedEntry
 */
void WindowClass::DrawContent() {
	ImGui::Text("Content: ");
	ig::Spacing();

	for (const auto& entry : fs::directory_iterator(m_currentPath)) {
		const bool is_selected	= entry.path() == m_selectedEntry;
		const bool is_directory = entry.is_directory();
		const bool is_file		= entry.is_regular_file();
		str		   entry_name	= entry.path().filename().string();

		if (is_directory) entry_name = "[D] " + entry_name;
		else if (is_file) entry_name = "[F] " + entry_name;

		if (ig::Selectable(entry_name.c_str(), is_selected)) {
			if (is_directory) m_currentPath /= entry.path().filename();
			m_selectedEntry = entry.path();
		}
		ig::Separator();
	}
}

/**
 * @brief Displays available operations for the selected filesystem entry.
 *
 * Shows:
 * - The type and path of the currently selected entry
 * - "Open" button (only for regular files) - opens file with system default editor
 * - "rename" button - opens the rename dialog popup
 * - "delete" button - opens the delete confirmation dialog popup
 *
 * @note The "Open" button is only displayed for regular files, not directories.
 * @see OpenFileWithDefaultEditor(), RenameFilePopUp(), DeleteFilePopUp()
 */
void WindowClass::DrawActions() {
	ImGui::Text("Actions");

	if (fs::is_directory(m_selectedEntry))
		ig::Text("Selected dir: %s", m_selectedEntry.string().c_str());
	else if (fs::is_regular_file(m_selectedEntry))
		ig::Text("Selected file: %s", m_selectedEntry.string().c_str());
	else { ig::Text("nothing selectected"); }

	if (fs::is_regular_file(m_selectedEntry) && ig::Button("Open")) OpenFileWithDefaultEditor();

	if (ig::Button("rename")) {
		m_renameDialogOpen = true;

		ig::OpenPopup("Rename File");
	}
	ig::SameLine();
	if (ig::Button("delete")) {
		m_deleteDialogOpen = true;
		ig::OpenPopup("Delete File");
	}

	RenameFilePopUp();
	DeleteFilePopUp();
}

/**
 * @brief Renders the file extension filter UI.
 *
 * Provides:
 * - An input text field (max 16 characters) for entering a file extension filter
 * - A count display showing the number of files matching the entered extension
 *
 * The filter compares the entered string against the complete file extension
 * (including the dot, e.g., ".txt", ".cpp"). The count is updated in real-time
 * and only includes regular files, not directories.
 *
 * @note The filter uses exact string matching, not pattern matching.
 * @note If the filter field is empty, no count is displayed.
 */
void WindowClass::DrawFilters() {
	ImGui::Text("DrawFilters");
	static char extention_filter[16]{"\0"};

	ImGui::Text("Filter By Extention: ");
	ig::SameLine();
	ig::InputText("###inFilter", extention_filter, sizeof(extention_filter));

	if (std::strlen(extention_filter) == 0) return;

	size_t filtered_file_count{};
	for (const auto& entry : fs::directory_iterator(m_currentPath)) {
		if (!fs::is_regular_file(entry)) continue;
		if (entry.path().extension().string() == extention_filter) ++filtered_file_count;
	}

	ig::Text("Number of files: %u", filtered_file_count);
}

/**
 * @brief Opens the selected file with the system's default editor/application.
 *
 * Uses platform-specific commands to open the file:
 * - Windows: Uses the "start" command via std::system()
 * - Other platforms: Currently not implemented, throws std::runtime_error
 *
 * @throws std::runtime_error on non-Windows platforms
 *
 * @note This function uses std::system() which may have security implications.
 * @warning The file path should be properly sanitized before use in system commands.
 * @see m_selectedEntry
 */
void WindowClass::OpenFileWithDefaultEditor() {

#ifdef WIN32

	const auto command = "start \"\" \"" + m_selectedEntry.string() + "\"";

	std::system(command.c_str());

#elifndef WIN32

	throw std::runtime_error("not implemented!");

#endif
}

/**
 * @brief Renders the 'Rename' modal popup.
 *
 * Displays a modal dialog with:
 * - An input text field (max 512 characters) for entering the new name
 * - A "Rename File" button to confirm the operation
 * - A "Cancel" button to close the dialog without changes
 *
 * On successful rename:
 * - The dialog is closed
 * - m_selectedEntry is updated to the new path
 * - The input buffer is cleared
 *
 * @note The new name should not include a path; it creates the new file in the same directory.
 * @note Uses a static buffer that persists across function calls to maintain input state.
 * @see renameFile(), m_renameDialogOpen, m_selectedEntry
 */
void WindowClass::RenameFilePopUp() {

	static bool Dialog_open = false;

	if (ig::BeginPopupModal("Rename File", &m_renameDialogOpen)) {
		static char buffer_name[512]{'\0'};
		ig::Text("New Name: ");
		ig::InputText("###newName", buffer_name, sizeof(buffer_name));

		if (ig::Button("Rename File")) {
			auto new_path = m_selectedEntry.parent_path() / buffer_name;
			if (renameFile(m_selectedEntry, new_path)) {
				m_renameDialogOpen = false;
				m_selectedEntry	   = new_path;
				std::memset(buffer_name, 0, sizeof(buffer_name));
			}
		}
		ig::SameLine();
		{
			if (ig::Button("Cancel")) m_renameDialogOpen = false;
		}

		ig::EndPopup();
	}
}

/**
 * @brief Renders the 'Delete' modal popup.
 *
 * Displays a modal confirmation dialog with:
 * - A confirmation message showing the path of the file/directory to be deleted
 * - A "Yes" button to confirm deletion
 * - A "No" button to cancel the operation
 *
 * On successful deletion:
 * - m_selectedEntry is cleared
 * - The dialog is closed
 *
 * @note This can delete both files and directories.
 * @see deleteFile(), m_deleteDialogOpen, m_selectedEntry
 */
void WindowClass::DeleteFilePopUp() {
	if (ig::BeginPopupModal("Delete File", &m_deleteDialogOpen)) {
		ig::Text("Are you sure that you want to delete %s?", m_selectedEntry.string().c_str());
		{
			if (ig::Button("Yes")) {
				if (deleteFile(m_selectedEntry)) m_selectedEntry.clear();
				m_deleteDialogOpen = false;
			}
			ig::SameLine();
			if (ig::Button("No")) m_deleteDialogOpen = false;
		}
		ig::EndPopup();
	}
}

/**
 * @brief Wraps std::filesystem::rename with HRESULT error handling and UI feedback.
 *
 * Attempts to rename or move a file or directory from old_Path to new_Path.
 * If an error occurs, the exception is caught, a MessageBox is displayed,
 * and an appropriate HRESULT is returned.
 *
 * @param old_Path The original filesystem path to rename/move.
 * @param new_Path The target filesystem path.
 * @return S_OK on success, or a failure HRESULT (e.g., E_ACCESSDENIED) if an exception occurs.
 *
 * @note This function can rename or move files and directories.
 * @warning If new_Path already exists, it may be overwritten depending on the OS and file type.
 * @see [std::filesystem::rename](en.cppreference.com)
 */
HRESULT WindowClass::renameFile(const fs::path& old_Path, const fs::path& new_Path) {
    try {
        fs::rename(old_Path, new_Path);
        return S_OK;
    } catch(const fs::filesystem_error& e) {

    Conv conv;
        // Constrói a mensagem de erro para o usuário
        std::wstring errorDetails = L"Source: " + old_Path.wstring() +
            L"\nDestination: " + new_Path.wstring() +
            L"\n\nError: " + conv.strtoWstr(e.what());

        // Exibe o erro visualmente
        MessageBox(NULL, errorDetails.c_str(), L"Rename Error", MB_ICONERROR | MB_OK);

        // Retorna o código de erro real do sistema convertido para HRESULT
        return HRESULT_FROM_WIN32(e.code().value());
    } catch(const std::exception& e) {
        Conv conv;
        MessageBox(NULL, conv.strtoWstr(e.what()).c_str(), L"General Error", MB_ICONERROR | MB_OK);
        return E_FAIL;
    }
}
/**
 * @brief Wraps std::filesystem::remove with HRESULT error handling and UI feedback.
 *
 * Attempts to delete a file at the specified path. If it fails, a Windows
 * MessageBox is displayed to the user with the error details.
 *
 * @param Path The filesystem path to the file or directory to delete.
 * @return S_OK on success, or a failure HRESULT if an exception occurs.
 */
HRESULT WindowClass::deleteFile(const fs::path& Path) {
	try {
		fs::remove(Path);
		return S_OK;
	} catch (const fs::filesystem_error& e) {

		std::wstring errorMsg =
			L"Failed to delete file:\n" + Path.wstring() + L"\n\nError: " + ConvStatic::strtoWstr(e.what());

		MessageBox(NULL, errorMsg.c_str(), L"File System Error", MB_ICONERROR | MB_OK);

		return HRESULT_FROM_WIN32(e.code().value());
	} catch (const std::exception& e) {

		MessageBox(NULL, ConvStatic::strtoWstr(e.what()).c_str(), L"Critical Error", MB_ICONERROR | MB_OK);
		return E_FAIL;
	}
}


} // namespace app
