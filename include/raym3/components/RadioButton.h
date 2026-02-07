#pragma once

#include <raylib.h>
#include "raym3/types.h"

namespace raym3 {

struct RadioButtonOptions {
  const char *tooltip = nullptr;
  TooltipPlacement tooltipPlacement = TooltipPlacement::Auto;
};

class RadioButtonComponent {
public:
  static bool Render(const char *label, Rectangle bounds, bool selected, const RadioButtonOptions* options = nullptr);
};

} // namespace raym3
