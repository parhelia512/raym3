#pragma once

#include "raym3/types.h"
#include <raylib.h>

namespace raym3 {

class SwitchComponent {
public:
    static bool Render(const char* label, Rectangle bounds, bool* checked);
    
private:
    static ComponentState GetState(Rectangle bounds);
    static Rectangle GetSwitchBounds(Rectangle bounds);
};

} // namespace raym3

