#include "raym3/components/Slider.h"
#include "raym3/components/Dialog.h"
#include "raym3/rendering/Renderer.h"
#include "raym3/styles/Theme.h"
#include <algorithm>
#include <cstdio>
#include <raylib.h>

#if RAYM3_USE_INPUT_LAYERS
#include "raym3/input/InputLayer.h"
#endif

namespace raym3 {

// Removed anonymous namespace and its static variables.
// Moved to static member variables of SliderComponent.

static int activeFieldId_ = -1;
static int currentFieldId_ = 0;

void SliderComponent::ResetFieldId() { currentFieldId_ = 0; }

static ComponentState GetSliderState(Rectangle bounds, Rectangle thumbRect) {
  Vector2 mousePos = GetMousePosition();
  // Check both bounds and specific hit areas
  Rectangle hitRect = {bounds.x, thumbRect.y - 10, bounds.width,
                       thumbRect.height + 20};
#if RAYM3_USE_INPUT_LAYERS
  bool canProcessInput = InputLayerManager::ShouldProcessMouseInput(bounds);
  bool isHovered = canProcessInput && CheckCollisionPointRec(mousePos, hitRect);
#else
  bool isHovered = CheckCollisionPointRec(mousePos, hitRect);
#endif
  bool isPressed = isHovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);

  // We can't easily check if *this* specific slider is dragging here without
  // fieldId But we can check if *any* slider is dragging (activeFieldId_ != -1)
  // Ideally GetState should take fieldId or we rely on the Render loop to set
  // state. For now, let's just rely on isPressed/isHovered and let Render
  // override if dragging.

  if (activeFieldId_ != -1) {
    // If dragging, we might be the one.
    // Render handles the specific override.
    // Here we just return Default if someone else is dragging?
    // Or just standard logic.
  }

  if (isPressed)
    return ComponentState::Pressed;
  if (isHovered)
    return ComponentState::Hovered;
  return ComponentState::Default;
}

