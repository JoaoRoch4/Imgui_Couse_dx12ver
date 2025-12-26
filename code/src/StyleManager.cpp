// StyleManager.cpp
// Implementation of style manager with reflectcpp serialization
// Handles persistent storage of ImGui style settings in JSON format with UTF-8 encoding

#include "PCH.hpp"
#include "Classes.hpp"
#include "StyleManager.hpp"
#include "StyleConfiguration.hpp"

namespace app {

/**
 * @brief Constructor - Initializes style manager with default values
 *
 * Sets up default ImGui style values (Dark theme).
 * The configuration file is stored in the same directory as the executable.
 * Uses UTF-8 encoding for file operations and wide strings for paths.
 */
StyleManager::StyleManager() :
m_styleConfig(), m_configFilePath(L"style_config.json"), m_bConfigLoaded(false) {
	// Don't call ImGui functions here - ImGui context might not exist yet during construction
	// ApplyPresetDark() and CaptureStyleFromImGui() will be called in Open() instead
}

/**
 * @brief Destructor - Cleans up resources
 *
 * Automatically saves configuration before destruction to ensure
 * any changes are persisted to disk.
 */
StyleManager::~StyleManager() {
	SaveConfiguration();
	m_bConfigLoaded = false;
}

/**
 * @brief Opens the style manager
 *
 * This method is called during initialization.
 * It attempts to load the style configuration from disk.
 * If loading fails, it uses default dark theme values.
 *
 * @override Master::Open()
 */
void StyleManager::Open() {
	// Try to load saved configuration first
	if (!LoadConfiguration()) {
		// No saved config found or config is invalid - initialize with default dark theme
		std::wcout << L"No valid style configuration found, applying default Dark theme" << std::endl;
		ApplyPresetDark();
		CaptureStyleFromImGui();
		
		// Save the default configuration so it's available next time
		SaveConfiguration();
		std::wcout << L"Default style configuration saved to " << m_configFilePath << std::endl;
	} else {
		// Saved config loaded successfully - apply it to ImGui
		std::wcout << L"Style configuration loaded successfully from " << m_configFilePath
				   << std::endl;
		ApplyStyleToImGui();
	}
}

/**
 * @brief Tick method - Called every frame
 *
 * Currently unused, but could be used for:
 * - Auto-saving at intervals
 * - Hot-reloading style on file change
 *
 * @override Master::Tick()
 */
void StyleManager::Tick() {
	// Currently no per-frame operations needed
}

/**
 * @brief Closes the style manager
 *
 * Saves the current style configuration to disk before closing.
 *
 * @override Master::Close()
 */
void StyleManager::Close() {
	SaveConfiguration();
	std::wcout << L"StyleManager closed and configuration saved" << std::endl;
}

/**
 * @brief Loads style configuration from JSON file with UTF-8 encoding
 *
 * Uses reflectcpp to deserialize JSON into StyleConfiguration struct.
 * If the file doesn't exist or parsing fails, returns false.
 *
 * @return true if configuration was loaded successfully, false otherwise
 */
bool StyleManager::LoadConfiguration() {
	try {
		// Check if file exists
		if (!fs::exists(m_configFilePath)) {
			std::wcout << L"Style configuration file not found: " << m_configFilePath << std::endl;
			return false;
		}

		// Open file with UTF-8 encoding
		std::ifstream file(m_configFilePath, std::ios::in | std::ios::binary);
		if (!file.is_open()) {
			std::wcerr << L"Failed to open style configuration file: " << m_configFilePath
					   << std::endl;
			return false;
		}

		// Read entire file content
		std::string jsonContent((std::istreambuf_iterator<char>(file)),
								std::istreambuf_iterator<char>());
		file.close();

		// Deserialize JSON using reflectcpp
		m_styleConfig = rfl::json::read<StyleConfiguration>(jsonContent).value();

		// Validate loaded config - check if it has valid values
		// If Alpha is 0, the config was never properly initialized
		if (m_styleConfig.Alpha <= 0.0f) {
			std::wcout << L"Loaded style configuration appears invalid (Alpha = 0), using defaults" << std::endl;
			return false;
		}

		m_bConfigLoaded = true;
		return true;

	} catch (const std::exception& e) {
		std::cerr << "Error loading style configuration: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Saves current style configuration to JSON file with UTF-8 encoding
 *
 * Uses reflectcpp to serialize StyleConfiguration struct to JSON.
 * Creates the file if it doesn't exist.
 *
 * @return true if configuration was saved successfully, false otherwise
 */
bool StyleManager::SaveConfiguration() {
	try {
		// Check if ImGui context exists before capturing style
		// During shutdown, ImGui context may be destroyed before StyleManager
		if (ImGui::GetCurrentContext() != nullptr) {
			// First capture current ImGui style
			CaptureStyleFromImGui();
		}

		// Serialize configuration to JSON using reflectcpp
		std::string jsonContent = rfl::json::write(m_styleConfig);

		// Open file for writing with UTF-8 encoding
		std::ofstream file(m_configFilePath, std::ios::out | std::ios::binary | std::ios::trunc);
		if (!file.is_open()) {
			std::wcerr << L"Failed to create/open style configuration file: " << m_configFilePath
					   << std::endl;
			return false;
		}

		// Write JSON content
		file << jsonContent;
		file.close();

		std::wcout << L"Style configuration saved to: " << m_configFilePath << std::endl;
		return true;

	} catch (const std::exception& e) {
		std::cerr << "Error saving style configuration: " << e.what() << std::endl;
		return false;
	}
}

/**
 * @brief Apply the loaded style configuration to ImGui
 *
 * Copies values from m_styleConfig to the current ImGuiStyle.
 */
void StyleManager::ApplyStyleToImGui() {
	ImGuiStyle& style = ImGui::GetStyle();

	// Apply main style settings
	style.Alpha			= m_styleConfig.Alpha;
	style.DisabledAlpha = m_styleConfig.DisabledAlpha;

	// Apply window properties
	style.WindowRounding   = m_styleConfig.WindowRounding;
	style.WindowBorderSize = m_styleConfig.WindowBorderSize;
	style.ChildRounding	   = m_styleConfig.ChildRounding;
	style.ChildBorderSize  = m_styleConfig.ChildBorderSize;
	style.PopupRounding	   = m_styleConfig.PopupRounding;
	style.PopupBorderSize  = m_styleConfig.PopupBorderSize;

	// Apply frame properties
	style.FrameRounding	  = m_styleConfig.FrameRounding;
	style.FrameBorderSize = m_styleConfig.FrameBorderSize;

	// Apply spacing and indentation
	style.IndentSpacing		= m_styleConfig.IndentSpacing;
	style.ScrollbarSize		= m_styleConfig.ScrollbarSize;
	style.ScrollbarRounding = m_styleConfig.ScrollbarRounding;
	style.GrabMinSize		= m_styleConfig.GrabMinSize;
	style.GrabRounding		= m_styleConfig.GrabRounding;

	// Apply tab properties
	style.TabRounding	= m_styleConfig.TabRounding;
	style.TabBorderSize = m_styleConfig.TabBorderSize;

	// Apply font scaling
	style.FontScaleDpi = m_styleConfig.FontScaleDpi;

	// Apply padding
	style.WindowPadding = ImVec2(m_styleConfig.WindowPaddingX, m_styleConfig.WindowPaddingY);
	style.FramePadding	= ImVec2(m_styleConfig.FramePaddingX, m_styleConfig.FramePaddingY);
	style.ItemSpacing	= ImVec2(m_styleConfig.ItemSpacingX, m_styleConfig.ItemSpacingY);
	style.ItemInnerSpacing =
		ImVec2(m_styleConfig.ItemInnerSpacingX, m_styleConfig.ItemInnerSpacingY);
	style.CellPadding = ImVec2(m_styleConfig.CellPaddingX, m_styleConfig.CellPaddingY);

	// Apply colors
	style.Colors[ImGuiCol_Text]				= ColorRGBAToImVec4(m_styleConfig.Text);
	style.Colors[ImGuiCol_TextDisabled]		= ColorRGBAToImVec4(m_styleConfig.TextDisabled);
	style.Colors[ImGuiCol_WindowBg]			= ColorRGBAToImVec4(m_styleConfig.WindowBg);
	style.Colors[ImGuiCol_ChildBg]			= ColorRGBAToImVec4(m_styleConfig.ChildBg);
	style.Colors[ImGuiCol_PopupBg]			= ColorRGBAToImVec4(m_styleConfig.PopupBg);
	style.Colors[ImGuiCol_Border]			= ColorRGBAToImVec4(m_styleConfig.Border);
	style.Colors[ImGuiCol_BorderShadow]		= ColorRGBAToImVec4(m_styleConfig.BorderShadow);
	style.Colors[ImGuiCol_FrameBg]			= ColorRGBAToImVec4(m_styleConfig.FrameBg);
	style.Colors[ImGuiCol_FrameBgHovered]	= ColorRGBAToImVec4(m_styleConfig.FrameBgHovered);
	style.Colors[ImGuiCol_FrameBgActive]	= ColorRGBAToImVec4(m_styleConfig.FrameBgActive);
	style.Colors[ImGuiCol_TitleBg]			= ColorRGBAToImVec4(m_styleConfig.TitleBg);
	style.Colors[ImGuiCol_TitleBgActive]	= ColorRGBAToImVec4(m_styleConfig.TitleBgActive);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ColorRGBAToImVec4(m_styleConfig.TitleBgCollapsed);
	style.Colors[ImGuiCol_MenuBarBg]		= ColorRGBAToImVec4(m_styleConfig.MenuBarBg);
	style.Colors[ImGuiCol_ScrollbarBg]		= ColorRGBAToImVec4(m_styleConfig.ScrollbarBg);
	style.Colors[ImGuiCol_ScrollbarGrab]	= ColorRGBAToImVec4(m_styleConfig.ScrollbarGrab);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] =
		ColorRGBAToImVec4(m_styleConfig.ScrollbarGrabHovered);
	style.Colors[ImGuiCol_ScrollbarGrabActive] =
		ColorRGBAToImVec4(m_styleConfig.ScrollbarGrabActive);
	style.Colors[ImGuiCol_CheckMark]		  = ColorRGBAToImVec4(m_styleConfig.CheckMark);
	style.Colors[ImGuiCol_SliderGrab]		  = ColorRGBAToImVec4(m_styleConfig.SliderGrab);
	style.Colors[ImGuiCol_SliderGrabActive]	  = ColorRGBAToImVec4(m_styleConfig.SliderGrabActive);
	style.Colors[ImGuiCol_Button]			  = ColorRGBAToImVec4(m_styleConfig.Button);
	style.Colors[ImGuiCol_ButtonHovered]	  = ColorRGBAToImVec4(m_styleConfig.ButtonHovered);
	style.Colors[ImGuiCol_ButtonActive]		  = ColorRGBAToImVec4(m_styleConfig.ButtonActive);
	style.Colors[ImGuiCol_Header]			  = ColorRGBAToImVec4(m_styleConfig.Header);
	style.Colors[ImGuiCol_HeaderHovered]	  = ColorRGBAToImVec4(m_styleConfig.HeaderHovered);
	style.Colors[ImGuiCol_HeaderActive]		  = ColorRGBAToImVec4(m_styleConfig.HeaderActive);
	style.Colors[ImGuiCol_Separator]		  = ColorRGBAToImVec4(m_styleConfig.Separator);
	style.Colors[ImGuiCol_SeparatorHovered]	  = ColorRGBAToImVec4(m_styleConfig.SeparatorHovered);
	style.Colors[ImGuiCol_SeparatorActive]	  = ColorRGBAToImVec4(m_styleConfig.SeparatorActive);
	style.Colors[ImGuiCol_ResizeGrip]		  = ColorRGBAToImVec4(m_styleConfig.ResizeGrip);
	style.Colors[ImGuiCol_ResizeGripHovered]  = ColorRGBAToImVec4(m_styleConfig.ResizeGripHovered);
	style.Colors[ImGuiCol_ResizeGripActive]	  = ColorRGBAToImVec4(m_styleConfig.ResizeGripActive);
	style.Colors[ImGuiCol_Tab]				  = ColorRGBAToImVec4(m_styleConfig.Tab);
	style.Colors[ImGuiCol_TabHovered]		  = ColorRGBAToImVec4(m_styleConfig.TabHovered);
	style.Colors[ImGuiCol_TabSelected]		  = ColorRGBAToImVec4(m_styleConfig.TabActive);
	style.Colors[ImGuiCol_TabActive]		  = ColorRGBAToImVec4(m_styleConfig.TabActive);
	style.Colors[ImGuiCol_TabDimmed]		  = ColorRGBAToImVec4(m_styleConfig.TabUnfocused);
	style.Colors[ImGuiCol_TabUnfocused]		  = ColorRGBAToImVec4(m_styleConfig.TabUnfocused);
	style.Colors[ImGuiCol_TabDimmedSelected]  = ColorRGBAToImVec4(m_styleConfig.TabUnfocusedActive);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ColorRGBAToImVec4(m_styleConfig.TabUnfocusedActive);
	style.Colors[ImGuiCol_PlotLines]		  = ColorRGBAToImVec4(m_styleConfig.PlotLines);
	style.Colors[ImGuiCol_PlotLinesHovered]	  = ColorRGBAToImVec4(m_styleConfig.PlotLinesHovered);
	style.Colors[ImGuiCol_PlotHistogram]	  = ColorRGBAToImVec4(m_styleConfig.PlotHistogram);
	style.Colors[ImGuiCol_PlotHistogramHovered] =
		ColorRGBAToImVec4(m_styleConfig.PlotHistogramHovered);
	style.Colors[ImGuiCol_TableHeaderBg]	 = ColorRGBAToImVec4(m_styleConfig.TableHeaderBg);
	style.Colors[ImGuiCol_TableBorderStrong] = ColorRGBAToImVec4(m_styleConfig.TableBorderStrong);
	style.Colors[ImGuiCol_TableBorderLight]	 = ColorRGBAToImVec4(m_styleConfig.TableBorderLight);
	style.Colors[ImGuiCol_TableRowBg]		 = ColorRGBAToImVec4(m_styleConfig.TableRowBg);
	style.Colors[ImGuiCol_TableRowBgAlt]	 = ColorRGBAToImVec4(m_styleConfig.TableRowBgAlt);
	style.Colors[ImGuiCol_TextSelectedBg]	 = ColorRGBAToImVec4(m_styleConfig.TextSelectedBg);
	style.Colors[ImGuiCol_DragDropTarget]	 = ColorRGBAToImVec4(m_styleConfig.DragDropTarget);
	style.Colors[ImGuiCol_NavHighlight]		 = ColorRGBAToImVec4(m_styleConfig.NavHighlight);
	style.Colors[ImGuiCol_NavWindowingHighlight] =
		ColorRGBAToImVec4(m_styleConfig.NavWindowingHighlight);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ColorRGBAToImVec4(m_styleConfig.NavWindowingDimBg);
	style.Colors[ImGuiCol_ModalWindowDimBg]	 = ColorRGBAToImVec4(m_styleConfig.ModalWindowDimBg);
}

/**
 * @brief Capture current ImGui style to our configuration
 *
 * Copies values from the current ImGuiStyle to m_styleConfig.
 * Only captures if ImGui context is available.
 */
void StyleManager::CaptureStyleFromImGui() {
	// Guard against calling ImGui functions when context doesn't exist
	// This can happen during shutdown when ImGui is destroyed before StyleManager
	if (ImGui::GetCurrentContext() == nullptr) { return; }

	ImGuiStyle& style = ImGui::GetStyle();

	// Capture main style settings
	m_styleConfig.Alpha			= style.Alpha;
	m_styleConfig.DisabledAlpha = style.DisabledAlpha;

	// Capture window properties
	m_styleConfig.WindowRounding   = style.WindowRounding;
	m_styleConfig.WindowBorderSize = style.WindowBorderSize;
	m_styleConfig.ChildRounding	   = style.ChildRounding;
	m_styleConfig.ChildBorderSize  = style.ChildBorderSize;
	m_styleConfig.PopupRounding	   = style.PopupRounding;
	m_styleConfig.PopupBorderSize  = style.PopupBorderSize;

	// Capture frame properties
	m_styleConfig.FrameRounding	  = style.FrameRounding;
	m_styleConfig.FrameBorderSize = style.FrameBorderSize;

	// Capture spacing and indentation
	m_styleConfig.IndentSpacing		= style.IndentSpacing;
	m_styleConfig.ScrollbarSize		= style.ScrollbarSize;
	m_styleConfig.ScrollbarRounding = style.ScrollbarRounding;
	m_styleConfig.GrabMinSize		= style.GrabMinSize;
	m_styleConfig.GrabRounding		= style.GrabRounding;

	// Capture tab properties
	m_styleConfig.TabRounding	= style.TabRounding;
	m_styleConfig.TabBorderSize = style.TabBorderSize;

	// Capture font scaling
	m_styleConfig.FontScaleDpi = style.FontScaleDpi;

	// Capture padding
	m_styleConfig.WindowPaddingX	= style.WindowPadding.x;
	m_styleConfig.WindowPaddingY	= style.WindowPadding.y;
	m_styleConfig.FramePaddingX		= style.FramePadding.x;
	m_styleConfig.FramePaddingY		= style.FramePadding.y;
	m_styleConfig.ItemSpacingX		= style.ItemSpacing.x;
	m_styleConfig.ItemSpacingY		= style.ItemSpacing.y;
	m_styleConfig.ItemInnerSpacingX = style.ItemInnerSpacing.x;
	m_styleConfig.ItemInnerSpacingY = style.ItemInnerSpacing.y;
	m_styleConfig.CellPaddingX		= style.CellPadding.x;
	m_styleConfig.CellPaddingY		= style.CellPadding.y;

	// Capture colors - Use ImGui defaults directly, no hardcoded overrides
	m_styleConfig.Text					= ImVec4ToColorRGBA(style.Colors[ImGuiCol_Text]);
	m_styleConfig.TextDisabled			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_TextDisabled]);
	m_styleConfig.WindowBg				= ImVec4ToColorRGBA(style.Colors[ImGuiCol_WindowBg]);
	m_styleConfig.ChildBg				= ImVec4ToColorRGBA(style.Colors[ImGuiCol_ChildBg]);
	m_styleConfig.PopupBg				= ImVec4ToColorRGBA(style.Colors[ImGuiCol_PopupBg]);
	m_styleConfig.Border				= ImVec4ToColorRGBA(style.Colors[ImGuiCol_Border]);
	m_styleConfig.BorderShadow			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_BorderShadow]);
	m_styleConfig.FrameBg				= ImVec4ToColorRGBA(style.Colors[ImGuiCol_FrameBg]);
	m_styleConfig.FrameBgHovered		= ImVec4ToColorRGBA(style.Colors[ImGuiCol_FrameBgHovered]);
	m_styleConfig.FrameBgActive			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_FrameBgActive]);
	m_styleConfig.TitleBg				= ImVec4ToColorRGBA(style.Colors[ImGuiCol_TitleBg]);
	m_styleConfig.TitleBgActive			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_TitleBgActive]);
	m_styleConfig.TitleBgCollapsed		= ImVec4ToColorRGBA(style.Colors[ImGuiCol_TitleBgCollapsed]);
	m_styleConfig.MenuBarBg				= ImVec4ToColorRGBA(style.Colors[ImGuiCol_MenuBarBg]);
	m_styleConfig.ScrollbarBg			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_ScrollbarBg]);
	m_styleConfig.ScrollbarGrab			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_ScrollbarGrab]);
	m_styleConfig.ScrollbarGrabHovered	= ImVec4ToColorRGBA(style.Colors[ImGuiCol_ScrollbarGrabHovered]);
	m_styleConfig.ScrollbarGrabActive	= ImVec4ToColorRGBA(style.Colors[ImGuiCol_ScrollbarGrabActive]);
	m_styleConfig.CheckMark				= ImVec4ToColorRGBA(style.Colors[ImGuiCol_CheckMark]);
	m_styleConfig.SliderGrab			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_SliderGrab]);
	m_styleConfig.SliderGrabActive		= ImVec4ToColorRGBA(style.Colors[ImGuiCol_SliderGrabActive]);
	m_styleConfig.Button				= ImVec4ToColorRGBA(style.Colors[ImGuiCol_Button]);
	m_styleConfig.ButtonHovered			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_ButtonHovered]);
	m_styleConfig.ButtonActive			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_ButtonActive]);
	m_styleConfig.Header				= ImVec4ToColorRGBA(style.Colors[ImGuiCol_Header]);
	m_styleConfig.HeaderHovered			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_HeaderHovered]);
	m_styleConfig.HeaderActive			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_HeaderActive]);
	m_styleConfig.Separator				= ImVec4ToColorRGBA(style.Colors[ImGuiCol_Separator]);
	m_styleConfig.SeparatorHovered		= ImVec4ToColorRGBA(style.Colors[ImGuiCol_SeparatorHovered]);
	m_styleConfig.SeparatorActive		= ImVec4ToColorRGBA(style.Colors[ImGuiCol_SeparatorActive]);
	m_styleConfig.ResizeGrip			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_ResizeGrip]);
	m_styleConfig.ResizeGripHovered		= ImVec4ToColorRGBA(style.Colors[ImGuiCol_ResizeGripHovered]);
	m_styleConfig.ResizeGripActive		= ImVec4ToColorRGBA(style.Colors[ImGuiCol_ResizeGripActive]);
	m_styleConfig.Tab					= ImVec4ToColorRGBA(style.Colors[ImGuiCol_Tab]);
	m_styleConfig.TabHovered			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_TabHovered]);
	m_styleConfig.TabActive				= ImVec4ToColorRGBA(style.Colors[ImGuiCol_TabSelected]);
	m_styleConfig.TabUnfocused			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_TabDimmed]);
	m_styleConfig.TabUnfocusedActive	= ImVec4ToColorRGBA(style.Colors[ImGuiCol_TabDimmedSelected]);
	m_styleConfig.PlotLines				= ImVec4ToColorRGBA(style.Colors[ImGuiCol_PlotLines]);
	m_styleConfig.PlotLinesHovered		= ImVec4ToColorRGBA(style.Colors[ImGuiCol_PlotLinesHovered]);
	m_styleConfig.PlotHistogram			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_PlotHistogram]);
	m_styleConfig.PlotHistogramHovered	= ImVec4ToColorRGBA(style.Colors[ImGuiCol_PlotHistogramHovered]);
	m_styleConfig.TableHeaderBg			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_TableHeaderBg]);
	m_styleConfig.TableBorderStrong		= ImVec4ToColorRGBA(style.Colors[ImGuiCol_TableBorderStrong]);
	m_styleConfig.TableBorderLight		= ImVec4ToColorRGBA(style.Colors[ImGuiCol_TableBorderLight]);
	m_styleConfig.TableRowBg			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_TableRowBg]);
	m_styleConfig.TableRowBgAlt			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_TableRowBgAlt]);
	m_styleConfig.TextSelectedBg		= ImVec4ToColorRGBA(style.Colors[ImGuiCol_TextSelectedBg]);
	m_styleConfig.DragDropTarget		= ImVec4ToColorRGBA(style.Colors[ImGuiCol_DragDropTarget]);
	m_styleConfig.NavHighlight			= ImVec4ToColorRGBA(style.Colors[ImGuiCol_NavHighlight]);
	m_styleConfig.NavWindowingHighlight	= ImVec4ToColorRGBA(style.Colors[ImGuiCol_NavWindowingHighlight]);
	m_styleConfig.NavWindowingDimBg		= ImVec4ToColorRGBA(style.Colors[ImGuiCol_NavWindowingDimBg]);
	m_styleConfig.ModalWindowDimBg		= ImVec4ToColorRGBA(style.Colors[ImGuiCol_ModalWindowDimBg]);
}

