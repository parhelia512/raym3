#pragma once

#include "raym3/types.h"
#include <raylib.h>

namespace raym3 {

class CheckboxComponent {
public:
    static bool Render(const char* label, Rectangle bounds, bool* checked);
    
private:
    static ComponentState GetState(Rectangle bounds);
    static Rectangle GetCheckboxBounds(Rectangle bounds);
};

} // namespace raym3

