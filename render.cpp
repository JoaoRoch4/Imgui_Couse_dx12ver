#include "PCH.hpp"

#include "render.hpp"

WindowClass::WindowClass() : currentPath(fs::current_path()), selectedEntry(fs::path{}) {
}

WindowClass::~WindowClass() {

	currentPath.clear();
}

void WindowClass::Draw(std::string_view label) {

	constexpr static auto window_flags{ImGuiWindowFlags_::ImGuiWindowFlags_NoResize |
									   ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |
									   ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus |
									   ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar};

	constexpr static auto window_size = ImVec2(1920.0F, 1080.0F);
	constexpr static auto window_pos = ImVec2(0.0F, 0.0F);

	ImGui::SetNextWindowSize(window_size);
	ImGui::SetNextWindowPos(window_pos);

	ImGui::Begin(label.data(), nullptr, window_flags);
	{
		ig::Spacing();
		ig::Separator();
		DrawMenu();
		ig::Spacing();
		ig::Separator();
		DrawContent();
		ig::Spacing();
		ig::Separator();
		DrawActions();
		ig::Spacing();
		ig::Separator();
		DrawFilters();
		ig::Spacing();
		ig::Separator();
	}
	ImGui::End();
}

void WindowClass::DrawMenu() {

	ImGui::Text("Menu");

	if (ImGui::Button("Go up")) {

		if (currentPath.has_parent_path()) {
			currentPath = currentPath.parent_path();
		}
	}

	ImGui::SameLine();

	ImGui::Text("Current directory: %s", currentPath.string().c_str());
}

void WindowClass::DrawContent() {
	ig::Separator();
	ImGui::Text("Content: ");
	ig::Spacing();

	for (const auto &entry : fs::directory_iterator(currentPath)) {

		const bool is_selected = entry.path() == selectedEntry;
		const bool is_directory = entry.is_directory();
		const bool is_file = entry.is_regular_file();
		str entry_name = entry.path().filename().string();

		if (is_directory)
			entry_name = "[D] " + entry_name;
		else if (is_file)
			entry_name = "[F] " + entry_name;

		if (ig::Selectable(entry_name.c_str(), is_selected)) {

			if (is_directory)
				currentPath /= entry.path().filename();

			selectedEntry = entry.path();
		}
		ig::Separator();
	}
}

void WindowClass::DrawActions() {
	ImGui::Text("Actions");
}

void WindowClass::DrawFilters() {
	ImGui::Text("Filters");
}

void render(WindowClass &window_obj) {
	window_obj.Draw("Label");
}
