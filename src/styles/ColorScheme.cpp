#include "raym3/styles/ColorScheme.h"
#include <algorithm>
#include <cmath>

namespace raym3 {

ColorScheme ColorScheme::Light() {
  ColorScheme scheme;

  scheme.primary = {103, 80, 164, 255};
  scheme.onPrimary = {255, 255, 255, 255};
  scheme.primaryContainer = {234, 221, 255, 255};
  scheme.onPrimaryContainer = {33, 0, 93, 255};

  scheme.secondary = {98, 91, 113, 255};
  scheme.onSecondary = {255, 255, 255, 255};
  scheme.secondaryContainer = {232, 222, 248, 255};
  scheme.onSecondaryContainer = {30, 25, 43, 255};

  scheme.tertiary = {125, 82, 96, 255};
  scheme.onTertiary = {255, 255, 255, 255};
  scheme.tertiaryContainer = {255, 216, 228, 255};
  scheme.onTertiaryContainer = {55, 11, 30, 255};

  scheme.error = {186, 26, 26, 255};
  scheme.onError = {255, 255, 255, 255};
  scheme.errorContainer = {255, 218, 214, 255};
  scheme.onErrorContainer = {65, 0, 2, 255};

  scheme.surface = {255, 251, 254, 255};
  scheme.onSurface = {28, 27, 31, 255};
  scheme.surfaceVariant = {231, 224, 236, 255};
  scheme.onSurfaceVariant = {73, 69, 79, 255};

  scheme.surfaceContainerLowest = {255, 255, 255, 255};
  scheme.surfaceContainerLow = {247, 242, 250, 255};
  scheme.surfaceContainer = {243, 237, 247, 255};
  scheme.surfaceContainerHigh = {236, 230, 240, 255};
  scheme.surfaceContainerHighest = {230, 224, 233, 255};

  scheme.outline = {121, 116, 126, 255};
  scheme.outlineVariant = {196, 199, 197, 255};

  scheme.shadow = {0, 0, 0, 255};
  scheme.scrim = {0, 0, 0, 255};

  scheme.inverseSurface = {49, 48, 51, 255};
  scheme.inverseOnSurface = {244, 239, 244, 255};
  scheme.inversePrimary = {208, 188, 255, 255};

  return scheme;
}

ColorScheme ColorScheme::Dark() {
  ColorScheme scheme;

  scheme.primary = {208, 188, 255, 255};
  scheme.onPrimary = {56, 30, 114, 255};
  scheme.primaryContainer = {79, 55, 139, 255};
  scheme.onPrimaryContainer = {234, 221, 255, 255};

  scheme.secondary = {204, 194, 220, 255};
  scheme.onSecondary = {51, 45, 65, 255};
  scheme.secondaryContainer = {74, 68, 88, 255};
  scheme.onSecondaryContainer = {232, 222, 248, 255};

  scheme.tertiary = {239, 184, 200, 255};
  scheme.onTertiary = {73, 37, 50, 255};
  scheme.tertiaryContainer = {99, 59, 72, 255};
  scheme.onTertiaryContainer = {255, 216, 228, 255};

  scheme.error = {255, 180, 171, 255};
  scheme.onError = {105, 0, 5, 255};
  scheme.errorContainer = {147, 0, 10, 255};
  scheme.onErrorContainer = {255, 218, 214, 255};

  scheme.surface = {28, 27, 31, 255};
  scheme.onSurface = {230, 225, 229, 255};
  scheme.surfaceVariant = {73, 69, 79, 255};
  scheme.onSurfaceVariant = {202, 196, 208, 255};

  scheme.surfaceContainerLowest = {15, 13, 19, 255};
  scheme.surfaceContainerLow = {29, 27, 32, 255};
  scheme.surfaceContainer = {33, 31, 38, 255};
  scheme.surfaceContainerHigh = {43, 41, 48, 255};
  scheme.surfaceContainerHighest = {54, 52, 59, 255};

  scheme.outline = {147, 143, 153, 255};
  scheme.outlineVariant = {73, 69, 79, 255};

  scheme.shadow = {0, 0, 0, 255};
  scheme.scrim = {0, 0, 0, 255};

  scheme.inverseSurface = {230, 225, 229, 255};
  scheme.inverseOnSurface = {49, 48, 51, 255};
  scheme.inversePrimary = {103, 80, 164, 255};

  return scheme;
}

ColorScheme ColorScheme::FromSeed(Color seedColor, bool darkMode) {
  if (darkMode) {
    return Dark();
  } else {
    return Light();
  }
}

Color ColorScheme::Blend(Color color1, Color color2, float ratio) {
  ratio = std::clamp(ratio, 0.0f, 1.0f);
  return {
      static_cast<unsigned char>(color1.r * (1.0f - ratio) + color2.r * ratio),
      static_cast<unsigned char>(color1.g * (1.0f - ratio) + color2.g * ratio),
      static_cast<unsigned char>(color1.b * (1.0f - ratio) + color2.b * ratio),
      static_cast<unsigned char>(color1.a * (1.0f - ratio) + color2.a * ratio)};
}

Color ColorScheme::AdjustLightness(Color color, float factor) {
  float r = color.r / 255.0f;
  float g = color.g / 255.0f;
  float b = color.b / 255.0f;

  r = std::pow(r, 1.0f / factor);
  g = std::pow(g, 1.0f / factor);
  b = std::pow(b, 1.0f / factor);

  return {static_cast<unsigned char>(std::clamp(r * 255.0f, 0.0f, 255.0f)),
          static_cast<unsigned char>(std::clamp(g * 255.0f, 0.0f, 255.0f)),
          static_cast<unsigned char>(std::clamp(b * 255.0f, 0.0f, 255.0f)),
          color.a};
}

} // namespace raym3
