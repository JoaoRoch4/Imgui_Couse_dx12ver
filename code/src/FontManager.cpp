// FontManager.cpp - Font Manager Class Implementation
// This file implements all the methods declared in FontManager.hpp

#include "PCH.hpp"		   // Include precompiled header
#include "FontManager.hpp" // Include our class declaration
namespace app {

// Namespace alias for shorter code - 'fs' now refers to std::filesystem
namespace fs = std::filesystem;

// ============================================================================
// CONSTRUCTORS AND DESTRUCTOR
// ============================================================================

// Default constructor - initializes all pointers to nullptr and vector to empty
FontManager::FontManager()
: m_io(nullptr),		  // ImGuiIO pointer set to null
  m_defaultFont(nullptr), // Default font pointer set to null
  m_loadedFonts({})		  // Initialize empty vector of font pointers
{}

void FontManager::GetIo(ImGuiIO* io) {

	const auto line = __LINE__;

	if (io == nullptr) {
		str err{"m_io passed at "};
		err.append(__FUNCTION__);
		err.append(" line: ");
		err.append(std::to_string(line));
		err.append("is nullptr! ");
		throw std::runtime_error(err.c_str());
	}
	m_io = io;
}


// Destructor - currently empty, but could be used for cleanup
FontManager::~FontManager() {}

// ============================================================================
// ORIGINAL FONT LOADING METHOD
// ============================================================================

// LoadFonts - Your original implementation with hardcoded font paths
void FontManager::LoadFonts() {
	// Load font from Windows Fonts folder - Segoe UI
	ImFont* font2 = m_io->Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
	IM_ASSERT(font2 != nullptr);	// Assert that font loaded successfully
	m_loadedFonts.push_back(font2); // Add to our vector of loaded fonts

	//// Load DroidSans from relative path
	//ImFont* font3 = m_io->Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
	//IM_ASSERT(font3 != nullptr);
	//m_loadedFonts.push_back(font3);

	//// Load Roboto Medium font
	//ImFont* font4 = m_io->Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
	//IM_ASSERT(font4 != nullptr);
	//m_loadedFonts.push_back(font4);

	//// Load Cousine Regular font
	//ImFont* font5 = m_io->Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
	//IM_ASSERT(font5 != nullptr);
	//m_loadedFonts.push_back(font5);

	// Load Arial from Windows Fonts
	ImFont* font6 = m_io->Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Arial.TTF");
	IM_ASSERT(font6 != nullptr);
	m_loadedFonts.push_back(font6);

	// Load Arial Rounded Bold
	ImFont* font7 = m_io->Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ARLRDBD.TTF");
	IM_ASSERT(font7 != nullptr);
	m_loadedFonts.push_back(font7);
}

// ============================================================================
// NEW: FOLDER SCANNING METHODS
// ============================================================================

// LoadFontsFromFolder - Scans a single folder (non-recursive) for font files
// @param folderPath: String path to the folder to scan
// @return: Integer count of successfully loaded fonts
int FontManager::LoadFontsFromFolder(const std::string& folderPath) {
	int loadedCount = 0; // Counter for successfully loaded fonts

	// Check if the provided path exists in the filesystem
	if (!fs::exists(folderPath)) {
		// Path doesn't exist - return 0 fonts loaded
		return 0;
	}

	// Check if the path is actually a directory (not a file)
	if (!fs::is_directory(folderPath)) {
		// Path exists but is not a directory - return 0
		return 0;
	}

	// Iterate through all entries in the directory
	// fs::directory_iterator only looks at immediate children (non-recursive)
	for (const auto& entry : fs::directory_iterator(folderPath)) {
		// Check if this entry is a regular file (not a directory or special file)
		if (entry.is_regular_file()) {
			// Get the path object for this file
			const fs::path& filePath = entry.path();

			// Check if this file has a valid font extension
			if (IsFontFile(filePath)) {
				// Convert path to string for ImGui's AddFontFromFileTTF
				std::string fontPathStr = filePath.string();

				// Try to load the font using our helper method
				ImFont* loadedFont = LoadSingleFont(fontPathStr);

				// If font loaded successfully (not nullptr)
				if (loadedFont != nullptr) {
					loadedCount++; // Increment our success counter
				}
			}
		}
	}

	// Return total number of fonts successfully loaded
	return loadedCount;
}

// LoadFontsFromFolderRecursive - Scans folder and optionally all subfolders
// @param folderPath: String path to the root folder
// @param recursive: Boolean flag - if true, scans all subdirectories
// @return: Integer count of successfully loaded fonts
int FontManager::LoadFontsFromFolderRecursive(const std::string& folderPath, bool recursive) {
	int loadedCount = 0; // Counter for successfully loaded fonts

	// Validate that the path exists and is a directory
	if (!fs::exists(folderPath)) {
		return 0; // Path doesn't exist
	}

	if (!fs::is_directory(folderPath)) {
		return 0; // Path is not a directory
	}

	// Choose the appropriate iterator based on recursive flag
	if (recursive) {
		// Use recursive_directory_iterator to scan all subdirectories
		// This will walk through the entire directory tree
		for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
			// Check if this entry is a regular file
			if (entry.is_regular_file()) {
				const fs::path& filePath = entry.path();

				// Check if file has valid font extension
				if (IsFontFile(filePath)) {
					std::string fontPathStr = filePath.string();

					// Attempt to load the font
					ImFont* loadedFont = LoadSingleFont(fontPathStr);

					if (loadedFont != nullptr) {
						loadedCount++; // Count successful load
					}
				}
			}
		}
	} else {
		// Non-recursive mode - just use regular directory_iterator
		// This only scans the immediate folder
		for (const auto& entry : fs::directory_iterator(folderPath)) {
			if (entry.is_regular_file()) {
				const fs::path& filePath = entry.path();

				if (IsFontFile(filePath)) {
					std::string fontPathStr = filePath.string();
					ImFont*		loadedFont	= LoadSingleFont(fontPathStr);

					if (loadedFont != nullptr) { loadedCount++; }
				}
			}
		}
	}

