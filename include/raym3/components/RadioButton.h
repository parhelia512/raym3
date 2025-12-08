#pragma once

#include <raylib.h>

namespace raym3 {

class RadioButtonComponent {
public:
  static bool Render(const char *label, Rectangle bounds, bool selected);
};

} // namespace raym3
