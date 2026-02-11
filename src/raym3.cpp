#include "raym3/raym3.h"
#include "raym3/components/Button.h"
#include "raym3/components/Card.h"
#include "raym3/components/Checkbox.h"
#include "raym3/components/Dialog.h"
#include "raym3/components/Menu.h"
#include "raym3/components/RangeSlider.h"
#include "raym3/components/Slider.h"
#include "raym3/components/Switch.h"
#include "raym3/components/TextField.h"
#include "raym3/components/Tooltip.h"
#include "raym3/layout/Container.h"
#include "raym3/styles/Theme.h"
#include "raym3/layout/Layout.h"

#include "raym3/components/Divider.h"
#include "raym3/components/Icon.h"
#include "raym3/components/IconButton.h"
#include "raym3/components/ProgressIndicator.h"
#include "raym3/components/RadioButton.h"
#include "raym3/components/SegmentedButton.h"
#include "raym3/components/Text.h"
#include "raym3/rendering/SvgRenderer.h"
#include <algorithm>
#include <cmath>
#include <vector>

#if RAYM3_USE_INPUT_LAYERS
#include "raym3/input/InputLayer.h"
#include "raym3/input/RenderQueue.h"
#endif

namespace raym3 {

static int s_requestedCursor = MOUSE_CURSOR_DEFAULT;
static bool initialized = false;
static std::vector<Rectangle> s_scissorStack;
static bool s_scissorDebugEnabled = false;
static std::vector<Rectangle> s_scissorDebugRects;

static Rectangle IntersectAndClampScissor(Rectangle requested, Rectangle current) {
  float left = std::max(requested.x, current.x);
  float top = std::max(requested.y, current.y);
  float right = std::min(requested.x + requested.width, current.x + current.width);
  float bottom = std::min(requested.y + requested.height, current.y + current.height);
  if (right <= left || bottom <= top)
    return {0, 0, 0, 0};
  int renderW = std::max(1, GetScreenWidth());
  int renderH = std::max(1, GetScreenHeight());
  int x = (int)std::floor(left);
  int y = (int)std::floor(top);
  int w = (int)std::ceil(right - left);
  int h = (int)std::ceil(bottom - top);
  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }
  if (x + w > renderW) w = renderW - x;
  if (y + h > renderH) h = renderH - y;
  if (w < 1) w = 1;
  if (h < 1) h = 1;
  return {(float)x, (float)y, (float)w, (float)h};
}

void PushScissor(Rectangle bounds) {
  int renderW = std::max(1, GetScreenWidth());
  int renderH = std::max(1, GetScreenHeight());
  Rectangle current = s_scissorStack.empty()
    ? Rectangle{0, 0, (float)renderW, (float)renderH}
    : s_scissorStack.back();
  Rectangle applied = IntersectAndClampScissor(bounds, current);
  if (applied.width < 1 || applied.height < 1)
    return;
  s_scissorStack.push_back(applied);
  BeginScissorMode((int)applied.x, (int)applied.y, (int)applied.width, (int)applied.height);
  if (s_scissorDebugEnabled)
    s_scissorDebugRects.push_back(applied);
}

void PopScissor() {
  if (s_scissorStack.empty())
    return;
  s_scissorStack.pop_back();
  if (s_scissorStack.empty()) {
    EndScissorMode();
    return;
  }
  Rectangle prev = s_scissorStack.back();
  BeginScissorMode((int)prev.x, (int)prev.y, (int)prev.width, (int)prev.height);
}

Rectangle GetCurrentScissorBounds() {
  if (s_scissorStack.empty()) {
    int w = std::max(1, GetScreenWidth());
    int h = std::max(1, GetScreenHeight());
    return {0, 0, (float)w, (float)h};
  }
  return s_scissorStack.back();
}

void SetScissorDebug(bool enabled) { s_scissorDebugEnabled = enabled; }
bool IsScissorDebug() { return s_scissorDebugEnabled; }

void BeginScissor(Rectangle bounds) {
  PushScissor(bounds);
}
static bool darkMode = false;

void Initialize() {
  if (initialized)
    return;

  Theme::Initialize();
  SvgRenderer::Initialize(nullptr); // Auto-detect resource path

#if RAYM3_USE_INPUT_LAYERS
  InputLayerManager::Initialize();
  RenderQueue::Initialize();
#endif

  initialized = true;
}

void Shutdown() {
  if (!initialized)
    return;

  SvgRenderer::Shutdown();
  Theme::Shutdown();
  initialized = false;
}

void RequestCursor(int cursor) {
  s_requestedCursor = cursor;
}

void BeginFrame() {
  if (!initialized)
    Initialize();
  s_requestedCursor = MOUSE_CURSOR_DEFAULT;
  s_scissorDebugRects.clear();
  TextFieldComponent::ResetFieldId();
  SliderComponent::ResetFieldId();
  RangeSliderComponent::ResetFieldId();

#if RAYM3_USE_INPUT_LAYERS
  InputLayerManager::BeginFrame();
  RenderQueue::BeginFrame();
#endif
}

void DrawScissorDebug() {
  if (!s_scissorDebugEnabled || s_scissorDebugRects.empty())
    return;
  while (!s_scissorStack.empty()) {
    s_scissorStack.pop_back();
  }
  EndScissorMode();
  for (const Rectangle& r : s_scissorDebugRects) {
    DrawRectangleRec(r, (Color){0, 255, 0, 35});
    DrawRectangleLinesEx(r, 2.0f, (Color){0, 255, 0, 180});
  }
  s_scissorDebugRects.clear();
}