	return loadedCount; // Return total fonts loaded
}

// ============================================================================
// NEW: FOLDER LOADING METHODS WITH MAP
// ============================================================================

// LoadFontsFromFolderToMap - Scans folder and loads fonts into map with names as keys
// @param folderPath: String path to the folder to scan
// @return: Integer count of successfully loaded fonts
int FontManager::LoadFontsFromFolderToMap(const std::string& folderPath) {
	int loadedCount = 0; // Counter for successfully loaded fonts

	// Validate that the provided path exists in the filesystem
	if (!fs::exists(folderPath)) {
		// Path doesn't exist - return 0 fonts loaded
		return 0;
	}

	// Validate that the path is actually a directory (not a file)
	if (!fs::is_directory(folderPath)) {
		// Path exists but is not a directory - return 0
		return 0;
	}

	// Iterate through all entries in the directory (non-recursive)
	for (const auto& entry : fs::directory_iterator(folderPath)) {
		// Check if this entry is a regular file
		if (entry.is_regular_file()) {
			// Get the path object for this file
			const fs::path& filePath = entry.path();

			// Check if this file has a valid font extension
			if (IsFontFile(filePath)) {
				// Extract the font name (filename without extension)
				std::string fontName = ExtractFontName(filePath);

				// Convert path to string for ImGui's AddFontFromFileTTF
				std::string fontPathStr = filePath.string();

				// Try to load the font using ImGui
				ImFont* loadedFont = m_io->Fonts->AddFontFromFileTTF(fontPathStr.c_str());

				// If font loaded successfully (not nullptr)
				if (loadedFont != nullptr) {
					// Add to both vector and map
					m_loadedFonts.push_back(loadedFont);
					m_fontMap[fontName] = loadedFont; // Store in map with name as key

					loadedCount++; // Increment our success counter
				}
			}
		}
	}

	// Return total number of fonts successfully loaded
	return loadedCount;
}

