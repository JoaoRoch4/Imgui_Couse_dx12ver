#pragma once

#include "PCH.hpp"

namespace app {


// Structure representing ImGui style configuration
// Maps to ImGuiStyle properties for serialization
// All members initialized to 0.0f - actual values captured from ImGui at runtime
struct StyleConfiguration {

	// Main style settings
	float Alpha = 0.0f;		 // Global alpha applies to everything
	float DisabledAlpha = 0.0f; // Additional alpha multiplier for disabled items

	// Window properties
	float WindowRounding = 0.0f;	// Radius of window corners rounding
	float WindowBorderSize = 0.0f; // Thickness of border around windows
	float ChildRounding = 0.0f;	// Radius of child window corners rounding
	float ChildBorderSize = 0.0f;	// Thickness of border around child windows
	float PopupRounding = 0.0f;	// Radius of popup window corners rounding
	float PopupBorderSize = 0.0f;	// Thickness of border around popup windows

	// Frame properties
	float FrameRounding = 0.0f;   // Radius of frame corners rounding (for buttons, inputs, etc.)
	float FrameBorderSize = 0.0f; // Thickness of border around frames

	// Indentation and spacing
	float IndentSpacing = 0.0f;	 // Horizontal spacing when entering a tree node
	float ScrollbarSize = 0.0f;	 // Width of the vertical scrollbar, Height of the horizontal scrollbar
	float ScrollbarRounding = 0.0f; // Radius of scrollbar corners rounding
	float GrabMinSize = 0.0f;		 // Minimum width/height of a grab box for slider/scrollbar
	float GrabRounding = 0.0f;		 // Radius of grabs corners rounding

	// Tab properties
	float TabRounding = 0.0f;	 // Radius of upper corners of a tab
	float TabBorderSize = 0.0f; // Thickness of border around tabs

	// Font scaling
	float FontScaleDpi = 0.0f; // DPI-aware font scaling factor

	// Padding (stored as arrays for reflectcpp compatibility)
	float WindowPaddingX = 0.0f;	 // Padding within a window (X component)
	float WindowPaddingY = 0.0f;	 // Padding within a window (Y component)
	float FramePaddingX = 0.0f;	 // Padding within a framed rectangle (X component)
	float FramePaddingY = 0.0f;	 // Padding within a framed rectangle (Y component)
	float ItemSpacingX = 0.0f;		 // Horizontal spacing between widgets/lines (X component)
	float ItemSpacingY = 0.0f;		 // Vertical spacing between widgets/lines (Y component)
	float ItemInnerSpacingX = 0.0f; // Horizontal spacing between elements of a composed widget (X
							 // component)
	float ItemInnerSpacingY = 0.0f; // Vertical spacing between elements of a composed widget (Y component)
	float CellPaddingX = 0.0f;		 // Padding within a table cell (X component)
	float CellPaddingY = 0.0f;		 // Padding within a table cell (Y component)

	// Colors - storing the most commonly customized ones
	// Each color is stored as RGBA float values (0.0f to 1.0f)
	// Simple POD struct for reflectcpp compatibility
	// All colors initialized to transparent black - actual values captured from ImGui at runtime
	struct ColorRGBA {
		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
		float a = 0.0f;
	};

	ColorRGBA Text{};
	ColorRGBA TextDisabled{};
	ColorRGBA WindowBg{};
	ColorRGBA ChildBg{};
	ColorRGBA PopupBg{};
	ColorRGBA Border{};
	ColorRGBA BorderShadow{};
	ColorRGBA FrameBg{};
	ColorRGBA FrameBgHovered{};
	ColorRGBA FrameBgActive{};
	ColorRGBA TitleBg{};
	ColorRGBA TitleBgActive{};
	ColorRGBA TitleBgCollapsed{};
	ColorRGBA MenuBarBg{};
	ColorRGBA ScrollbarBg{};
	ColorRGBA ScrollbarGrab{};
	ColorRGBA ScrollbarGrabHovered{};
	ColorRGBA ScrollbarGrabActive{};
	ColorRGBA CheckMark{};
	ColorRGBA SliderGrab{};
	ColorRGBA SliderGrabActive{};
	ColorRGBA Button{};
	ColorRGBA ButtonHovered{};
	ColorRGBA ButtonActive{};
	ColorRGBA Header{};
	ColorRGBA HeaderHovered{};
	ColorRGBA HeaderActive{};
	ColorRGBA Separator{};
	ColorRGBA SeparatorHovered{};
	ColorRGBA SeparatorActive{};
	ColorRGBA ResizeGrip{};
	ColorRGBA ResizeGripHovered{};
	ColorRGBA ResizeGripActive{};
	ColorRGBA Tab{};
	ColorRGBA TabHovered{};
	ColorRGBA TabActive{};
	ColorRGBA TabUnfocused{};
	ColorRGBA TabUnfocusedActive{};
	ColorRGBA PlotLines{};
	ColorRGBA PlotLinesHovered{};
	ColorRGBA PlotHistogram{};
	ColorRGBA PlotHistogramHovered{};
	ColorRGBA TableHeaderBg{};
	ColorRGBA TableBorderStrong{};
	ColorRGBA TableBorderLight{};
	ColorRGBA TableRowBg{};
	ColorRGBA TableRowBgAlt{};
	ColorRGBA TextSelectedBg{};
	ColorRGBA DragDropTarget{};
	ColorRGBA NavHighlight{};
	ColorRGBA NavWindowingHighlight{};
	ColorRGBA NavWindowingDimBg{};
	ColorRGBA ModalWindowDimBg{};

};

} // namespace app
