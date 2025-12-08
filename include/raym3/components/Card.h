#pragma once

#include "raym3/types.h"
#include <raylib.h>

namespace raym3 {

class CardComponent {
public:
    static void Render(Rectangle bounds, CardVariant variant = CardVariant::Elevated);
    
private:
    static float GetCornerRadius();
};

} // namespace raym3