// LoadFontsFromFolderRecursiveToMap - Scans folder recursively and loads into map
// @param folderPath: String path to the root folder
// @param recursive: Boolean flag - if true, scans all subdirectories
// @return: Integer count of successfully loaded fonts
int FontManager::LoadFontsFromFolderRecursiveToMap(const std::string& folderPath, bool recursive) {
	int loadedCount = 0; // Counter for successfully loaded fonts

	// Validate that the path exists and is a directory
	if (!fs::exists(folderPath)) {
		return 0; // Path doesn't exist
	}

	if (!fs::is_directory(folderPath)) {
		return 0; // Path is not a directory
	}

	// Choose the appropriate iterator based on recursive flag
	if (recursive) {
		// Use recursive_directory_iterator to scan all subdirectories
		for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
			// Check if this entry is a regular file
			if (entry.is_regular_file()) {
				const fs::path& filePath = entry.path();

				// Check if file has valid font extension
				if (IsFontFile(filePath)) {
					// Extract font name
					std::string fontName	= ExtractFontName(filePath);
					std::string fontPathStr = filePath.string();

					// Attempt to load the font
					ImFont* loadedFont = m_io->Fonts->AddFontFromFileTTF(fontPathStr.c_str());

					if (loadedFont != nullptr) {
						// Add to both vector and map
						m_loadedFonts.push_back(loadedFont);
						m_fontMap[fontName] = loadedFont;

						loadedCount++; // Count successful load
					}
				}
			}
		}
	} else {
		// Non-recursive mode - just use regular directory_iterator
		for (const auto& entry : fs::directory_iterator(folderPath)) {
			if (entry.is_regular_file()) {
				const fs::path& filePath = entry.path();

				if (IsFontFile(filePath)) {
					std::string fontName	= ExtractFontName(filePath);
					std::string fontPathStr = filePath.string();

					ImFont* loadedFont = m_io->Fonts->AddFontFromFileTTF(fontPathStr.c_str());

					if (loadedFont != nullptr) {
						m_loadedFonts.push_back(loadedFont);
						m_fontMap[fontName] = loadedFont;
						loadedCount++;
					}
				}
			}
		}
	}

	return loadedCount; // Return total fonts loaded
}

// ============================================================================
// HELPER METHODS
// ============================================================================

// IsFontFile - Checks if a file path has a valid font file extension
// @param path: filesystem::path object to check
// @return: true if file has font extension, false otherwise
bool FontManager::IsFontFile(const std::filesystem::path& path) const {
	// Get the file extension (includes the dot, e.g., ".ttf")
	std::string extension = path.extension().string();

	// Convert extension to lowercase for case-insensitive comparison
	// This handles .TTF, .ttf, .TtF, etc. all the same way
	std::transform(extension.begin(), extension.end(), extension.begin(),
				   [](unsigned char c) { return std::tolower(c); });

	// Check if extension matches any valid font format
	// Common font extensions:
	// .ttf  = TrueType Font
	// .otf  = OpenType Font
	// .ttc  = TrueType Collection
	// .dfont = Mac OS X Data Fork Font
	return extension == ".ttf" || // TrueType Font
		   extension == ".otf" || // OpenType Font
		   extension == ".ttc" || // TrueType Collection
		   extension == ".dfont"; // Data Fork Font
}

// ExtractFontName - Extracts the font name from a file path (removes extension)
// @param path: filesystem::path object to extract name from
// @return: String containing font name without extension
std::string FontManager::ExtractFontName(const std::filesystem::path& path) const {
	// Get the filename with extension (e.g., "arial.ttf")
	std::string filename = path.filename().string();

	// Find the position of the last dot (.)
	size_t lastDot = filename.find_last_of('.');

	// If a dot was found, extract everything before it
	if (lastDot != std::string::npos) {
		// Return substring from beginning to the dot (excludes the dot and extension)
		return filename.substr(0, lastDot);
	}

	// No dot found (unusual for font files), return the whole filename
	return filename;
}

// ============================================================================
// WINDOWS FILE DIALOG METHODS
// ============================================================================

