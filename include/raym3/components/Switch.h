#pragma once

#include "raym3/types.h"
#include <raylib.h>

namespace raym3 {

struct SwitchOptions {
  const char *tooltip = nullptr;
  TooltipPlacement tooltipPlacement = TooltipPlacement::Auto;
};

class SwitchComponent {
public:
    static bool Render(const char* label, Rectangle bounds, bool* checked, const SwitchOptions* options = nullptr);
    
private:
    static ComponentState GetState(Rectangle bounds);
    static Rectangle GetSwitchBounds(Rectangle bounds);
};

} // namespace raym3

