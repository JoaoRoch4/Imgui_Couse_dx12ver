#include "PCH.hpp"

#include "render.hpp"

WindowClass::WindowClass() : currentPath(fs::current_path()), selectedEntry(fs::path{}) {}

WindowClass::~WindowClass() { currentPath.clear(); }

void WindowClass::Draw(std::string_view label) {

	constexpr static auto window_flags{ImGuiWindowFlags_::ImGuiWindowFlags_NoResize |
									   ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |
									   ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus |
									   ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar};

	constexpr static auto window_size = ImVec2(1920.0F, 1080.0F);
	constexpr static auto window_pos  = ImVec2(0.0F, 0.0F);

	ImGui::SetNextWindowSize(window_size);
	ImGui::SetNextWindowPos(window_pos);

	ImGui::Begin(label.data(), nullptr, window_flags);
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

void WindowClass::DrawMenu() {

	ImGui::Text("Menu");

	if (ImGui::Button("Go up")) {

		if (currentPath.has_parent_path()) { currentPath = currentPath.parent_path(); }
	}

	ImGui::SameLine();

	ImGui::Text("Current directory: %s", currentPath.string().c_str());
}

void WindowClass::DrawContent() {
	ImGui::Text("Content: ");
	ig::Spacing();

	for (const auto& entry : fs::directory_iterator(currentPath)) {

		const bool is_selected	= entry.path() == selectedEntry;
		const bool is_directory = entry.is_directory();
		const bool is_file		= entry.is_regular_file();
		str entry_name			= entry.path().filename().string();

		if (is_directory) entry_name = "[D] " + entry_name;
		else if (is_file) entry_name = "[F] " + entry_name;

		if (ig::Selectable(entry_name.c_str(), is_selected)) {

			if (is_directory) currentPath /= entry.path().filename();

			selectedEntry = entry.path();
		}
		ig::Separator();
	}
}

void WindowClass::DrawActions() {

	ImGui::Text("Actions");

	if (fs::is_directory(selectedEntry))
		ig::Text("Selected dir: %s", selectedEntry.string().c_str());

	else if (fs::is_regular_file(selectedEntry))
		ig::Text("Selected file: %s", selectedEntry.string().c_str());

	else { ig::Text("nothing selectected"); }

	if (fs::is_regular_file(selectedEntry) && ig::Button("Open")) OpenFileWithDefaultEditor();


	if (ig::Button("rename")) ig::OpenPopup("rename");
	ig::SameLine();
	if (ig::Button("delete")) ig::OpenPopup("delete");

	RenameFilePopUp();
	DeleteFilePopUp();
}

void WindowClass::DrawFilters() {

	ImGui::Text("DrawFilters");

	static char extention_filter[16]{"\0"};

	ImGui::Text("Filter By Extention: ");
	ig::SameLine();
	ig::InputText("###inFilter", extention_filter, sizeof(extention_filter));

	if (std::strlen(extention_filter) == 0) return;

	size_t filtered_file_count{};

	for (const auto& entry : fs::directory_iterator(currentPath)) {

		if (!fs::is_regular_file(entry)) continue;

		if (entry.path().extension().string() == extention_filter) ++filtered_file_count;
	}

	ig::Text("Number of files: %u", filtered_file_count);
}

void WindowClass::OpenFileWithDefaultEditor() {}

void WindowClass::RenameFilePopUp() {

	static bool Dialog_open = false;

	if (ig::BeginPopupModal("rename")) {

		static char buffer_name[512]{'\0'};

		ig::Text("New Name: ");

		ig::InputText("###buffer_name", buffer_name, sizeof(buffer_name));

		if (ig::Button("rename")) {

			auto new_path = selectedEntry.parent_path() / buffer_name;

			if (renameFile(selectedEntry, new_path)) {

				ig::EndPopup();
				return;
			}

			ig::EndPopup();
		}
	}
}

void WindowClass::DeleteFilePopUp() {

	if (ig::BeginPopupModal("delete")) { ig::EndPopup(); }
}

bool WindowClass::renameFile(const fs::path& old_Path, const fs::path& new_Path) {
	try {

		fs::rename(old_Path, new_Path);
		return true;
	} catch (std::runtime_error& e) {

		std::cerr << e.what() << std::endl;
		return false;
	}
}

bool WindowClass::deleteFile(const fs::path& Path) {
	try {

		fs::remove(Path);
		return true;
	} catch (std::runtime_error& e) {

		std::cerr << e.what() << std::endl;
		return false;
	}
}


void render(WindowClass& window_obj) { window_obj.Draw("Label"); }