// OpenFontFileDialog - Opens Windows file dialog to select a font file
// This uses the Windows Common Dialog API to show a native file picker
// @param hwnd: Handle to parent window (can be NULL if no parent window)
// @return: String containing the selected file path, or empty string if cancelled
std::string FontManager::OpenFontFileDialog(HWND hwnd) const {
	// OPENFILENAMEW structure - contains information for the file dialog
	// The 'W' suffix means it uses wide characters (Unicode)
	OPENFILENAMEW ofn;

	// Buffer to store the selected file path (wide character string)
	// MAX_PATH is a Windows constant = 260 characters
	wchar_t szFile[MAX_PATH] = {0}; // Initialize all characters to 0 (null)

	// Zero-initialize the entire OPENFILENAMEW structure
	// This ensures all fields start with default values
	ZeroMemory(&ofn, sizeof(ofn));

	// Set the size of the structure (required by Windows API)
	ofn.lStructSize = sizeof(ofn);

	// Set the parent window handle
	// If NULL, the dialog will be a top-level window
	ofn.hwndOwner = hwnd;

	// Set the buffer where the selected file path will be stored
	ofn.lpstrFile = szFile;

	// Set the size of the file path buffer
	ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);

	// Set the file type filter
	// Format: "Description\0*.ext1;*.ext2\0Description2\0*.ext3\0\0"
	// \0 is the null terminator that separates each part
	// The filter ends with double null terminator \0\0
	ofn.lpstrFilter = L"Font Files (*.ttf;*.otf;*.ttc)\0*.ttf;*.otf;*.ttc\0"
					  L"TrueType Fonts (*.ttf)\0*.ttf\0"
					  L"OpenType Fonts (*.otf)\0*.otf\0"
					  L"TrueType Collection (*.ttc)\0*.ttc\0"
					  L"All Files (*.*)\0*.*\0";

	// Set which filter to show by default (1-based index)
	// 1 = first filter (Font Files), 2 = second filter (TrueType), etc.
	ofn.nFilterIndex = 1;

	// Set the initial directory (NULL = use current directory or last used)
	ofn.lpstrInitialDir = NULL;

	// Set dialog flags to control behavior
	// OFN_PATHMUSTEXIST: Selected path must exist
	// OFN_FILEMUSTEXIST: Selected file must exist
	// OFN_NOCHANGEDIR: Don't change the current working directory
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	// Show the Open File Dialog
	// GetOpenFileNameW returns TRUE if user selected a file, FALSE if cancelled
	if (GetOpenFileNameW(&ofn) == TRUE) {
		// User selected a file - convert wide string to regular string

		// Get the length of the wide string
		int sizeNeeded = WideCharToMultiByte(CP_UTF8, // Convert to UTF-8 encoding
											 0,		  // No special flags
											 szFile,  // Source wide string
											 -1,	  // -1 means string is null-terminated
											 NULL,	  // NULL means we're just calculating size
											 0,		  // Size is 0 when calculating
											 NULL,	  // No default character
											 NULL);	  // No flag for unmappable characters

		// Create a string with the calculated size
		std::string result(sizeNeeded, 0); // Initialize with null characters

		// Perform the actual conversion from wide string to UTF-8
		WideCharToMultiByte(CP_UTF8,	// Convert to UTF-8
							0,			// No special flags
							szFile,		// Source wide string
							-1,			// Null-terminated
							&result[0], // Destination buffer
							sizeNeeded, // Size of destination buffer
							NULL,		// No default character
							NULL);		// No flag for unmappable characters

		// Remove the null terminator that WideCharToMultiByte added
		result.resize(sizeNeeded - 1);

		// Return the selected file path
		return result;
	}

	// User cancelled the dialog - return empty string
	return "";
}

// Callback function for folder browser dialog
// This is called by Windows to handle events in the folder browser
// LPARAM lParam: User data passed to the dialog (can be used for custom behavior)
// UINT uMsg: Message type (what event occurred)
// LPARAM lpData: Additional message data
// LPARAM lpData: User data pointer (we don't use it here)
int CALLBACK BrowseFolderCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
	// Check if this is the initialization message
	if (uMsg == BFFM_INITIALIZED) {
		// Set the initial selection to C:\Windows\Fonts as an example
		// You can change this to any default folder you want
		// SendMessage sends a message to the dialog window
		// BFFM_SETSELECTION tells it to select a specific folder
		// TRUE means lpData is a path string (not a PIDL)
		// The last parameter is the path to select
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)L"C:\\");
	}

	// Return 0 to indicate we handled the message
	return 0;
}

