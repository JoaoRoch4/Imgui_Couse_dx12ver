#pragma once

// FontManagerWindow.cpp - Complete example of ImGui with file/folder dialogs
// This shows how to create buttons that open Windows file dialogs

#include "PCH.hpp"
#include "FontManager.hpp"
#include "MemoryManagement.hpp"

namespace app {

// ============================================================================
// IMGUI DIALOG EXAMPLE CLASS
// ============================================================================

// This class manages the ImGui UI with file/folder dialog buttons
class FontManagerWindow : public MemoryManagement{


	// FontManager instance for managing fonts
	FontManager* m_fontManager;

	// String to store the selected file path
	std::string m_selectedFilePath;

	// String to store the selected folder path
	std::string m_selectedFolderPath;

	// String to store status messages
	std::string m_statusMessage;

	// Handle to the main window (needed for dialogs to be modal)
	HWND m_hwnd;

	// Flag to track if fonts are currently loaded
	bool m_fontsLoaded;

	// Counter for total fonts loaded
	int m_totalFontsLoaded;

    MemoryManagement* m_memory;


    // Main render function - called every frame to draw the UI
    void Render();

public:
	FontManagerWindow();

	void GetAux(HWND hwnd, FontManager* fontManager);
	void GetFontManager(FontManager* fontManager);
	void GetHwnd(HWND hwnd);

    virtual void Open() override;
    virtual void Tick() override;
    virtual void Close() override;

	// Destructor
	~FontManagerWindow();

	// Method to display a font preview window (bonus feature)
	void RenderFontPreview();

	bool bShowFontPreview;

	bool bFonstsWereLoaded;
};

//void ExampleUsageInMainLoop(HWND hwnd);

} // namespace app
