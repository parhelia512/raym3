#include "raym3/components/Divider.h"
#include "raym3/styles/Theme.h"

namespace raym3 {

void DividerComponent::Render(Rectangle bounds, DividerVariant variant) {
  ColorScheme &scheme = Theme::GetColorScheme();
  Color color = scheme.outlineVariant;
  float thickness = 1.0f;

  if (variant == DividerVariant::Horizontal) {
    // Draw horizontal line centered vertically in bounds
    float y = bounds.y + bounds.height / 2.0f;
    DrawLineEx({bounds.x, y}, {bounds.x + bounds.width, y}, thickness, color);
  } else {
    // Draw vertical line centered horizontally in bounds
    float x = bounds.x + bounds.width / 2.0f;
    DrawLineEx({x, bounds.y}, {x, bounds.y + bounds.height}, thickness, color);
  }
}

} // namespace raym3
