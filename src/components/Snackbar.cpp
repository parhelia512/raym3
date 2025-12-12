#include "raym3/components/Snackbar.h"
#include "raym3/layout/Layout.h"
#include "raym3/rendering/Renderer.h"
#include "raym3/styles/Theme.h"
#include <raylib.h>

#if RAYM3_USE_INPUT_LAYERS
#include "raym3/input/InputLayer.h"
#endif

namespace raym3 {

bool SnackbarComponent::isOpen_ = false;
float SnackbarComponent::timer_ = 0.0f;
std::string SnackbarComponent::message_ = "";
SnackbarAction SnackbarComponent::action_ = {nullptr, nullptr};

void SnackbarComponent::Show(const char *message, float duration,
                             SnackbarAction action) {
  message_ = message ? message : "";
  timer_ = duration;
  action_ = action;
  isOpen_ = true;
}

void SnackbarComponent::Hide() { isOpen_ = false; }

Rectangle SnackbarComponent::GetBounds(Rectangle screenBounds) {
  // MD3 Specs:
  // Mobile: 100% width usually (or minimal margins)
  // Desktop/Tablet: min-width 344dp, max-width 672dp.
  // We'll stick to a centered desktop-like look for now as `raym3` seems
  // desktop-oriented.
  float padding = 24.0f;
  float width = 344.0f;
  float height = 48.0f;

  return {screenBounds.x + (screenBounds.width - width) / 2.0f,
          screenBounds.y + screenBounds.height - height - padding, width,
          height};
}

void SnackbarComponent::Render(Rectangle screenBounds) {
  if (!isOpen_)
    return;

  float dt = GetFrameTime();
  if (timer_ > 0) {
    timer_ -= dt;
    if (timer_ <= 0) {
      isOpen_ = false;
      return;
    }
  }

  Rectangle bounds = GetBounds(screenBounds);

  // Register debug rect
  Layout::RegisterDebugRect(bounds);

#if RAYM3_USE_INPUT_LAYERS
  // Layer 500: Above normal content, below Dialogs/Modals
  InputLayerManager::PushLayer(500);
  // Register blocking region so clicks on the snackbar don't pass through
  InputLayerManager::RegisterBlockingRegion(bounds, true);
#endif

  ColorScheme &scheme = Theme::GetColorScheme();

  // Container - Inverse Surface
  // Elevation 2 is typical for snackbars (lighter shadow) or 3 (darker)
  // Corner Extra Small (4dp)
  float cornerRadius = 4.0f; // Theme::GetShapeTokens().cornerExtraSmall
  Renderer::DrawElevatedRectangle(bounds, cornerRadius, 3,
                                  scheme.inverseSurface);

  // Text - Inverse On Surface
  float textPaddingX = 16.0f;
  // Measure text to vertically center it exactly
  Vector2 textSize =
      Renderer::MeasureText(message_.c_str(), 14.0f, FontWeight::Regular);
  Vector2 textPos = {bounds.x + textPaddingX,
                     bounds.y + (bounds.height - textSize.y) / 2.0f};

  Renderer::DrawText(message_.c_str(), textPos, 14.0f, scheme.inverseOnSurface,
                     FontWeight::Regular);

  // Action Button
  if (action_.label) {
    // Custom button rendering to match "Inverse Primary" requirement without
    // needing specialized Button variants in the core library just yet.
    Color actionColor = scheme.inversePrimary;
    const char *text = action_.label;

    Vector2 btnSize = Renderer::MeasureText(text, 14.0f, FontWeight::Medium);
    float btnExPadding = 12.0f;
    float btnWidth = btnSize.x + btnExPadding * 2;
    float btnHeight = 36.0f;

    // Place button at the end (right side)
    Rectangle btnBounds = {
        bounds.x + bounds.width - btnWidth - 8.0f, // 8dp margin from right
        bounds.y + (bounds.height - btnHeight) / 2.0f, btnWidth, btnHeight};

    Layout::RegisterDebugRect(btnBounds);

    bool clicked = false;
    Vector2 mousePos = GetMousePosition();

    // Interaction Check
    bool isHovered = CheckCollisionPointRec(mousePos, btnBounds);
#if RAYM3_USE_INPUT_LAYERS
    // If using layers, ensure we have input focus
    if (isHovered && InputLayerManager::BeginInputCapture(btnBounds, true)) {
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        clicked = true;
      }
      // Draw Hover Overlay
      Color hoverColor = ColorAlpha(actionColor, 0.08f);
      DrawRectangleRounded(btnBounds, 0.5f, 4, hoverColor);
    }
#else
    if (isHovered) {
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        clicked = true;
      }
      // Draw Hover Overlay
      Color hoverColor = ColorAlpha(actionColor, 0.08f);
      DrawRectangleRounded(btnBounds, 0.5f, 4, hoverColor);
    }
#endif

    // Draw Button Text
    Vector2 btnTextPos = {btnBounds.x + btnExPadding,
                          btnBounds.y + (btnBounds.height - btnSize.y) / 2.0f};
    Renderer::DrawText(text, btnTextPos, 14.0f, actionColor,
                       FontWeight::Medium);

    if (clicked) {
      if (action_.callback) {
        action_.callback();
      }
      isOpen_ = false;
    }
  }

#if RAYM3_USE_INPUT_LAYERS
  InputLayerManager::PopLayer();
#endif
}

} // namespace raym3