// OpenFolderBrowserDialog - Opens Windows folder browser to select a folder
// This uses the SHBrowseForFolder API to show a native folder picker
// @param hwnd: Handle to parent window (can be NULL if no parent window)
// @return: String containing the selected folder path, or empty string if cancelled
std::string FontManager::OpenFolderBrowserDialog(HWND hwnd) const {
	// BROWSEINFOW structure - contains information for the folder browser
	// The 'W' suffix means it uses wide characters (Unicode)
	BROWSEINFOW bi;

	// Zero-initialize the structure
	ZeroMemory(&bi, sizeof(bi));

	// Buffer to store the selected folder path
	wchar_t szFolder[MAX_PATH] = {0};

	// Set the parent window handle
	bi.hwndOwner = hwnd;

	// Set the root folder (NULL = Desktop is the root)
	// You could set this to restrict browsing to a specific folder
	bi.pidlRoot = NULL;

	// Set the buffer where the result will be stored
	bi.pszDisplayName = szFolder;

	// Set the title text that appears at the top of the dialog
	bi.lpszTitle = L"Select Folder Containing Fonts";

	// Set dialog flags to control behavior and appearance
	// BIF_RETURNONLYFSDIRS: Only allow selection of file system directories
	// BIF_NEWDIALOGSTYLE: Use the new style dialog (resizable with folder tree)
	// BIF_USENEWUI: Use new UI features (combine with BIF_NEWDIALOGSTYLE)
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_USENEWUI;

	// Set the callback function for dialog events
	// This allows us to set an initial folder selection
	bi.lpfn = BrowseFolderCallback;

	// User data to pass to callback (we don't use it, so set to 0)
	bi.lParam = 0;

	// Show the folder browser dialog
	// SHBrowseForFolderW returns a PIDL (Pointer to Item ID List)
	// PIDL is Windows' way of identifying shell objects like folders
	// Returns NULL if user cancelled
	PIDLIST_ABSOLUTE pidl = SHBrowseForFolderW(&bi);

	// Check if user selected a folder (not cancelled)
	if (pidl != NULL) {
		// Convert PIDL to actual file system path
		// SHGetPathFromIDListW converts the PIDL to a readable path string
		if (SHGetPathFromIDListW(pidl, szFolder)) {
			// Successfully got the path - convert wide string to regular string

			// Calculate size needed for conversion
			int sizeNeeded = WideCharToMultiByte(CP_UTF8,  // Convert to UTF-8
												 0,		   // No special flags
												 szFolder, // Source wide string
												 -1,	   // Null-terminated
												 NULL,	   // Just calculating size
												 0,		   // Size is 0 when calculating
												 NULL,	   // No default character
												 NULL);	   // No flag for unmappable characters

			// Create string with calculated size
			std::string result(sizeNeeded, 0);

			// Perform actual conversion
			WideCharToMultiByte(CP_UTF8,	// Convert to UTF-8
								0,			// No special flags
								szFolder,	// Source wide string
								-1,			// Null-terminated
								&result[0], // Destination buffer
								sizeNeeded, // Size of destination
								NULL,		// No default character
								NULL);		// No flag for unmappable characters

			// Remove null terminator
			result.resize(static_cast<size_t>(sizeNeeded - 1));

			// Free the PIDL m_memory allocated by SHBrowseForFolderW
			// This is important to prevent m_memory leaks
			// CoTaskMemFree is the correct way to free PIDL m_memory
			CoTaskMemFree(pidl);

			// Return the selected folder path
			return result;
		}

		// Failed to get path from PIDL - free m_memory and return empty string
		CoTaskMemFree(pidl);
	}

	// User cancelled or error occurred - return empty string
	return "";
}

// LoadSingleFont - Safely loads a single font file and adds to loaded fonts vector
// @param fontPath: String path to the font file
// @return: Pointer to loaded ImFont, or nullptr if loading failed
ImFont* FontManager::LoadSingleFont(const std::string& fontPath) {
	// Attempt to load the font using ImGui's font loading function
	// This function returns nullptr if the file cannot be loaded
	ImFont* font = m_io->Fonts->AddFontFromFileTTF(fontPath.c_str());

	// Check if font loaded successfully
	if (font != nullptr) {
		// Font loaded successfully - add it to our vector of loaded fonts
		m_loadedFonts.push_back(font);
	}

	// Return the font pointer (will be nullptr if loading failed)
	return font;
}

// ============================================================================
// CLEANUP METHODS
// ============================================================================