/**
 * @brief Apply the dark preset style
 * Note: Does not auto-save. Call SaveConfiguration() manually if you want to persist.
 */
void StyleManager::ApplyPresetDark() {
	ImGui::StyleColorsDark();
	CaptureStyleFromImGui();
}

/**
 * @brief Apply the light preset style
 * Note: Does not auto-save. Call SaveConfiguration() manually if you want to persist.
 */
void StyleManager::ApplyPresetLight() {
	ImGui::StyleColorsLight();
	CaptureStyleFromImGui();
}

/**
 * @brief Apply the classic preset style
 * Note: Does not auto-save. Call SaveConfiguration() manually if you want to persist.
 */
void StyleManager::ApplyPresetClassic() {
	ImGui::StyleColorsClassic();
	CaptureStyleFromImGui();
}

/**
 * @brief Get the configuration file path
 */
const std::wstring& StyleManager::GetConfigFilePath() const { return m_configFilePath; }

/**
 * @brief Set the configuration file path
 */
void StyleManager::SetConfigFilePath(const std::wstring& path) { m_configFilePath = path; }

/**
 * @brief Get direct access to style configuration
 */
StyleConfiguration& StyleManager::GetStyleConfig() { return m_styleConfig; }

/**
 * @brief Get const access to style configuration
 */
