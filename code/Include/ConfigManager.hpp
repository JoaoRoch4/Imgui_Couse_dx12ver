// ConfigManager.hpp
// Configuration manager for persistent application settings
// Uses reflectcpp library for JSON serialization/deserialization

#pragma once

#include "PCH.hpp"
#include "Master.hpp"

struct AppConfiguration {
	float clearColorR;
	float clearColorG;
	float clearColorB;
	float clearColorA;
};

class ConfigManager : public Master {
public:
	ConfigManager();

	virtual ~ConfigManager();

	virtual void Open() override;

	virtual void Tick() override;

	virtual void Close() override;

	bool LoadConfiguration();

	bool SaveConfiguration();

	void SetClearColor(float r, float g, float b, float a);

	void GetClearColor(float& r, float& g, float& b, float& a) const;

	ImVec4 GetClearColorAsImVec4() const;

	const std::wstring& GetConfigFilePath() const;

	void SetConfigFilePath(const std::wstring& path);

private:
	AppConfiguration m_config;

	std::wstring m_configFilePath;

	bool m_bConfigLoaded;
};
