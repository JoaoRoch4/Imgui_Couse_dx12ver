#include "PCH.hpp"
#include "Classes.hpp"
#include "DX12Demos.hpp"

DxDemos::DxDemos() : m_io(nullptr) {}

DxDemos::DxDemos(ImGuiIO* m_io) :DxDemos()  {

    if (m_io) m_io = m_io;
	else throw std::runtime_error("m_io is nullptr");
}

DxDemos::~DxDemos() {}

void DxDemos::Render() {

    ImGui::Begin("DX12 demos Window!");
    {

    }
    ImGui::End();
}

void DxDemos::Open() {
    Render();
}

void DxDemos::Tick() {
    Render();
}

void DxDemos::Close() {}
