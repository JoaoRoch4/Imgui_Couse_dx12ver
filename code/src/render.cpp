/**
 * @file WindowClass.cpp
 * @brief Implementation of the WindowClass for the ImGui File Explorer.
 */

#include "PCH.hpp"
#include "render.hpp"
#include "Classes.hpp"
#include "MemoryManagement.hpp"

namespace app {

void WindowClass::Open() {}
void WindowClass::Tick() { Draw("Label"); }
void WindowClass::Close() {}

/**
 * @brief Construct a new Window Class object.
 * Initializes m_currentPath to the application's current working directory.
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
 * Clears the current path string/buffer.
 */
WindowClass::~WindowClass() {
	m_currentPath.clear();
	m_selectedEntry.clear();
	m_memory = nullptr;
}

/**
 * @brief Main rendering loop for the window.
 *
 * Orchestrates the drawing of the menu, actions, filters, and directory content.
 * @param label The window title displayed in the ImGui title bar.
 */
void WindowClass::Draw(std::string_view label) {
	constexpr static auto window_flags{ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysHorizontalScrollbar
   | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize};

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
 * Displays the current path and provides a "Go up" button to navigate to the parent directory.
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
 * Iterates through the current directory, differentiates between files [F] and
 * directories [D], and handles selection and folder entry logic.
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
 * Shows buttons for Rename, Delete, and Open (if the entry is a file).
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
 * Provides an input text field to filter files by extension and displays
 * the count of matching files in the current directory.
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
 * @brief Placeholder for opening a file with the system's default handler.
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
 * Provides an input buffer for the new name and calls renameFile upon confirmation.
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
 * @brief Wraps std::filesystem::rename with error handling.
 * @param old_Path Original path.
 * @param new_Path Target path.
 * @return true if successful, false otherwise.
 */
bool WindowClass::renameFile(const fs::path& old_Path, const fs::path& new_Path) {
	try {
		fs::rename(old_Path, new_Path);
		return true;
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Wraps std::filesystem::remove with error handling.
 * @param Path Path to the entry to be deleted.
 * @return true if successful, false otherwise.
 */
bool WindowClass::deleteFile(const fs::path& Path) {
	try {
		fs::remove(Path);
		return true;
	} catch (std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
}


} // namespace app
