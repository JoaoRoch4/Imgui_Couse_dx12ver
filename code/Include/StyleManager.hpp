// StyleManager.hpp
// Style manager for persistent ImGui style settings
// Uses reflectcpp library for JSON serialization/deserialization

#pragma once

#include "PCH.hpp"
#include "Master.hpp"
#include "StyleConfiguration.hpp"

namespace app {



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
