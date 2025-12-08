#include "raym3/components/Card.h"
#include "raym3/rendering/Renderer.h"
#include "raym3/styles/Theme.h"

#if RAYM3_USE_INPUT_LAYERS
#include "raym3/input/InputLayer.h"
#endif

namespace raym3 {

void CardComponent::Render(Rectangle bounds, CardVariant variant) {
#if RAYM3_USE_INPUT_LAYERS
  // Register this card as a blocking region
  // Even though cards don't handle clicks, they block input to elements beneath
  InputLayerManager::RegisterBlockingRegion(bounds, true);
#endif

  ColorScheme &scheme = Theme::GetColorScheme();
  float cornerRadius = GetCornerRadius();

  if (variant == CardVariant::Elevated) {
    // Elevated: Surface Container Low, Shadow
    Renderer::DrawElevatedRectangle(bounds, cornerRadius, 1,
                                    scheme.surfaceContainerLow);
  } else if (variant == CardVariant::Filled) {
    // Filled: Surface Container Highest, No Shadow
    Renderer::DrawRoundedRectangle(bounds, cornerRadius,
                                   scheme.surfaceContainerHighest);
  } else if (variant == CardVariant::Outlined) {
    // Outlined: Surface color, Outline Border, No Shadow
    Renderer::DrawRoundedRectangle(bounds, cornerRadius, scheme.surface);
    Renderer::DrawRoundedRectangleEx(bounds, cornerRadius, scheme.outline,
                                     1.0f);
  }
}

float CardComponent::GetCornerRadius() {
  return Theme::GetShapeTokens()
      .cornerMedium; // MD3 Card usually uses Medium (12dp) or Large (16dp)
}

} // namespace raym3
