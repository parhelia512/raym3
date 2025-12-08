#pragma once

#include "raym3/types.h"
#include <raylib.h>
#include <string>
#include <unordered_map>

namespace raym3 {

class SvgRenderer {
public:
  static void Initialize(const char *basePath);
  static void Shutdown();

  static void DrawIcon(const char *name, Rectangle bounds,
                       IconVariation variation = IconVariation::Filled,
                       Color color = BLACK);

  // Helper to get the variation folder name
  static const char *GetVariationFolder(IconVariation variation);

private:
  static std::string basePath;
  static std::unordered_map<std::string, Texture2D> textureCache;
  static bool autoDetected;

  static std::string GetCacheKey(const char *name, IconVariation variation,
                                 int width, int height);
  static Texture2D LoadSvgTexture(const char *name, IconVariation variation,
                                  int width, int height);
};

} // namespace raym3
