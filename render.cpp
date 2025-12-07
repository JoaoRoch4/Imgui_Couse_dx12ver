#include "PCH.hpp"

#include "render.hpp"

WindowClass::WindowClass() : currentPath(fs::current_path()) { }

WindowClass::~WindowClass() {

    currentPath.clear();
}

void WindowClass::Draw(std::string_view label) {

    constexpr static auto window_flags{
        ImGuiWindowFlags_::ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar};

    constexpr static auto window_size = ImVec2(1920.0F, 1080.0F);
    constexpr static auto window_pos = ImVec2(0.0F, 0.0F);

    ImGui::SetNextWindowSize(window_size);
    ImGui::SetNextWindowPos(window_pos);

    ImGui::Begin(label.data(), nullptr, window_flags);
    {

       DrawMenu();
        DrawContent();
        DrawActions();
        DrawFilters();
       
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

void WindowClass::DrawContent() { ImGui::Text("Content"); }

void WindowClass::DrawActions() { ImGui::Text("Actions"); }

void WindowClass::DrawFilters()
{
    ImGui::Text("Filters");
}

void render(WindowClass &window_obj)
{
    window_obj.Draw("Label");
}
