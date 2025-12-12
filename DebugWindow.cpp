#include "PCH.hpp"

#include "DebugWindow.hpp"


DebugWindow::DebugWindow() : m_io(nullptr) {}

DebugWindow::DebugWindow(ImGuiIO* io) : DebugWindow() {

    if(io)
        m_io = io;
    else
        throw std::runtime_error("io is nullptr");
}

DebugWindow::~DebugWindow() {}

void DebugWindow::Render() {

    ImGui::Begin("Debug Window!"); // Create a window called "Hello, world!"
    // and append into it.

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
        1000.0f / m_io->Framerate,
        m_io->Framerate);

    if(ImGui::Button("Debug Break"))
        __debugbreak();

    ImGui::End();
}
