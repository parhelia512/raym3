#pragma once

#include "raym3/types.h"
#include <raylib.h>
#include <cstring>

namespace raym3 {

class ButtonComponent {
public:
    static bool Render(const char* text, Rectangle bounds, ButtonVariant variant = ButtonVariant::Filled);
    
private:
    static ComponentState GetState(Rectangle bounds);
    static Color GetBackgroundColor(ButtonVariant variant, ComponentState state);
    static Color GetTextColor(ButtonVariant variant, ComponentState state);
    static float GetCornerRadius();
};

} // namespace raym3

