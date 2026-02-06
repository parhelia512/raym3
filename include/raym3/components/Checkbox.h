#pragma once

#include "raym3/types.h"
#include <raylib.h>

namespace raym3 {

struct CheckboxOptions {
  const char *tooltip = nullptr;
  TooltipPlacement tooltipPlacement = TooltipPlacement::Auto;
};

class CheckboxComponent {
public:
    static bool Render(const char* label, Rectangle bounds, bool* checked, const CheckboxOptions* options = nullptr);
    
private:
    static ComponentState GetState(Rectangle bounds);
    static Rectangle GetCheckboxBounds(Rectangle bounds);
};

} // namespace raym3