float SliderComponent::Render(Rectangle bounds, float value, float min,
                              float max, const char *label) {
  ColorScheme &scheme = Theme::GetColorScheme();

  // MD3 measurements
  float trackHeight = 24.0f; // Thick pill shape
  float thumbWidth = 4.0f;   // User requested hard minimum 4px
  float thumbHeight = 32.0f;

  Rectangle trackBounds = GetTrackBounds(bounds);

  // Calculate normalized value and splitX early
  float normalizedValue = (value - min) / (max - min);
  normalizedValue = std::clamp(normalizedValue, 0.0f, 1.0f);
  float splitX = trackBounds.x + (trackBounds.width * normalizedValue);

  Rectangle thumbRect = {splitX - thumbWidth / 2.0f,
                         trackBounds.y +
                             (trackBounds.height - thumbHeight) / 2.0f,
                         thumbWidth, thumbHeight};

  // Check interaction
  ComponentState state = GetSliderState(bounds, thumbRect);

  bool inputBlocked =
      DialogComponent::IsActive() && !DialogComponent::IsRendering();
  if (inputBlocked) {
    state = ComponentState::Default;
  }

  // Handle input
  int fieldId = currentFieldId_++;
  bool isDraggingThis = (activeFieldId_ == fieldId);

  Vector2 mousePos = GetMousePosition();
  // Expand hit target for easier interaction
  Rectangle hitRect = {trackBounds.x, trackBounds.y - 10, trackBounds.width,
                       trackBounds.height + 20};
#if RAYM3_USE_INPUT_LAYERS
  bool canProcessInput = InputLayerManager::ShouldProcessMouseInput(bounds);
  bool mouseOverHit = canProcessInput && CheckCollisionPointRec(mousePos, hitRect);
  bool mouseDown = canProcessInput && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
  bool mousePressed = canProcessInput && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
  bool mouseReleased = canProcessInput && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
#else
  bool mouseOverHit = CheckCollisionPointRec(mousePos, hitRect);
  bool mouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
  bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
  bool mouseReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
#endif

  if (!inputBlocked) {
    if (mousePressed && mouseOverHit) {
      activeFieldId_ = fieldId;
      isDraggingThis = true;
#if RAYM3_USE_INPUT_LAYERS
      InputLayerManager::ConsumeInput();
#endif
    }

    if (mouseReleased && isDraggingThis) {
      activeFieldId_ = -1;
      isDraggingThis = false;
    }

    if (isDraggingThis) {
      state = ComponentState::Pressed; // Override state while dragging
      if (mouseDown) {
        float normalized = GetValueFromPosition(trackBounds, mousePos.x);
        value = min + normalized * (max - min);
        value = std::clamp(value, min, max);
        // Recalculate normalized for drawing
        normalizedValue = (value - min) / (max - min);
        splitX = trackBounds.x + (trackBounds.width * normalizedValue);
        thumbRect.x = splitX - thumbWidth / 2.0f;
#if RAYM3_USE_INPUT_LAYERS
        InputLayerManager::ConsumeInput();
#endif
      } else {
        activeFieldId_ = -1;
        isDraggingThis = false;
      }
    }
  } else {
    // Force stop dragging if blocked
    if (isDraggingThis) {
      activeFieldId_ = -1;
      isDraggingThis = false;
    }
  }

  // Draw Tracks
  // "The taller the slider gets the less circular the track should get"
  // Use a fixed max radius (e.g., 6.0f) so that as height increases, it looks
  // more rectangular. For small heights (e.g. 4.0f), it will still be a pill
  // (2.0f radius).
  float cornerRadius = std::min(trackHeight / 2.0f, 6.0f);

  // 1. Active Track (Left side)
  if (normalizedValue > 0.0f) {
    Rectangle activeRect = {trackBounds.x, trackBounds.y,
                            trackBounds.width * normalizedValue, trackHeight};

    // Draw rounded start
    Vector2 leftCircleCenter = {trackBounds.x + cornerRadius,
                                trackBounds.y + cornerRadius};
    // Draw top-left and bottom-left quadrants or just a circle?
    // Since we want "less circular", we should use DrawRoundedRectangle logic
    // but clipped? Raylib's DrawRoundedRectangle handles all corners. We want
    // Left corners rounded, Right corners flat (at split). A simple way: Draw
    // rounded rect for the whole active part, but if it's not 100%, we need the
    // right side flat. Or: Draw a rounded rect of full width, then clip? No.
    // Construct it:
    // 1. Circle/Arc at left.
    // 2. Rect to right.
    // Actually, with a small fixed radius, drawing a circle at the corner is
    // not enough, we need the corner arc. Let's approximate: Draw a rounded
    // rect that extends slightly past the split, then clip? Or just draw a
    // rounded rect from start to split. If split is far enough, the right
    // corners will be rounded too, which is wrong. Correct approach for
    // "Rounded Left, Flat Right": Draw a Rectangle from (x+radius) to (splitX).
    // Draw a Rectangle from (x) to (x+radius) with rounded corners? No.
    // Draw a rounded rectangle of width (splitX - x), then cover the right
    // corners? Draw a rounded rectangle from x to splitX+radius, then clip to
    // splitX? Let's use the Scissor mode for cleanliness.

    BeginScissorMode((int)trackBounds.x, (int)trackBounds.y,
                     (int)(trackBounds.width * normalizedValue),
                     (int)trackHeight);
    Renderer::DrawRoundedRectangle(
        trackBounds, cornerRadius,
        scheme.primary); // Draw full active track, clipped to current value
    EndScissorMode();
  }

  // 2. Inactive Track (Right side)
  if (normalizedValue < 1.0f) {
    // Clip from splitX to end
    int startX = (int)(trackBounds.x + trackBounds.width * normalizedValue);
    int width = (int)(trackBounds.width * (1.0f - normalizedValue));

    BeginScissorMode(startX, (int)trackBounds.y, width, (int)trackHeight);
    Renderer::DrawRoundedRectangle(
        trackBounds, cornerRadius,
        scheme.surfaceVariant); // Draw full inactive track, clipped
    EndScissorMode();

    // Draw End Dot
    // "End of track circle should be 3 px wide it should be a small dot in
    // primary colors" "Thumb should snap to it at the 100%" We draw it at the
    // visual end of the track. Position: Right edge - padding? Or centered in
    // the "rounded" area? If the track is 24px high, centered y. x:
    // trackBounds.x + trackBounds.width - (something). If thumb snaps to it,
    // and thumb is at splitX. At 100%, splitX = right edge. So dot should be at
    // right edge? Let's place it at `right - 6px` (center of the 12px padding
    // area usually). Or just `right - 4px`.
    float dotRadius = 1.5f; // 3px wide
    Vector2 dotPos = {trackBounds.x + trackBounds.width - 6.0f,
                      trackBounds.y + trackHeight / 2.0f};
    DrawCircleV(dotPos, dotRadius, scheme.primary);
  }

  // Draw Handle (Vertical Line)

  // Gap Mask (Surface color)
  // User requested "surface color bar on each side" to give the illusion of a
  // gap. "Instead of an outline we should just make the thumb round and add a
  // surface color bar on each side" This implies a flat vertical cut, not a
  // rounded one. We draw a rectangle behind the thumb to mask the track.
  float gapSize = 6.0f;
  Rectangle maskRect = {thumbRect.x - gapSize,
                        thumbRect.y, // Match thumb y
                        thumbRect.width + (gapSize * 2), thumbRect.height};
  DrawRectangleRec(maskRect, scheme.surface);

  // Draw thumb (Primary color)
  // "inner thumb ... should be rounded"
  Renderer::DrawRoundedRectangle(thumbRect, thumbWidth / 2.0f, scheme.primary);

  if (label) {
    Vector2 labelPos = {bounds.x, bounds.y};
    Renderer::DrawText(label, labelPos, 14.0f, scheme.onSurface,
                       FontWeight::Regular);
  }

  return value;
}

Rectangle SliderComponent::GetTrackBounds(Rectangle bounds) {
  float trackHeight = 24.0f; // Updated to match Render
  float labelHeight = 20.0f; // Approximate space for label

  // If label is present, track is pushed down. Logic in Render handles label
  // drawing at top. Assuming bounds includes label space? The original code put
  // label at bounds.y. Let's put track below label.

  float yOffset = 24.0f;

  return {bounds.x,
          bounds.y + yOffset + (bounds.height - yOffset - trackHeight) / 2.0f,
          bounds.width, trackHeight};
}

float SliderComponent::GetValueFromPosition(Rectangle trackBounds, float x) {
  float normalized = (x - trackBounds.x) / trackBounds.width;
  normalized = std::clamp(normalized, 0.0f, 1.0f);
  return normalized;
}

} // namespace raym3
