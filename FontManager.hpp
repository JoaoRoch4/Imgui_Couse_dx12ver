// FontManager.hpp - Font Manager Class Declaration
// This header declares the FontManager class which handles loading and managing ImGui fonts

#pragma once  // Prevent multiple inclusion of this header
#include "PCH.hpp"  // Include precompiled header with all necessary dependencies

// FontManager class - Manages ImGui font loading and switching
class FontManager {
private:
    // Pointer to ImGui's IO structure (handles input/output and configuration)
    ImGuiIO* io;
    
    // Pointer to the default font that will be used when none is explicitly selected
    ImFont* m_defaultFont;
    
    // Vector (dynamic array) storing pointers to all loaded fonts
    std::vector<ImFont*> m_loadedFonts;
    
    // Map storing loaded fonts with their names as keys
    // Key: Font name (e.g., "Arial"), Value: Pointer to ImFont
    std::map<std::string, ImFont*> m_fontMap;

public:
    // Default constructor - initializes all members to nullptr/empty
    FontManager();
    
    // Parameterized constructor - takes ImGuiIO pointer for initialization
    // @param io: Pointer to ImGui's IO structure
    FontManager(ImGuiIO* io);
    
    // Destructor - cleans up resources when object is destroyed
    ~FontManager();

    // Loads fonts from hardcoded paths (your original implementation)
    void LoadFonts();
    
    // NEW: Scans a folder and loads all font files found
    // @param folderPath: Path to the folder containing font files
    // @return: Number of fonts successfully loaded
    int LoadFontsFromFolder(const std::string& folderPath);
    
    // NEW: Scans a folder and loads fonts into the map with their names
    // @param folderPath: Path to the folder containing font files
    // @return: Number of fonts successfully loaded
    int LoadFontsFromFolderToMap(const std::string& folderPath);
    
    // NEW: Scans a folder recursively and loads all font files
    // @param folderPath: Path to the root folder to scan
    // @param recursive: If true, scans all subfolders
    // @return: Number of fonts successfully loaded
    int LoadFontsFromFolderRecursive(const std::string& folderPath, bool recursive = true);
    
    // NEW: Scans a folder recursively and loads fonts into the map
    // @param folderPath: Path to the root folder to scan
    // @param recursive: If true, scans all subfolders
    // @return: Number of fonts successfully loaded
    int LoadFontsFromFolderRecursiveToMap(const std::string& folderPath, bool recursive = true);
    
    // Cleans up all loaded fonts by clearing the vector
    void CleanupFonts();
    
    // Sets the default font to the font at index 5 in the loaded fonts vector
    void SetDefaultFont();
    
    // Sets a specific font as the default font
    // @param font: Pointer to the ImFont to set as default
    void SetDefaultFont(ImFont* font);
    
    // Loads a font from a file path and sets it as default
    // @param path: File path to the font file
    void SetDefaultFont(const char* path);
    
    // NEW: Gets the list of all loaded fonts
    // @return: Const reference to vector of loaded font pointers
    const std::vector<ImFont*>& GetLoadedFonts() const;
    
    // NEW: Gets the map of all loaded fonts with their names
    // @return: Const reference to map of font names to font pointers
    const std::map<std::string, ImFont*>& GetFontMap() const;
    
    // NEW: Gets a font by name from the map
    // @param fontName: Name of the font to retrieve
    // @return: Pointer to ImFont if found, nullptr if not found
    ImFont* GetFontByName(const std::string& fontName) const;
    
    // NEW: Gets the number of loaded fonts
    // @return: Count of loaded fonts
    size_t GetFontCount() const;
    
    // NEW: Search and list all font files in a folder without loading them
    // @param folderPath: Path to the folder to search
    // @param recursive: If true, searches all subfolders
    // @return: Vector of strings containing full paths to all font files found
    std::vector<std::string> SearchFontsInFolder(const std::string& folderPath, bool recursive = false) const;
    
    // NEW: Search fonts and return as a map with font names as keys
    // @param folderPath: Path to the folder to search
    // @param recursive: If true, searches all subfolders
    // @return: Map where key is font name and value is full path
    std::map<std::string, std::string> SearchFontsInFolderAsMap(const std::string& folderPath, bool recursive = false) const;
    
    // NEW: Search and get detailed information about fonts in a folder
    // @param folderPath: Path to the folder to search
    // @param recursive: If true, searches all subfolders
    // @return: Vector of FontInfo structures with detailed font information
    struct FontInfo {
        std::string fullPath;      // Full path to the font file
        std::string fileName;      // Just the file name (e.g., "arial.ttf")
        std::string fontName;      // Font name without extension (e.g., "arial")
        std::string extension;     // File extension (e.g., ".ttf")
        size_t fileSize;          // File size in bytes
    };
    std::vector<FontInfo> SearchFontsWithInfo(const std::string& folderPath, bool recursive = false) const;
    
    // NEW: Search fonts with info and return as a map
    // @param folderPath: Path to the folder to search
    // @param recursive: If true, searches all subfolders
    // @return: Map where key is font name and value is FontInfo structure
    std::map<std::string, FontInfo> SearchFontsWithInfoAsMap(const std::string& folderPath, bool recursive = false) const;
   std::string OpenFolderBrowserDialog(HWND hwnd) const;
   std::string OpenFontFileDialog(HWND hwnd) const;
private:
    // NEW: Helper function to check if a file has a valid font extension
    // @param path: Path to check
    // @return: true if the file has a font extension (.ttf, .otf, etc.)
    bool IsFontFile(const std::filesystem::path& path) const;
    
    // NEW: Helper function to extract font name from file path
    // Removes the extension and returns just the font name
    // @param path: Path to the font file
    // @return: Font name without extension (e.g., "arial" from "arial.ttf")
    std::string ExtractFontName(const std::filesystem::path& path) const;
    
    // NEW: Helper function to safely load a single font file
    // @param fontPath: Path to the font file
    // @return: Pointer to loaded font, or nullptr if failed
    ImFont* LoadSingleFont(const std::string& fontPath);
};