const StyleConfiguration& StyleManager::GetStyleConfig() const { return m_styleConfig; }

/**
 * @brief Convert ImVec4 to ColorRGBA structure
 */
StyleConfiguration::ColorRGBA StyleManager::ImVec4ToColorRGBA(const ImVec4& color) {
	return StyleConfiguration::ColorRGBA{color.x, color.y, color.z, color.w};
}

/**
 * @brief Convert ColorRGBA structure to ImVec4
 */
ImVec4 StyleManager::ColorRGBAToImVec4(const StyleConfiguration::ColorRGBA& color) {
	return ImVec4(color.r, color.g, color.b, color.a);
}

/**
 * @brief Get a themed color with optional channel multipliers
 * 
 * @param colorIndex ImGui color index to use as base
 * @param rMult Red channel multiplier (default 1.0)
 * @param gMult Green channel multiplier (default 1.0)
 * @param bMult Blue channel multiplier (default 1.0)
 * @param aMult Alpha channel multiplier (default 1.0)
 * @return ImVec4 color with applied multipliers
 */
ImVec4 StyleManager::GetThemedColor(ImGuiCol_ colorIndex, float rMult, float gMult, float bMult, float aMult) {
	const ImGuiStyle& style = ImGui::GetStyle();
	const ImVec4& baseColor = style.Colors[colorIndex];
	
	return ImVec4(
		baseColor.x * rMult,
		baseColor.y * gMult,
		baseColor.z * bMult,
		baseColor.w * aMult
	);
}