void EndFrame() {
  SetMouseCursor(s_requestedCursor);

  TooltipManager::Update();

#if RAYM3_USE_INPUT_LAYERS
  RenderQueue::ExecuteRenderQueue();
  InputLayerManager::EndFrame();
#endif
}

#if RAYM3_USE_INPUT_LAYERS
static constexpr int OVERLAY_LAYER_THRESHOLD = 100;

void PushLayer(int zOrder) {
  InputLayerManager::PushLayer(zOrder);
  RenderQueue::PushLayer(zOrder);

  if (zOrder >= OVERLAY_LAYER_THRESHOLD) {
    EndScissorMode();
  }
}

void PopLayer() {
  InputLayerManager::PopLayer();
  RenderQueue::PopLayer();
  
  // If we returned to the base layer (or if we need to restore layout clipping context)
  // We check if there's an active layout scissor and restore it.
  // Note: This logic assumes we mostly use Layers for Overlays on top of Layouts.
  // If Layouts are nested in Layers, this might need refinement.
  
  if (InputLayerManager::GetCurrentLayerId() <= 0 && !s_scissorStack.empty()) {
    Rectangle s = s_scissorStack.back();
    BeginScissorMode((int)s.x, (int)s.y, (int)s.width, (int)s.height);
  }
}
#endif

void SetTheme(bool isDarkMode) {
  darkMode = isDarkMode;
  Theme::SetDarkMode(isDarkMode);
}

bool IsDarkMode() { return darkMode; }

void SetIconBasePath(const char *path) { SvgRenderer::Initialize(path); }

void BeginContainer(Rectangle bounds, LayoutDirection direction) {
  Container::Begin(bounds, direction);
}

void EndContainer() { Container::End(); }

bool Button(const char *text, Rectangle bounds, ButtonVariant variant) {
  return ButtonComponent::Render(text, bounds, variant);
}

bool IconButton(const char *iconName, Rectangle bounds, ButtonVariant variant,
                IconVariation iconVariation) {
  return IconButtonComponent::Render(iconName, bounds, variant, iconVariation);
}

bool TextField(char *buffer, int bufferSize, Rectangle bounds,
               const char *label) {
  return TextFieldComponent::Render(buffer, bufferSize, bounds, label);
}

bool TextField(char *buffer, int bufferSize, Rectangle bounds,
               const char *label, const TextFieldOptions &options) {
  return TextFieldComponent::Render(buffer, bufferSize, bounds, label, options);
}

bool Checkbox(const char *label, Rectangle bounds, bool *checked) {
  return CheckboxComponent::Render(label, bounds, checked);
}

bool Switch(const char *label, Rectangle bounds, bool *checked) {
  return SwitchComponent::Render(label, bounds, checked);
}

bool RadioButton(const char *label, Rectangle bounds, bool selected) {
  return RadioButtonComponent::Render(label, bounds, selected);
}

float Slider(Rectangle bounds, float value, float min, float max,
             const char *label) {
  return SliderComponent::Render(bounds, value, min, max, label);
}

float Slider(Rectangle bounds, float value, float min, float max,
             const char *label, const SliderOptions &options) {
  return SliderComponent::Render(bounds, value, min, max, label, options);
}

std::vector<float> RangeSlider(Rectangle bounds,
                               const std::vector<float> &values, float min,
                               float max, const char *label,
                               const RangeSliderOptions &options) {
  return RangeSliderComponent::Render(bounds, values, min, max, label, options);
}

void Icon(const char *name, Rectangle bounds, IconVariation variation,
          Color color) {
  IconComponent::Render(name, bounds, variation, color);
}

void Text(const char *text, Rectangle bounds, float fontSize, Color color,
          FontWeight weight, TextAlignment alignment) {
  TextComponent::Render(text, bounds, fontSize, color, weight, alignment);
}

void CircularProgressIndicator(Rectangle bounds, float value,
                               bool indeterminate, Color color,
                               float wiggleAmplitude, float wiggleFrequency) {
  ProgressIndicator::Circular(bounds, value, indeterminate, color,
                              wiggleAmplitude, wiggleFrequency);
}

void LinearProgressIndicator(Rectangle bounds, float value, bool indeterminate,
                             Color color, float wiggleAmplitude,
                             float wiggleFrequency) {
  ProgressIndicator::Linear(bounds, value, indeterminate, color,
                            wiggleAmplitude, wiggleFrequency);
}

void Card(Rectangle bounds, CardVariant variant) {
  CardComponent::Render(bounds, variant);
}

bool Dialog(const char *title, const char *message, const char *buttons) {
  return DialogComponent::Render(title, message, buttons);
}

void Menu(Rectangle bounds, const MenuItem *items, int itemCount,
          int *selectedIndex, bool iconOnly) {
  MenuComponent::Render(bounds, items, itemCount, selectedIndex, iconOnly);
}

bool SegmentedButton(Rectangle bounds, const SegmentedButtonItem *items,
                     int itemCount, int *selectedIndex) {
  return SegmentedButtonComponent::Render(bounds, items, itemCount,
                                          selectedIndex);
}

void Divider(Rectangle bounds, DividerVariant variant) {
  DividerComponent::Render(bounds, variant);
}

} // namespace raym3
