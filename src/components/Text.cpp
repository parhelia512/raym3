#include "raym3/components/Text.h"
#include "raym3/rendering/Renderer.h"
#include "raym3/styles/Theme.h"
#include <cstring>

namespace raym3 {

void TextComponent::Render(const char *text, Rectangle bounds, float fontSize,
                           Color color, FontWeight weight,
                           TextAlignment alignment) {
  if (!text || strlen(text) == 0)
    return;

  Color finalColor = color;
  if (color.a == 0) {
    finalColor = Theme::GetColorScheme().onSurface;
  }

  // Measure text to handle alignment
  Vector2 textSize = Renderer::MeasureText(text, fontSize, weight);

  Vector2 position = {bounds.x, bounds.y};

  // Vertical alignment: Center by default for now, or top?
  // Usually Text component draws from top-left of bounds.
  // Let's center vertically if bounds height > text height?
  // Or just stick to top-left + alignment.
  // Standard UI text usually aligns within the box.

  // Let's implement alignment relative to bounds.

  // Vertical centering
  position.y = bounds.y + (bounds.height - textSize.y) / 2.0f;

  switch (alignment) {
  case TextAlignment::Left:
    position.x = bounds.x;
    break;
  case TextAlignment::Center:
    position.x = bounds.x + (bounds.width - textSize.x) / 2.0f;
    break;
  case TextAlignment::Right:
    position.x = bounds.x + bounds.width - textSize.x;
    break;
  }

  Renderer::DrawText(text, position, fontSize, color, weight);
}

} // namespace raym3