// CleanupFonts - Clears all loaded fonts from the vector and map
void FontManager::CleanupFonts() {
	// Iterate through all loaded fonts in the vector
	for (const auto* font : m_loadedFonts) {
		// Note: Setting font to nullptr here doesn't actually free m_memory
		// because 'font' is a local copy of the pointer
		// ImGui manages the actual font m_memory internally
		font = nullptr; // This line doesn't actually do much
	}

	// Clear the vector, removing all font pointers
	m_loadedFonts.clear();

	// Clear the map, removing all font name -> pointer mappings
	m_fontMap.clear();
}

// ============================================================================
// DEFAULT FONT SETTERS
// ============================================================================

// SetDefaultFont - Sets default font to font at index 5 (Arial in your original code)
void FontManager::SetDefaultFont() {
	return;
	m_io->FontDefault = m_loadedFonts.at(5); // .at() throws if index out of bounds
}

// SetDefaultFont - Sets a specific font as the default
// @param font: Pointer to ImFont to use as default
void FontManager::SetDefaultFont(ImFont* font) {
	m_defaultFont = font; // Store in our member variable
}

// SetDefaultFont - Loads a font from path and sets it as default
// @param path: C-string path to font file
void FontManager::SetDefaultFont(const char* path) {
	// Load the font from the provided path
	ImFont* fontarg = m_io->Fonts->AddFontFromFileTTF(path);
	// Note: This function doesn't check if loading succeeded or store the font
	// You might want to add error checking here
}

// ============================================================================
// GETTER METHODS
// ============================================================================

// GetLoadedFonts - Returns const reference to the vector of loaded fonts
// @return: Const reference to vector (cannot be modified by caller)
const std::vector<ImFont*>& FontManager::GetLoadedFonts() const { return m_loadedFonts; }

// GetFontMap - Returns const reference to the map of loaded fonts
// @return: Const reference to map with font names as keys
const std::map<std::string, ImFont*>& FontManager::GetFontMap() const { return m_fontMap; }

// GetFontByName - Retrieves a font by its name from the map
// @param fontName: String name of the font to find (e.g., "arial")
// @return: Pointer to ImFont if found, nullptr if not found
ImFont* FontManager::GetFontByName(const std::string& fontName) const {
	// Try to find the font name in the map
	// std::map::find returns an iterator to the element if found
	auto it = m_fontMap.find(fontName);

	// Check if the font was found
	if (it != m_fontMap.end()) {
		// Font found - return the pointer (second element of the pair)
		return it->second;
	}

	// Font not found - return nullptr
	return nullptr;
}

// GetFontCount - Returns the number of loaded fonts
// @return: Size of the loaded fonts vector
size_t FontManager::GetFontCount() const { return m_loadedFonts.size(); }

// ============================================================================
// FONT SEARCH METHODS (WITHOUT LOADING)
// ============================================================================

// SearchFontsInFolder - Searches for font files and returns their paths
// This method does NOT load the fonts, it only finds them
// @param folderPath: String path to the folder to search
// @param recursive: Boolean flag - if true, searches all subdirectories
// @return: Vector of strings containing full paths to all font files found
std::vector<std::string> FontManager::SearchFontsInFolder(const std::string& folderPath,
														  bool				 recursive) const {

	// Create an empty vector to store the paths of found fonts
	std::vector<std::string> foundFonts;

	// Validate that the provided path exists in the filesystem
	if (!fs::exists(folderPath)) {
		// Path doesn't exist - return empty vector
		return foundFonts;
	}

	// Validate that the path is actually a directory (not a regular file)
	if (!fs::is_directory(folderPath)) {
		// Path exists but is not a directory - return empty vector
		return foundFonts;
	}

	// Choose the appropriate iterator based on recursive flag
	if (recursive) {
		// Use recursive_directory_iterator to search all subdirectories
		// This will walk through the entire directory tree
		for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
			// Check if this entry is a regular file (not a directory or special file)
			if (entry.is_regular_file()) {
				// Get the path object for this file
				const fs::path& filePath = entry.path();

				// Check if this file has a valid font extension
				if (IsFontFile(filePath)) {
					// Convert the path to a string and add it to our results vector
					foundFonts.push_back(filePath.string());
				}
			}
		}
	} else {
		// Use regular directory_iterator for non-recursive search
		// This only searches the immediate folder, not subdirectories
		for (const auto& entry : fs::directory_iterator(folderPath)) {
			// Check if this entry is a regular file
			if (entry.is_regular_file()) {
				// Get the path object for this file
				const fs::path& filePath = entry.path();

				// Check if this file has a valid font extension
				if (IsFontFile(filePath)) {
					// Convert the path to a string and add it to our results
					foundFonts.push_back(filePath.string());
				}
			}
		}
	}

	// Return the vector containing all found font file paths
	return foundFonts;
}

