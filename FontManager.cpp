#include "PCH.hpp"

#include "FontManager.hpp"

FontManager::FontManager()
    : io(nullptr), m_defaultFont(nullptr), m_loadedFonts({nullptr}) {}

FontManager::FontManager(ImGuiIO *io) : FontManager() { this->io = io; }

FontManager::~FontManager() {}

void FontManager::LoadFonts() {

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can
  // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
  // them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
  // need to select the font among multiple.
  // - If the file cannot be loaded, the function will return a nullptr. Please
  // handle those errors in your application (e.g. use an assertion, or display
  // an error and quit).
  // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype
  // for higher quality font rendering.
  // - Read 'docs/FONTS.md' for more instructions and details. If you like the
  // default font but want it to scale better, consider using the 'ProggyVector'
  // from the same author!
  // - Remember that in C/C++ if you want to include a backslash \ in a string
  // literal you need to write a double backslash \\ ! style.FontSizeBase
  // = 20.0f;
  ImFont *font2 =
      io->Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
  IM_ASSERT(font2 != nullptr);
  m_loadedFonts.push_back(font2);

  ImFont *font3 =
      io->Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
  IM_ASSERT(font3 != nullptr);
  m_loadedFonts.push_back(font3);

  ImFont *font4 =
      io->Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
  IM_ASSERT(font4 != nullptr);
  m_loadedFonts.push_back(font4);

  ImFont *font5 =
      io->Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
  IM_ASSERT(font5 != nullptr);
  m_loadedFonts.push_back(font5);

  ImFont *font6 =
      io->Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Arial.TTF");
  IM_ASSERT(font6 != nullptr);
  m_loadedFonts.push_back(font6);

  ImFont *font7 =
      io->Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ARLRDBD.TTF");
  IM_ASSERT(font7 != nullptr);
  m_loadedFonts.push_back(font7);
}

void FontManager::CleanupFonts() {
  for (const auto *font : m_loadedFonts) {

    font = nullptr;
  }
  m_loadedFonts.clear();
}

void FontManager::SetDefaultFont() { io->FontDefault = m_loadedFonts.at(5); }

void FontManager::SetDefaultFont(ImFont *font) { m_defaultFont = font; }

void FontManager::SetDefaultFont(const char *path) {
  ImFont *fontarg = io->Fonts->AddFontFromFileTTF(path);
}
