#pragma once

#include "PCH.hpp"
#include "MemoryManagement.hpp"


namespace app {
class WindowClass : public MemoryManagement {

public:
	virtual void Open() override;
	virtual void Tick() override;
	virtual void Close() override;

	WindowClass();
	~WindowClass();

	void Draw(std::string_view label);

protected:
	void DrawMenu();
	void DrawContent();
	void DrawActions();
	void DrawFilters();
	void OpenFileWithDefaultEditor();
	void RenameFilePopUp();
	void DeleteFilePopUp();
	bool renameFile(const fs::path& old_Path, const fs::path& new_Path);
	bool deleteFile(const fs::path& Path);

private:
	fs::path		  currentPath;
	fs::path		  selectedEntry;
	MemoryManagement* memory;
};


} // namespace app
