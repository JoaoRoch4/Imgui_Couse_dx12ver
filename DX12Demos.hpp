#pragma once

#include "PCH.hpp"

class DxDemos {

public:

     DxDemos(ImGuiIO* io);
     virtual ~DxDemos();

private:

    DxDemos();

    ImGuiIO* m_io;
};
