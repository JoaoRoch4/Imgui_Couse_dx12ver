#pragma once

#include "PCH.hpp"

class Master {

public:

    virtual void Open() = 0;
    virtual void Tick() = 0;
    virtual void Close() = 0;

};