/**
 * @brief Get an info-style button color (blue-tinted)
 */
ImVec4 StyleManager::GetInfoButtonColor() {
	return GetThemedColor(ImGuiCol_Button, 0.8f, 0.8f, 1.2f, 1.0f);
}

/**
 * @brief Get a success-style button color (green-tinted)
 */
ImVec4 StyleManager::GetSuccessButtonColor() {
	return GetThemedColor(ImGuiCol_Button, 0.8f, 1.5f, 0.8f, 1.0f);
}

/**
 * @brief Get a warning-style button color (yellow-tinted)
 */
ImVec4 StyleManager::GetWarningButtonColor() {
	return GetThemedColor(ImGuiCol_Button, 1.3f, 1.3f, 0.7f, 1.0f);
}

/**
 * @brief Get a danger-style button color (red-tinted)
 */
ImVec4 StyleManager::GetDangerButtonColor() {
	return GetThemedColor(ImGuiCol_Button, 1.5f, 0.7f, 0.7f, 1.0f);
}

/**
 * @brief Get a brightened version of a color (for hover states)
 * 
 * @param baseColor Base color to brighten
 * @param brightenFactor Multiplier for brightness (default 1.2 = 20% brighter)
 * @return Brightened color
 */
ImVec4 StyleManager::GetHoveredColor(const ImVec4& baseColor, float brightenFactor) {
	return ImVec4(
		baseColor.x * brightenFactor,
		baseColor.y * brightenFactor,
		baseColor.z * brightenFactor,
		baseColor.w
	);
}

/**
 * @brief Get a darkened version of a color (for active/pressed states)
 * 
 * @param baseColor Base color to darken
 * @param darkenFactor Multiplier for darkness (default 0.8 = 20% darker)
 * @return Darkened color
 */
ImVec4 StyleManager::GetActiveColor(const ImVec4& baseColor, float darkenFactor) {
	return ImVec4(
		baseColor.x * darkenFactor,
		baseColor.y * darkenFactor,
		baseColor.z * darkenFactor,
		baseColor.w
	);
}

} // namespace app
