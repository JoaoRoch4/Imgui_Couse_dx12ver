#pragma once

// FontManagerWindow.cpp - Complete example of ImGui with file/folder dialogs
// This shows how to create buttons that open Windows file dialogs

#include "PCH.hpp"
#include "FontManager.hpp"

// ============================================================================
// IMGUI DIALOG EXAMPLE CLASS
// ============================================================================

// This class manages the ImGui UI with file/folder dialog buttons
class FontManagerWindow {
private:
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

public:
    // Constructor - initializes all member variables
    // @param fontManager: Pointer to FontManager instance
    // @param hwnd: Handle to the parent window
    FontManagerWindow(FontManager* fontManager, HWND hwnd) 
        : m_fontManager(fontManager),           // Store font manager pointer
          m_selectedFilePath(""),               // Initialize empty file path
          m_selectedFolderPath(""),             // Initialize empty folder path
          m_statusMessage("Ready"),             // Initial status message
          m_hwnd(hwnd),                         // Store window handle
          m_fontsLoaded(false),                 // No fonts loaded yet
          m_totalFontsLoaded(0)                 // Zero fonts loaded
    {}
    
    // Destructor
    ~FontManagerWindow() {}
    
    // Main render function - called every frame to draw the UI
    void Render();
    // Method to display a font preview window (bonus feature)
    void RenderFontPreview();
};

// ============================================================================
// USAGE EXAMPLE IN MAIN LOOP
// ============================================================================

// Example of how to use this in your main application loop
void ExampleUsageInMainLoop(HWND hwnd);
