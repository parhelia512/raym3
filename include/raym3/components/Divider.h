#pragma once

#include <raylib.h>

namespace raym3 {

enum class DividerVariant { Horizontal, Vertical };

class DividerComponent {
public:
  static void Render(Rectangle bounds,
                     DividerVariant variant = DividerVariant::Horizontal);
};

} // namespace raym3
