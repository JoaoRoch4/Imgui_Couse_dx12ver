// ConfigManager.cpp
// Implementation of configuration manager with reflectcpp serialization
// Handles persistent storage of application settings in JSON format with UTF-8 encoding

#include "PCH.hpp"
#include "Classes.hpp"
#include "ConfigManager.hpp"

namespace app {
// Include reflectcpp for JSON serialization

/**
 * @brief Constructor - Initializes configuration manager
 * 
 * Sets up default values and determines the configuration file path.
 * The configuration file is stored in the same directory as the executable.
 * Uses UTF-8 encoding for file operations and wide strings for paths.
 * 
 * Default clear color is set to dark gray: RGB(0.15, 0.15, 0.15, 1.0)
 */
ConfigManager::ConfigManager()
: m_config(),						// Initialize config struct
  m_configFilePath(L"config.json"), // Configuration file name (wide string)
  m_bConfigLoaded(false)			// No config loaded yet
{
	// Set default clear color values
	// These values represent a dark gray background
	m_config.clearColorR = 0.15f; // Red component (15% intensity)
	m_config.clearColorG = 0.15f; // Green component (15% intensity)
	m_config.clearColorB = 0.15f; // Blue component (15% intensity)
	m_config.clearColorA = 1.0f;  // Alpha component (100% opaque)
}

/**
 * @brief Destructor - Cleans up resources
 * 
 * Automatically saves configuration before destruction to ensure
 * any changes are persisted to disk.
 */
ConfigManager::~ConfigManager() {
	// Save configuration on destruction
	// This ensures any changes are written to disk
	SaveConfiguration();

	// Reset the loaded flag
	m_bConfigLoaded = false;
}

/**
 * @brief Opens the configuration manager
 * 
 * This method is called during initialization.
 * It attempts to load the configuration from disk.
 * If loading fails, it uses default values.
 * 
 * @override Master::Open()
 */
void ConfigManager::Open() {
	// Try to load configuration from disk
	// If loading fails, we'll use the default values set in constructor
	if (!LoadConfiguration()) {
		// Loading failed - print message to console
		// Convert wide string to narrow string for console output
		std::wcout << L"Using default configuration values" << std::endl;
	} else {
		// Loading succeeded - print confirmation with file path
		std::wcout << L"Configuration loaded successfully from " << m_configFilePath << std::endl;
	}
}

/**
 * @brief Tick method - Called every frame
 * 
 * Currently unused, but could be used for:
 * - Auto-saving at intervals
 * - Monitoring configuration changes
 * - Reloading configuration on file change
 * 
 * @override Master::Tick()
 */
void ConfigManager::Tick() {
	// Currently no per-frame operations needed
	// This could be used for auto-saving or file watching in the future
}

/**
 * @brief Closes the configuration manager
 * 
 * Saves the current configuration to disk before closing.
 * Ensures all changes are persisted.
 * 
 * @override Master::Close()
 */
void ConfigManager::Close() {
	// Save configuration before closing
	// This ensures any unsaved changes are written to disk
	SaveConfiguration();

	// Print confirmation message
	std::wcout << L"ConfigManager closed and configuration saved" << std::endl;
}

/**
 * @brief Loads configuration from JSON file with UTF-8 encoding
 * 
 * This method:
 * 1. Checks if the configuration file exists
 * 2. Opens the file with UTF-8 encoding using wide character functions
 * 3. Reads the entire file content as UTF-8 string
 * 4. Uses reflectcpp to deserialize JSON into AppConfiguration struct
 * 5. Updates member variables with loaded values
 * 
 * If the file doesn't exist or parsing fails, returns false
 * and keeps default values.
 * 
 * UTF-8 Handling:
 * - Uses std::wstring for file paths (Windows native)
 * - Reads file as UTF-8 encoded text
 * - reflectcpp handles UTF-8 JSON parsing
 * 
 * @return true if configuration loaded successfully, false otherwise
 */
bool ConfigManager::LoadConfiguration() {
	// Try-catch block to handle any file or parsing errors
	try {
		// Create a filesystem path object from the wide string config file path
		// std::filesystem::path natively handles wide strings on Windows
		std::filesystem::path configPath(m_configFilePath);

		// Check if the configuration file exists on disk
		// exists() returns true if the file is present, false otherwise
		if (!std::filesystem::exists(configPath)) {
			// File doesn't exist - this is not an error on first run
			// Return false to indicate we should use default values
			std::wcout << L"Configuration file not found: " << m_configFilePath << std::endl;
			return false;
		}

		// Open the file for reading with UTF-8 encoding
		// std::ifstream is an input file stream
		// On Windows, we need to use the wide string path for proper Unicode support
		std::ifstream configFile(configPath, std::ios::in | std::ios::binary);

		// Check if the file was opened successfully
		// is_open() returns true if the file stream is ready to read
		if (!configFile.is_open()) {
			// File exists but couldn't be opened (permission issue, etc.)
			std::wcerr << L"Failed to open configuration file: " << m_configFilePath << std::endl;
			return false;
		}

		// Read the entire file content into a string
		// std::stringstream is used as a buffer to accumulate file content
		std::stringstream buffer;

		// rdbuf() returns the underlying buffer of the file stream
		// The << operator copies the entire buffer content to stringstream
		// This reads all bytes from the file in one operation
		buffer << configFile.rdbuf();

		// Convert stringstream to std::string
		// str() method extracts the accumulated content as a string
		// The file content is UTF-8 encoded
		std::string jsonContent = buffer.str();

		// Close the file now that we've read all content
		// Good practice to close files as soon as we're done with them
		configFile.close();

		// Use reflectcpp to parse JSON string into AppConfiguration struct
		// rfl::json::read<AppConfiguration> is a template function that:
		// 1. Parses the UTF-8 JSON string
		// 2. Maps JSON fields to struct members by name
		// 3. Performs type conversions
		// 4. Returns a Result type that can contain either value or error
		auto result = rfl::json::read<AppConfiguration>(jsonContent);

		// Check if parsing was successful
		// The result object has a value() method if successful
		// or an error() method if parsing failed
		if (result) {
			// Parsing succeeded - extract the value
			// value() returns the deserialized AppConfiguration struct
			m_config = result.value();

			// Set the loaded flag to true
			m_bConfigLoaded = true;

			// Return success
			return true;
		} else {
			// Parsing failed - print error message
			// error() returns an error object with details
			// Convert error message to wide string for console output
			std::string	 errorMsg = result.error().what();
			std::wstring wErrorMsg(errorMsg.begin(), errorMsg.end());
			std::wcerr << L"Failed to parse configuration JSON: " << wErrorMsg << std::endl;
			return false;
		}

	} catch (const std::exception& e) {
		// Catch any exceptions that might occur during file operations
		// std::exception is the base class for standard exceptions
		// what() returns a description of the error
		// Convert error message to wide string for console output
		std::string	 errorMsg = e.what();
		std::wstring wErrorMsg(errorMsg.begin(), errorMsg.end());
		std::wcerr << L"Exception while loading configuration: " << wErrorMsg << std::endl;
		return false;
	}
}

/**
 * @brief Saves configuration to JSON file with UTF-8 encoding
 * 
 * This method:
 * 1. Uses reflectcpp to serialize AppConfiguration struct to UTF-8 JSON
 * 2. Opens the configuration file for writing (creates if doesn't exist)
 * 3. Writes the UTF-8 JSON string to the file
 * 4. Closes the file
 * 
 * The JSON is formatted with indentation for human readability.
 * Uses UTF-8 encoding for proper Unicode support.
 * 
 * UTF-8 Handling:
 * - JSON is written as UTF-8 encoded text
 * - File paths use wide strings on Windows
 * - reflectcpp generates UTF-8 JSON
 * 
 * @return true if configuration saved successfully, false otherwise
 */
bool ConfigManager::SaveConfiguration() {
	// Try-catch block to handle any serialization or file errors
	try {
		// Use reflectcpp to serialize the configuration struct to UTF-8 JSON
		// rfl::json::write is a template function that:
		// 1. Inspects the struct members using reflection
		// 2. Creates a JSON object with member names as keys
		// 3. Converts member values to JSON types
		// 4. Returns a formatted UTF-8 JSON string
		std::string jsonContent = rfl::json::write(m_config);

		// Create a filesystem path object from wide string
		std::filesystem::path configPath(m_configFilePath);

		// Open the file for writing with binary mode
		// std::ofstream is an output file stream
		// std::ios::out - open for writing (default for ofstream)
		// std::ios::trunc - truncate file if it exists (overwrite)
		// std::ios::binary - write in binary mode (no text conversions)
		// Binary mode ensures UTF-8 is written exactly as-is
		std::ofstream configFile(configPath, std::ios::out | std::ios::trunc | std::ios::binary);

		// Check if file was opened successfully
		if (!configFile.is_open()) {
			// Failed to open file for writing (permission issue, disk full, etc.)
			std::wcerr << L"Failed to open configuration file for writing: " << m_configFilePath
					   << std::endl;
			return false;
		}

		// Write the UTF-8 JSON content to the file
		// The << operator writes the string to the file stream
		// Since we opened in binary mode, UTF-8 bytes are written directly
		// No character set conversion is performed
		configFile << jsonContent;

		// Flush the buffer to ensure all data is written
		// This forces any buffered data to be written to disk immediately
		configFile.flush();

		// Check if write operation succeeded
		// good() returns true if no errors occurred during writing
		if (!configFile.good()) {
			std::wcerr << L"Error occurred while writing configuration file" << std::endl;
			configFile.close();
			return false;
		}

		// Close the file
		// This flushes any remaining buffered data and releases the file handle
		// Good practice to explicitly close files
		configFile.close();

		// Print confirmation message with file path
		std::wcout << L"Configuration saved to " << m_configFilePath << std::endl;

		// Return success
		return true;

	} catch (const std::exception& e) {
		// Catch any exceptions during serialization or file writing
		// Convert error message to wide string for console output
		std::string	 errorMsg = e.what();
		std::wstring wErrorMsg(errorMsg.begin(), errorMsg.end());
		std::wcerr << L"Exception while saving configuration: " << wErrorMsg << std::endl;
		return false;
	}
}

/**
 * @brief Sets the clear color values
 * 
 * Updates the clear color in the configuration struct.
 * Call SaveConfiguration() after this to persist changes.
 * 
 * @param r Red component (0.0 to 1.0)
 * @param g Green component (0.0 to 1.0)
 * @param b Blue component (0.0 to 1.0)
 * @param a Alpha component (0.0 to 1.0)
 */
void ConfigManager::SetClearColor(float r, float g, float b, float a) {
	// Update all four color components in the configuration struct
	m_config.clearColorR = r; // Red component
	m_config.clearColorG = g; // Green component
	m_config.clearColorB = b; // Blue component
	m_config.clearColorA = a; // Alpha component (opacity)
}

/**
 * @brief Gets the clear color values by reference
 * 
 * Retrieves the current clear color values from configuration.
 * Values are returned through reference parameters.
 * 
 * @param r Reference to store red component
 * @param g Reference to store green component
 * @param b Reference to store blue component
 * @param a Reference to store alpha component
 */
void ConfigManager::GetClearColor(float& r, float& g, float& b, float& a) const {
	// Copy color components from config struct to reference parameters
	// Using references allows the caller to receive multiple values
	r = m_config.clearColorR; // Copy red component
	g = m_config.clearColorG; // Copy green component
	b = m_config.clearColorB; // Copy blue component
	a = m_config.clearColorA; // Copy alpha component
}

/**
 * @brief Gets the clear color as an ImVec4
 * 
 * Convenience method that returns the color as ImGui's ImVec4 type.
 * This is useful for direct use with ImGui color functions.
 * 
 * @return ImVec4 containing the RGBA color values
 */
ImVec4 ConfigManager::GetClearColorAsImVec4() const {
	// Create and return an ImVec4 with the current color values
	// ImVec4 is a structure with x, y, z, w members (or r, g, b, a)
	return ImVec4(m_config.clearColorR, // Red component (x)
				  m_config.clearColorG, // Green component (y)
				  m_config.clearColorB, // Blue component (z)
				  m_config.clearColorA	// Alpha component (w)
	);
}

/**
 * @brief Gets the configuration file path
 * 
 * Returns a const reference to the configuration file path wide string.
 * Useful for displaying the file location to the user.
 * 
 * @return Const reference to the file path wide string
 */
const std::wstring& ConfigManager::GetConfigFilePath() const {
	// Return a const reference to avoid copying the string
	// The reference remains valid as long as the ConfigManager exists
	return m_configFilePath;
}

/**
 * @brief Sets the configuration file path
 * 
 * Allows changing the configuration file location.
 * Must be called before Open() to take effect.
 * 
 * This is useful for:
 * - Using custom configuration directories
 * - Supporting multiple user profiles
 * - Storing config in user's documents folder
 * 
 * Example paths:
 * - L"config.json" - current directory
 * - L"C:\\Users\\Username\\Documents\\MyApp\\config.json" - absolute path
 * - L"./settings/config.json" - relative path
 * 
 * @param path Wide string containing the new file path
 */
void ConfigManager::SetConfigFilePath(const std::wstring& path) {
	// Update the configuration file path
	// The path is stored as a wide string for proper Unicode support
	m_configFilePath = path;

	// Print confirmation with the new path
	std::wcout << L"Configuration file path set to: " << m_configFilePath << std::endl;
}

} // namespace app
