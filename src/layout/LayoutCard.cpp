#include "raym3/layout/LayoutCard.h"
#include "raym3/raym3.h"

namespace raym3 {

LayoutCard::CardState LayoutCard::currentCard_ = {};

Rectangle LayoutCard::BeginCard(LayoutStyle style, CardVariant variant) {
  // Begin the container in the layout system
  Rectangle bounds = Layout::BeginContainer(style);

  // Store card state
  currentCard_.bounds = bounds;
  currentCard_.variant = variant;

  // Draw the card background
  if (bounds.width > 0 && bounds.height > 0) {
    ColorScheme &scheme = Theme::GetColorScheme();
    Color cardColor;
    float elevation = 0;

    switch (variant) {
    case CardVariant::Elevated:
      cardColor = scheme.surfaceContainerLow;
      elevation = 1.0f;
      break;
    case CardVariant::Filled:
      cardColor = scheme.surfaceContainerHighest;
      elevation = 0;
      break;
    case CardVariant::Outlined:
      cardColor = scheme.surface;
      elevation = 0;
      break;
    }

    // Draw shadow for elevated cards
    if (elevation > 0) {
      Color shadowColor = {0, 0, 0, 30};
      DrawRectangleRounded(
          {bounds.x + 2, bounds.y + 2, bounds.width, bounds.height}, 0.15f, 10,
          shadowColor);
    }

    // Draw card background
    DrawRectangleRounded(bounds, 0.15f, 10, cardColor);

    // Draw outline for outlined variant
    if (variant == CardVariant::Outlined) {
#ifdef PLATFORM_ANDROID
      DrawRectangleRoundedLinesEx(bounds, 0.15f, 10, 1.0f, scheme.outlineVariant);
#else
      DrawRectangleRoundedLines(bounds, 0.15f, 10, scheme.outlineVariant);
#endif
    }
  }

  // Return bounds with padding for content
  return {bounds.x + 16, bounds.y + 16, bounds.width - 32, bounds.height - 32};
}

void LayoutCard::EndCard() {
  // End the layout container
  Layout::EndContainer();

  // Reset card state
  currentCard_ = {};
}

} // namespace raym3
