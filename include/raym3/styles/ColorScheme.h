#pragma once

#include "raym3/types.h"
#include <raylib.h>

namespace raym3 {

class ColorScheme {
public:
  static ColorScheme Light();
  static ColorScheme Dark();
  static ColorScheme FromSeed(Color seedColor, bool darkMode);

  Color primary;
  Color onPrimary;
  Color primaryContainer;
  Color onPrimaryContainer;
  Color secondary;
  Color onSecondary;
  Color secondaryContainer;
  Color onSecondaryContainer;
  Color tertiary;
  Color onTertiary;
  Color tertiaryContainer;
  Color onTertiaryContainer;
  Color error;
  Color onError;
  Color errorContainer;
  Color onErrorContainer;
  Color surface;
  Color onSurface;
  Color surfaceVariant;
  Color onSurfaceVariant;

  Color surfaceContainerLowest;
  Color surfaceContainerLow;
  Color surfaceContainer;
  Color surfaceContainerHigh;
  Color surfaceContainerHighest;

  Color outline;
  Color outlineVariant;
  Color shadow;
  Color scrim;
  Color inverseSurface;
  Color inverseOnSurface;
  Color inversePrimary;

private:
  static Color Blend(Color color1, Color color2, float ratio);
  static Color AdjustLightness(Color color, float factor);
};

} // namespace raym3