// SearchFontsWithInfo - Searches for fonts and returns detailed information
// This method provides more information about each font file found
// @param folderPath: String path to the folder to search
// @param recursive: Boolean flag - if true, searches all subdirectories
// @return: Vector of FontInfo structures with detailed information about each font
std::vector<FontManager::FontInfo> FontManager::SearchFontsWithInfo(const std::string& folderPath,
																	bool recursive) const {

	// Create an empty vector to store FontInfo structures
	std::vector<FontInfo> fontInfoList;

	// Validate that the provided path exists
	if (!fs::exists(folderPath)) {
		// Path doesn't exist - return empty vector
		return fontInfoList;
	}

	// Validate that the path is a directory
	if (!fs::is_directory(folderPath)) {
		// Path is not a directory - return empty vector
		return fontInfoList;
	}

	// Choose iterator based on recursive flag
	if (recursive) {
		// Recursive search through all subdirectories
		for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
			// Only process regular files
			if (entry.is_regular_file()) {
				const fs::path& filePath = entry.path();

				// Check if this is a font file
				if (IsFontFile(filePath)) {
					// Create a FontInfo structure to hold detailed information
					FontInfo info;

					// Store the full path to the font file
					info.fullPath = filePath.string();

					// Extract just the filename (e.g., "arial.ttf" from "C:\Windows\Fonts\arial.ttf")
					info.fileName = filePath.filename().string();

					// Extract the font name without extension (e.g., "arial" from "arial.ttf")
					info.fontName = ExtractFontName(filePath);

					// Extract the file extension (e.g., ".ttf")
					info.extension = filePath.extension().string();

					// Get the file size in bytes
					// Use error_code to handle potential errors without throwing exceptions
					std::error_code ec;
					info.fileSize = fs::file_size(filePath, ec);

					// If there was an error getting file size, set it to 0
					if (ec) { info.fileSize = 0; }

					// Add this FontInfo structure to our results vector
					fontInfoList.push_back(info);
				}
			}
		}
	} else {
		// Non-recursive search - only immediate folder
		for (const auto& entry : fs::directory_iterator(folderPath)) {
			// Only process regular files
			if (entry.is_regular_file()) {
				const fs::path& filePath = entry.path();

				// Check if this is a font file
				if (IsFontFile(filePath)) {
					// Create FontInfo structure
					FontInfo info;

					// Populate with file information
					info.fullPath  = filePath.string();
					info.fileName  = filePath.filename().string();
					info.fontName  = ExtractFontName(filePath);
					info.extension = filePath.extension().string();

					// Get file size with error handling
					std::error_code ec;
					info.fileSize = fs::file_size(filePath, ec);
					if (ec) { info.fileSize = 0; }

					// Add to results
					fontInfoList.push_back(info);
				}
			}
		}
	}

	// Return vector containing all font information
	return fontInfoList;
}

