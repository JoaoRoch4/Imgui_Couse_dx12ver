#pragma once

#include "PCH.hpp"

class WindowClass {

public:
	WindowClass();
	~WindowClass();

	void Draw(std::string_view label);

protected:
	void DrawMenu();
	void DrawContent();
	void DrawActions();
	void DrawFilters();

private:
	fs::path currentPath;
	fs::path selectedEntry;
};

void render(WindowClass &window_obj);
