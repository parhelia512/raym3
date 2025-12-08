#pragma once

#include "raym3/types.h"
#include <raylib.h>
#include <string>
#include <unordered_map>

namespace raym3 {

struct FontKey {
  FontWeight weight;
  FontStyle style;
  int size;

  bool operator==(const FontKey &other) const {
    return weight == other.weight && style == other.style && size == other.size;
  }
};

struct FontKeyHash {
  std::size_t operator()(const FontKey &key) const {
    return std::hash<int>()(static_cast<int>(key.weight)) ^
           (std::hash<int>()(static_cast<int>(key.style)) << 1) ^
           (std::hash<int>()(key.size) << 2);
  }
};

class FontManager {
public:
  static void Initialize();
  static void Shutdown();

  static Font LoadFont(FontWeight weight = FontWeight::Regular,
                       FontStyle style = FontStyle::Normal, int size = 16);
  static Font LoadCustomFont(const std::string &path, int size);

  static void UnloadFont(Font font);

  static Font GetDefaultFont() { return defaultFont_; }

private:
  static Font LoadRobotoFont(FontWeight weight, FontStyle style, int size);
  static std::unordered_map<FontKey, Font, FontKeyHash> fontCache_;
  static Font defaultFont_;
  static bool initialized_;
};

} // namespace raym3
