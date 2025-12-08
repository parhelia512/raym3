#pragma once

#include "raym3/types.h"
#include <raylib.h>

namespace raym3 {

class TextComponent {
public:
  static void Render(const char *text, Rectangle bounds, float fontSize,
                     Color color = BLANK,
                     FontWeight weight = FontWeight::Regular,
                     TextAlignment alignment = TextAlignment::Left);
};

} // namespace raym3
