#pragma once

#include "raym3/types.h"
#include <raylib.h>

namespace raym3 {

class IconComponent {
public:
  static void Render(const char *name, Rectangle bounds,
                     IconVariation variation = IconVariation::Filled,
                     Color color = BLACK);
};

} // namespace raym3