// SearchFontsWithInfoAsMap - Searches for fonts and returns detailed info as map
// Key is the font name (without extension), value is FontInfo structure
// @param folderPath: String path to the folder to search
// @param recursive: Boolean flag - if true, searches all subdirectories
// @return: Map where key is font name and value is FontInfo with details
std::map<std::string, FontManager::FontInfo> FontManager::SearchFontsWithInfoAsMap(
	const std::string& folderPath, bool recursive) const {

	// Create an empty map to store font name -> FontInfo mappings
	std::map<std::string, FontInfo> fontInfoMap;

	// Validate that the provided path exists
	if (!fs::exists(folderPath)) {
		return fontInfoMap; // Return empty map
	}

	// Validate that the path is a directory
	if (!fs::is_directory(folderPath)) {
		return fontInfoMap; // Return empty map
	}

	// Choose iterator based on recursive flag
	if (recursive) {
		// Recursive search through all subdirectories
		for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
			if (entry.is_regular_file()) {
				const fs::path& filePath = entry.path();

				// Check if this is a font file
				if (IsFontFile(filePath)) {
					// Create FontInfo structure
					FontInfo info;

					// Populate with file information
					info.fullPath  = filePath.string();
					info.fileName  = filePath.filename().string();
					info.fontName  = ExtractFontName(filePath);
					info.extension = filePath.extension().string();

					// Get file size with error handling
					std::error_code ec;
					info.fileSize = fs::file_size(filePath, ec);
					if (ec) {
						info.fileSize = 0; // Set to 0 on error
					}

					// Add to map using font name as key
					// If font name already exists, this will overwrite it
					fontInfoMap[info.fontName] = info;
				}
			}
		}
	} else {
		// Non-recursive search
		for (const auto& entry : fs::directory_iterator(folderPath)) {
			if (entry.is_regular_file()) {
				const fs::path& filePath = entry.path();

				if (IsFontFile(filePath)) {
					// Create and populate FontInfo
					FontInfo info;
					info.fullPath  = filePath.string();
					info.fileName  = filePath.filename().string();
					info.fontName  = ExtractFontName(filePath);
					info.extension = filePath.extension().string();

					// Get file size
					std::error_code ec;
					info.fileSize = fs::file_size(filePath, ec);
					if (ec) { info.fileSize = 0; }

					// Add to map
					fontInfoMap[info.fontName] = info;
				}
			}
		}
	}

	// Return the map containing font name -> FontInfo mappings
	return fontInfoMap;
}
// SearchFontsWithInfoAsMap - Searches for fonts and returns detailed info as map
// Key is the font name (without extension), value is FontInfo structure
// @param folderPath: String path to the folder to search
// @param recursive: Boolean flag - if true, searches all subdirectories
// @return: Map where key is font name and value is FontInfo with details
std::map<std::string, std::string> FontManager::SearchFontsInFolderAsMap(
	const std::string& folderPath, bool recursive) const {

	// Create an empty map to store font name -> FontInfo mappings
	std::map<std::string, std::string> fontInfoMap;
	return fontInfoMap;
	/*
    // Validate that the provided path exists
    if (!fs::exists(folderPath)) {
        return fontInfoMap;  // Return empty map
    }
    
    // Validate that the path is a directory
    if (!fs::is_directory(folderPath)) {
        return fontInfoMap;  // Return empty map
    }
    
    // Choose iterator based on recursive flag
    if (recursive) {
        // Recursive search through all subdirectories
        for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
            if (entry.is_regular_file()) {
                const fs::path& filePath = entry.path();
                
                // Check if this is a font file
                if (IsFontFile(filePath)) {
                    // Create FontInfo structure
                    FontInfo info;
                    
                    // Populate with file information
                    info.fullPath = filePath.string();
                    info.fileName = filePath.filename().string();
                    info.fontName = ExtractFontName(filePath);
                    info.extension = filePath.extension().string();
                    
                    // Get file size with error handling
                    std::error_code ec;
                    info.fileSize = fs::file_size(filePath, ec);
                    if (ec) {
                        info.fileSize = 0;  // Set to 0 on error
                    }
                    
                    // Add to map using font name as key
                    // If font name already exists, this will overwrite it
                    fontInfoMap[info.fontName] = info;
                }
            }
        }
    } else {
        // Non-recursive search
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (entry.is_regular_file()) {
                const fs::path& filePath = entry.path();
                
                if (IsFontFile(filePath)) {
                    // Create and populate FontInfo
                    FontInfo info;
                    info.fullPath = filePath.string();
                    info.fileName = filePath.filename().string();
                    info.fontName = ExtractFontName(filePath);
                    info.extension = filePath.extension().string();
                    
                    // Get file size
                    std::error_code ec;
                    info.fileSize = fs::file_size(filePath, ec);
                    if (ec) {
                        info.fileSize = 0;
                    }
                    
                    // Add to map
                    fontInfoMap[info.fontName] = info;
                }
            }
        }
    }
    
    // Return the map containing font name -> FontInfo mappings
    return fontInfoMap;
    */
}
} // namespace app
