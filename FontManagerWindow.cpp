#include "PCH.hpp"
#include "FontManagerWindow.hpp"

FontManagerWindow::FontManagerWindow()
    : m_fontManager(nullptr),           // Store font manager pointer
    m_selectedFilePath(""),               // Initialize empty file path
    m_selectedFolderPath(""),             // Initialize empty folder path
    m_statusMessage("Ready"),             // Initial status message
    m_hwnd(nullptr),                         // Store window handle
    m_fontsLoaded(false),                 // No fonts loaded yet
    m_totalFontsLoaded(0),                // Zero fonts loaded+
    bShowFontPreview(false),
bFonstsWereLoaded(false){}

FontManagerWindow::FontManagerWindow(FontManager* fontManager, HWND hwnd) : FontManagerWindow() {
    if(fontManager)
        m_fontManager = fontManager;
    else throw std::runtime_error("fontManager is nullptr");

    if(hwnd)
        m_hwnd = hwnd;
    else throw std::runtime_error("hwnd is nullptr");

}
FontManagerWindow::~FontManagerWindow() {}


void FontManagerWindow::Render() {
    // Begin a new ImGui window
    // "Font Manager Demo" is the window title
    // The window is resizable and can be moved
    ImGui::Begin("Font Manager Demo");

    ImGui::Checkbox("Show font preview", &bShowFontPreview);

    ImGui::ShowFontSelector("FontManager");

    if(bShowFontPreview) RenderFontPreview();

    // Display a text header
    ImGui::Text("Font File and Folder Selection");

    // Add a horizontal separator line for visual separation
    ImGui::Separator();

    // ====================================================================
    // SECTION 1: SINGLE FONT FILE SELECTION
    // ====================================================================

    // Display a sub-header for this section
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Select Single Font File:");

    // Create a button labeled "Browse for Font File..."
    // When clicked, the button returns true
    if(ImGui::Button("Browse for Font File...")) {
        // Button was clicked - open the file dialog
        // Call FontManager's method to show Windows file picker
        std::string selectedFile = m_fontManager->OpenFontFileDialog(m_hwnd);

        // Check if user selected a file (not cancelled)
        if(!selectedFile.empty()) {
            // User selected a file - store it
            m_selectedFilePath = selectedFile;

            // Update status message to show what was selected
            m_statusMessage = "File selected: " + selectedFile;
        } else {
            // User cancelled the dialog
            m_statusMessage = "File selection cancelled";
        }
    }

    // Display the currently selected file path (if any)
    // This shows on the same line as the button
    ImGui::SameLine();  // Put next item on same line
    ImGui::Text("Selected: %s",
        m_selectedFilePath.empty() ? "None" : m_selectedFilePath.c_str());

    // Add a button to load the selected font file
    // Only enabled if a file is selected (not empty)
    if(!m_selectedFilePath.empty()) {
        // File is selected - show Load button
        if(ImGui::Button("Load This Font")) {
            // Try to load the selected font
            ImFont* loadedFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(
                m_selectedFilePath.c_str());

            // Check if font loaded successfully
            if(loadedFont != nullptr) {
                m_statusMessage = "Successfully loaded font!";
                m_totalFontsLoaded++;
            } else {
                m_statusMessage = "ERROR: Failed to load font!";
            }
        }
    }

    // Add some vertical spacing between sections
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ====================================================================
    // SECTION 2: FOLDER SELECTION AND BATCH LOADING
    // ====================================================================

    // Display sub-header for folder section
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Select Folder Containing Fonts:");

    // Create a button to browse for folder
    if(ImGui::Button("Browse for Folder...")) {
        // Button clicked - open folder browser dialog
        std::string selectedFolder = m_fontManager->OpenFolderBrowserDialog(m_hwnd);

        // Check if user selected a folder
        if(!selectedFolder.empty()) {
            // Folder selected - store it
            m_selectedFolderPath = selectedFolder;
            m_statusMessage = "Folder selected: " + selectedFolder;
        } else {
            // User cancelled
            m_statusMessage = "Folder selection cancelled";
        }
    }

    // Display selected folder path
    ImGui::SameLine();
    ImGui::Text("Selected: %s",
        m_selectedFolderPath.empty() ? "None" : m_selectedFolderPath.c_str());

    // Show buttons for loading fonts from folder (only if folder selected)
    if(!m_selectedFolderPath.empty()) {
        // Button to load fonts from folder (non-recursive)
        if(ImGui::Button("Load Fonts from This Folder")) {
            // Load all fonts from the selected folder
            int loaded = m_fontManager->LoadFontsFromFolderToMap(m_selectedFolderPath);

            // Update status with number of fonts loaded
            m_statusMessage = "Loaded " + std::to_string(loaded) + " fonts from folder";
            m_totalFontsLoaded += loaded;
            m_fontsLoaded = true;
        }

        // Button to load fonts recursively
        ImGui::SameLine();
        if(ImGui::Button("Load Fonts Recursively") && bFonstsWereLoaded) {
            // Load fonts from folder and all subfolders
            int loaded = m_fontManager->LoadFontsFromFolderRecursiveToMap(
                m_selectedFolderPath, true);

            m_statusMessage = "Loaded " + std::to_string(loaded) +
                " fonts recursively";
            m_totalFontsLoaded += loaded;
            m_fontsLoaded = true;

        }

        // Button to search fonts without loading
        if(ImGui::Button("Search Fonts (Don't Load)") && bFonstsWereLoaded) {
            // Search for fonts but don't load them into memory
            std::map<std::string, std::string> foundFonts =
                m_fontManager->SearchFontsInFolderAsMap(m_selectedFolderPath, false);

            m_statusMessage = "Found " + std::to_string(foundFonts.size()) +
                " font files";
        }
    }

    // Add spacing
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ====================================================================
    // SECTION 3: LOADED FONTS DISPLAY
    // ====================================================================

    // Display header for loaded fonts section
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Loaded Fonts:");

    // Display total count of loaded fonts
    ImGui::Text("Total fonts in memory: %zu", m_fontManager->GetFontMap().size());

    // Create a child window with scrollbar for font list
    // This creates a scrollable area to display all loaded fonts
    ImGui::BeginChild("FontList",           // ID for this child window
        ImVec2(0, 200),        // Size: full width, 200 pixels height
        true,                  // Show border around child window
        ImGuiWindowFlags_HorizontalScrollbar);  // Enable horizontal scroll

    // Get reference to the font map
    const std::map<std::string, ImFont*>& fontMap = m_fontManager->GetFontMap();

    // Iterate through all loaded fonts
    int fontIndex = 0;
    for(const auto& pair : fontMap) {
        // pair.first = font name (string)
        // pair.second = ImFont pointer

        // Create a unique ID for this font entry
        ImGui::PushID(fontIndex);

        // Display font name as text
        ImGui::Text("%d. %s", fontIndex + 1, pair.first.c_str());

        // Add button to test this font
        ImGui::SameLine();
        if(ImGui::SmallButton("Test")) {
            // User clicked Test - push this font and show sample text
            // Note: In a real app, you'd show this in a separate area
            m_statusMessage = "Testing font: " + pair.first;
        }

        // Pop the ID to avoid conflicts
        ImGui::PopID();

        fontIndex++;
    }

    // End the scrollable child window
    ImGui::EndChild();

    // Add spacing
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ====================================================================
    // SECTION 4: STATUS BAR
    // ====================================================================

    // Display status message at the bottom
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Status:");
    ImGui::SameLine();

    // Color code the status based on content
    if(m_statusMessage.find("ERROR") != std::string::npos) {
        // Error message - show in red
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", m_statusMessage.c_str());
    } else if(m_statusMessage.find("Success") != std::string::npos) {
        // Success message - show in green
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", m_statusMessage.c_str());
    } else {
        // Normal message - show in white
        ImGui::Text("%s", m_statusMessage.c_str());
    }

    // End the ImGui window
    ImGui::End();
}

