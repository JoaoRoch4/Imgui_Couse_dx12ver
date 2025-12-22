/**
 * @file WindowClass.cpp
 * @brief Implementation of the WindowClass for the ImGui File Explorer.
 */

#include "PCH.hpp"
#include "render.hpp"
#include "Classes.hpp"
#include "MemoryManagement.hpp"

namespace app {
    void WindowClass::Open() {
        memory = MemoryManagement::Get_MemoryManagement();
    }
    void WindowClass::Tick() {
        Draw("Label");
    }
    void WindowClass::Close() {}
    /**
     * @brief Construct a new Window Class object.
     * Initializes currentPath to the application's current working directory.
     */
    WindowClass::WindowClass() : currentPath(fs::current_path()), selectedEntry(fs::path{}), memory(nullptr){
    }

    /**
     * @brief Destroy the Window Class object.
     * Clears the current path string/buffer.
     */
    WindowClass::~WindowClass() { currentPath.clear(); }

    /**
     * @brief Main rendering loop for the window.
     *
     * Orchestrates the drawing of the menu, actions, filters, and directory content.
     * @param label The window title displayed in the ImGui title bar.
     */
    void WindowClass::Draw(std::string_view label) {
        constexpr static auto window_flags{ ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar };

        ImGui::Begin(label.data(), &memory->m_bShow_FileSys_window, window_flags);
        {
            ig::Spacing();
            ig::Separator();
            DrawMenu();
            ig::Spacing();
            ig::Separator();

            DrawActions();
            ig::Spacing();
            ig::Separator();
            DrawFilters();
            ig::Spacing();
            ig::Separator();
            DrawContent();

            ig::Spacing();
            ig::Separator();
        }
        ImGui::End();
    }

    /**
     * @brief Renders the navigation menu.
     * Displays the current path and provides a "Go up" button to navigate to the parent directory.
     */
    void WindowClass::DrawMenu() {
        ImGui::Text("Menu");

        if(ImGui::Button("Go up")) {
            if(currentPath.has_parent_path()) { currentPath = currentPath.parent_path(); }
        }

        ImGui::SameLine();
        ImGui::Text("Current directory: %s", currentPath.string().c_str());
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

        for(const auto& entry : fs::directory_iterator(currentPath)) {
            const bool is_selected = entry.path() == selectedEntry;
            const bool is_directory = entry.is_directory();
            const bool is_file = entry.is_regular_file();
            str		   entry_name = entry.path().filename().string();

            if(is_directory) entry_name = "[D] " + entry_name;
            else if(is_file) entry_name = "[F] " + entry_name;

            if(ig::Selectable(entry_name.c_str(), is_selected)) {
                if(is_directory) currentPath /= entry.path().filename();
                selectedEntry = entry.path();
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

        if(fs::is_directory(selectedEntry))
            ig::Text("Selected dir: %s", selectedEntry.string().c_str());
        else if(fs::is_regular_file(selectedEntry))
            ig::Text("Selected file: %s", selectedEntry.string().c_str());
        else { ig::Text("nothing selectected"); }

        if(fs::is_regular_file(selectedEntry) && ig::Button("Open")) OpenFileWithDefaultEditor();

        if(ig::Button("rename")) ig::OpenPopup("rename");
        ig::SameLine();
        if(ig::Button("delete")) ig::OpenPopup("delete");

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
        static char extention_filter[16]{ "\0" };

        ImGui::Text("Filter By Extention: ");
        ig::SameLine();
        ig::InputText("###inFilter", extention_filter, sizeof(extention_filter));

        if(std::strlen(extention_filter) == 0) return;

        size_t filtered_file_count{};
        for(const auto& entry : fs::directory_iterator(currentPath)) {
            if(!fs::is_regular_file(entry)) continue;
            if(entry.path().extension().string() == extention_filter) ++filtered_file_count;
        }

        ig::Text("Number of files: %u", filtered_file_count);
    }

    /**
     * @brief Placeholder for opening a file with the system's default handler.
     */
    void WindowClass::OpenFileWithDefaultEditor() {}

    /**
     * @brief Renders the 'Rename' modal popup.
     * Provides an input buffer for the new name and calls renameFile upon confirmation.
     */
    void WindowClass::RenameFilePopUp() {

        static bool Dialog_open = false;

        if(ig::BeginPopupModal("rename")) {
            static char buffer_name[512]{ '\0' };
            ig::Text("New Name: ");
            ig::InputText("###newName", buffer_name, sizeof(buffer_name));

            if(ig::Button("rename")) {
                auto new_path = selectedEntry.parent_path() / buffer_name;
                if(renameFile(selectedEntry, new_path)) {
                    ig::EndPopup();
                    return;
                }
            }
            ig::EndPopup();

        }
    }

    /**
     * @brief Renders the 'Delete' modal popup.
     */
    void WindowClass::DeleteFilePopUp() {
        if(ig::BeginPopupModal("delete")) { ig::EndPopup(); }
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
        } catch(std::exception& e) {
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
        } catch(std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
    }

  

} // namespace app
