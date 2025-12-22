#pragma once

#include "PCH.hpp"
#include "Master.hpp"

namespace app {

class DxDemos : public Master{

public:

     DxDemos(ImGuiIO* m_io);
     virtual ~DxDemos();


     virtual void Open()  override;
     virtual void Tick()  override;
     virtual void Close() override;

private:

    DxDemos();
         void Render();


    ImGuiIO* m_io;
};
}
