#pragma once

#include "PCH.hpp"

// main namespace for the app
namespace app {

class Master {

public:
	virtual void Open()	 = 0;
	virtual void Tick()	 = 0;
	virtual void Close() = 0;
};
} // namespace app
