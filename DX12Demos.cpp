#include "PCH.hpp"
#include "Classes.hpp"
#include "DX12Demos.hpp"

DxDemos::DxDemos() : m_io(nullptr) {}

DxDemos::DxDemos(ImGuiIO* io) :DxDemos()  {

    if (io) m_io = io;
	else throw std::runtime_error("io is nullptr");
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
