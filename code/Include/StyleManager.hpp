// StyleManager.hpp
// Style manager for persistent ImGui style settings
// Uses reflectcpp library for JSON serialization/deserialization

#pragma once

#include "PCH.hpp"
#include "Master.hpp"

namespace app {

// Structure representing ImGui style configuration
// Maps to ImGuiStyle properties for serialization
struct StyleConfiguration {
	// Main style settings
	float Alpha;                      // Global alpha applies to everything
	float DisabledAlpha;              // Additional alpha multiplier for disabled items
	
	// Window properties
	float WindowRounding;             // Radius of window corners rounding
	float WindowBorderSize;           // Thickness of border around windows
	float ChildRounding;              // Radius of child window corners rounding
	float ChildBorderSize;            // Thickness of border around child windows
	float PopupRounding;              // Radius of popup window corners rounding
	float PopupBorderSize;            // Thickness of border around popup windows
	
	// Frame properties
	float FrameRounding;              // Radius of frame corners rounding (for buttons, inputs, etc.)
	float FrameBorderSize;            // Thickness of border around frames
	
	// Indentation and spacing
	float IndentSpacing;              // Horizontal spacing when entering a tree node
	float ScrollbarSize;              // Width of the vertical scrollbar, Height of the horizontal scrollbar
	float ScrollbarRounding;          // Radius of scrollbar corners rounding
	float GrabMinSize;                // Minimum width/height of a grab box for slider/scrollbar
	float GrabRounding;               // Radius of grabs corners rounding
	
	// Tab properties
	float TabRounding;                // Radius of upper corners of a tab
	float TabBorderSize;              // Thickness of border around tabs
	
	// Font scaling
	float FontScaleDpi;               // DPI-aware font scaling factor
	
	// Padding (stored as arrays for reflectcpp compatibility)
	float WindowPaddingX;             // Padding within a window (X component)
	float WindowPaddingY;             // Padding within a window (Y component)
	float FramePaddingX;              // Padding within a framed rectangle (X component)
	float FramePaddingY;              // Padding within a framed rectangle (Y component)
	float ItemSpacingX;               // Horizontal spacing between widgets/lines (X component)
	float ItemSpacingY;               // Vertical spacing between widgets/lines (Y component)
	float ItemInnerSpacingX;          // Horizontal spacing between elements of a composed widget (X component)
	float ItemInnerSpacingY;          // Vertical spacing between elements of a composed widget (Y component)
	float CellPaddingX;               // Padding within a table cell (X component)
	float CellPaddingY;               // Padding within a table cell (Y component)
	
	// Colors - storing the most commonly customized ones
	// Each color is stored as RGBA float values (0.0f to 1.0f)
	struct ColorRGBA {
		float r, g, b, a;
	};
	
	ColorRGBA Text;
	ColorRGBA TextDisabled;
	ColorRGBA WindowBg;
	ColorRGBA ChildBg;
	ColorRGBA PopupBg;
	ColorRGBA Border;
	ColorRGBA BorderShadow;
	ColorRGBA FrameBg;
	ColorRGBA FrameBgHovered;
	ColorRGBA FrameBgActive;
	ColorRGBA TitleBg;
	ColorRGBA TitleBgActive;
	ColorRGBA TitleBgCollapsed;
	ColorRGBA MenuBarBg;
	ColorRGBA ScrollbarBg;
	ColorRGBA ScrollbarGrab;
	ColorRGBA ScrollbarGrabHovered;
	ColorRGBA ScrollbarGrabActive;
	ColorRGBA CheckMark;
	ColorRGBA SliderGrab;
	ColorRGBA SliderGrabActive;
	ColorRGBA Button;
	ColorRGBA ButtonHovered;
	ColorRGBA ButtonActive;
	ColorRGBA Header;
	ColorRGBA HeaderHovered;
	ColorRGBA HeaderActive;
	ColorRGBA Separator;
	ColorRGBA SeparatorHovered;
	ColorRGBA SeparatorActive;
	ColorRGBA ResizeGrip;
	ColorRGBA ResizeGripHovered;
	ColorRGBA ResizeGripActive;
	ColorRGBA Tab;
	ColorRGBA TabHovered;
	ColorRGBA TabActive;
	ColorRGBA TabUnfocused;
	ColorRGBA TabUnfocusedActive;
	ColorRGBA PlotLines;
	ColorRGBA PlotLinesHovered;
	ColorRGBA PlotHistogram;
	ColorRGBA PlotHistogramHovered;
	ColorRGBA TableHeaderBg;
	ColorRGBA TableBorderStrong;
	ColorRGBA TableBorderLight;
	ColorRGBA TableRowBg;
	ColorRGBA TableRowBgAlt;
	ColorRGBA TextSelectedBg;
	ColorRGBA DragDropTarget;
	ColorRGBA NavHighlight;
	ColorRGBA NavWindowingHighlight;
	ColorRGBA NavWindowingDimBg;
	ColorRGBA ModalWindowDimBg;
};

class StyleManager : public Master {
public:
	StyleManager();
	virtual ~StyleManager();

	// Master interface implementation
	virtual void Open() override;
	virtual void Tick() override;
	virtual void Close() override;

	// Configuration management
	bool LoadConfiguration();
	bool SaveConfiguration();

	// Apply the loaded style to ImGui
	void ApplyStyleToImGui();

	// Capture current ImGui style to our configuration
	void CaptureStyleFromImGui();

	// Apply a preset style
	void ApplyPresetDark();
	void ApplyPresetLight();
	void ApplyPresetClassic();

	// File path management
	const std::wstring& GetConfigFilePath() const;
	void SetConfigFilePath(const std::wstring& path);

	// Direct access to configuration for GUI editing
	StyleConfiguration& GetStyleConfig();
	const StyleConfiguration& GetStyleConfig() const;

	// Color utility functions for dynamic theme-aware colors
	static ImVec4 GetThemedColor(ImGuiCol_ colorIndex, float rMult = 1.0f, float gMult = 1.0f, float bMult = 1.0f, float aMult = 1.0f);
	static ImVec4 GetInfoButtonColor();      // Blue-tinted button
	static ImVec4 GetSuccessButtonColor();   // Green-tinted button
	static ImVec4 GetWarningButtonColor();   // Yellow-tinted button
	static ImVec4 GetDangerButtonColor();    // Red-tinted button
	
	// Get hover and active states for a color
	static ImVec4 GetHoveredColor(const ImVec4& baseColor, float brightenFactor = 1.2f);
	static ImVec4 GetActiveColor(const ImVec4& baseColor, float darkenFactor = 0.8f);

private:
	// Helper methods for color conversion
	static StyleConfiguration::ColorRGBA ImVec4ToColorRGBA(const ImVec4& color);
	static ImVec4 ColorRGBAToImVec4(const StyleConfiguration::ColorRGBA& color);

	StyleConfiguration m_styleConfig;
	std::wstring m_configFilePath;
	bool m_bConfigLoaded;
};

} // namespace app
