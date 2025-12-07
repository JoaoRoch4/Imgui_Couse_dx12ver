#pragma once

#include "PCH.hpp"

class FontManager {
protected:
    FontManager();

public:
    FontManager(ImGuiIO *io);
    ~FontManager();

    void LoadFonts();
    void CleanupFonts();

    void SetDefaultFont();
    void SetDefaultFont(ImFont *font);
    void SetDefaultFont(const char *path);

private:
    ImGuiIO *io;
    ImFont *m_defaultFont;
    std::vector<ImFont *> m_loadedFonts;
};