void FontManagerWindow::RenderFontPreview() {
    // Create a separate window to preview loaded fonts
    ImGui::Begin("Font Preview");

    // Sample text to display in each font
    const char* sampleText = "The quick brown fox jumps over the lazy dog 0123456789";

    // Get the font map
    const std::map<std::string, ImFont*>& fontMap = m_fontManager->GetFontMap();

    // Show preview for each loaded font
    for(const auto& pair : fontMap) {
        // Push the font to use it for rendering
        ImGui::PushFont(pair.second, NULL);

        // Display the font name and sample text
        ImGui::Text("%s:", pair.first.c_str());
        ImGui::TextWrapped("%s", sampleText);

        // Pop the font to restore previous font
        ImGui::PopFont();

        // Add separator between fonts
        ImGui::Separator();
    }

    ImGui::End();
}

void ExampleUsageInMainLoop(HWND hwnd) {
    // Get ImGui IO
    ImGuiIO& io = ImGui::GetIO();

    // Create FontManager
    FontManager fontManager(&io);

    // Create dialog example UI
    FontManagerWindow dialogUI(&fontManager, hwnd);

    // Main application loop (pseudo-code)
    bool running = true;
    while(running) {
        // Process Windows messages
        // ... (message pump code here)

        // Start ImGui frame
        // ImGui_ImplDX11_NewFrame();
        // ImGui_ImplWin32_NewFrame();
        // ImGui::NewFrame();

        // Render our dialog UI
        dialogUI.Render();

        // Optional: Render font preview window
        dialogUI.RenderFontPreview();

        // Render ImGui
        // ImGui::Render();
        // ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present frame
        // g_pSwapChain->Present(1, 0);
    }

    // Cleanup
    fontManager.CleanupFonts();
}
