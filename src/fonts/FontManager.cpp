#include "raym3/fonts/FontManager.h"
#include "raym3/config.h"
#include "EmbeddedFonts.h"
#include <algorithm>
#include <filesystem>

namespace raym3 {

std::unordered_map<FontKey, Font, FontKeyHash> FontManager::fontCache_;
Font FontManager::defaultFont_ = {0};
bool FontManager::initialized_ = false;
void FontManager::Initialize() {
  if (initialized_)
    return;

  defaultFont_ = LoadFont(FontWeight::Regular, FontStyle::Normal, 16);
  initialized_ = true;
}

void FontManager::Shutdown() {
  for (auto &[key, font] : fontCache_) {
    if (font.texture.id != 0) {
      UnloadFont(font);
    }
  }
  fontCache_.clear();

  // defaultFont_ is a copy of a font in fontCache_, so it's already unloaded.
  defaultFont_ = {0};

  initialized_ = false;
}

Font FontManager::LoadFont(FontWeight weight, FontStyle style, int size) {
  FontKey key{weight, style, size};

  auto it = fontCache_.find(key);
  if (it != fontCache_.end()) {
    return it->second;
  }

  Font font = LoadRobotoFont(weight, style, size);
  if (font.texture.id != 0) {
    fontCache_[key] = font;
  }

  return font;
}

Font FontManager::LoadCustomFont(const std::string &path, int size) {
  std::string resolvedPath = path;
  
  if (!std::filesystem::path(path).is_absolute()) {
    std::vector<std::string> searchPaths = {
      std::string(RAYM3_RESOURCE_DIR) + "/fonts/" + path,
      std::string(RAYM3_RESOURCE_DIR) + "/fonts/Roboto/" + path,
      std::string(RAYM3_RESOURCE_DIR) + "/" + path,
      "./resources/fonts/" + path,
      "./resources/fonts/Roboto/" + path,
      "./raym3/resources/fonts/" + path,
      path
    };
    
    for (const auto& testPath : searchPaths) {
      if (std::filesystem::exists(testPath)) {
        resolvedPath = testPath;
        break;
      }
    }
  }
  
  if (!std::filesystem::exists(resolvedPath)) {
    return {0};
  }

  Font font = LoadFontEx(resolvedPath.c_str(), size, nullptr, 0);
  return font;
}

void FontManager::UnloadFont(Font font) {
  if (font.texture.id != 0) {
    ::UnloadFont(font);
  }
}

Font FontManager::LoadRobotoFont(FontWeight weight, FontStyle style, int size) {
  // Map weights to available embedded fonts
  unsigned char *fontData = nullptr;
  unsigned int fontDataLen = 0;

  if (weight == FontWeight::Bold || weight == FontWeight::Black) {
    fontData = Roboto_v3_012_hinted_static_Roboto_Bold_ttf;
    fontDataLen = Roboto_v3_012_hinted_static_Roboto_Bold_ttf_len;
  } else {
    fontData = Roboto_v3_012_hinted_static_Roboto_Regular_ttf;
    fontDataLen = Roboto_v3_012_hinted_static_Roboto_Regular_ttf_len;
  }

  Font font =
      LoadFontFromMemory(".ttf", fontData, fontDataLen, size, nullptr, 0);
  return font;
}

} // namespace raym3
