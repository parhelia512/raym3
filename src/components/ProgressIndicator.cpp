#include "raym3/components/ProgressIndicator.h"
#include "raym3/rendering/Renderer.h"
#include "raym3/styles/Theme.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>

namespace raym3 {

// Simple Cubic Bezier easing helper (1D)
// For full 2D Bezier we'd need a solver, but for animation we can approximate
// or use standard easing functions. MD3 uses specific beziers. Circular:
//   Head: CubicBezier(0.4, 0.0, 0.2, 1.0)
//   Tail: CubicBezier(0.4, 0.0, 0.2, 1.0)
// Linear:
//   Line 1: CubicBezier(0.2, 0.0, 0.8, 1.0) ? (Standard)
// Let's use standard EaseInOutCubic for now as a good approximation.

static float EaseInOutCubic(float t) {
  return t < 0.5f ? 4.0f * t * t * t
                  : 1.0f - powf(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

static float EaseOutCubic(float t) { return 1.0f - powf(1.0f - t, 3.0f); }

static float EaseInCubic(float t) { return t * t * t; }

// MD3 Standard Easing (approximate)
static float EaseStandard(float t) {
  // CubicBezier(0.2, 0.0, 0.0, 1.0)
  // This is close to EaseOutCubic but sharper start.
  return EaseOutCubic(t);
}

// Helper to draw a wiggly line
static void DrawWigglyLine(Vector2 start, Vector2 end, float amplitude,
                           float frequency, float phase, Color color,
                           float thick) {
  float length = Vector2Distance(start, end);
  if (length <= 0)
    return;

  Vector2 dir = Vector2Normalize(Vector2Subtract(end, start));
  Vector2 normal = {-dir.y, dir.x};

  // Adaptive segments: roughly 1 segment per 2 pixels
  int segments = (int)(length / 2.0f);
  if (segments < 2)
    segments = 2;

  // If amplitude is tiny, just draw a straight line
  if (amplitude < 0.01f) {
    DrawLineEx(start, end, thick, color);
    DrawCircleV(start, thick / 2.0f, color);
    DrawCircleV(end, thick / 2.0f, color);
    return;
  }

  Vector2 prevPos = start;

  for (int i = 1; i <= segments; i++) {
    float t = (float)i / segments;
    float dist = t * length;

    // Sine wave offset
    float offset = sinf(dist * frequency + phase) * amplitude;

    // Taper amplitude at ends for smooth connection
    float taper = 1.0f;
    if (t < 0.1f)
      taper = t / 0.1f;
    else if (t > 0.9f)
      taper = (1.0f - t) / 0.1f;
    offset *= taper;

    Vector2 center = Vector2Add(Vector2Add(start, Vector2Scale(dir, dist)),
                                Vector2Scale(normal, offset));

    DrawLineEx(prevPos, center, thick, color);
    DrawCircleV(center, thick / 2.0f, color); // Smooth joins
    prevPos = center;
  }
}

// Helper to draw a wiggly arc
static void DrawWigglyArc(Vector2 center, float radius, float startAngle,
                          float endAngle, float amplitude, float frequency,
                          float phase, Color color, float thick) {
  // Normalize angles and calculate sweep
  float sweep = endAngle - startAngle;

  // Calculate approximate arc length
  float arcLength = fabsf(sweep * DEG2RAD * radius);

  // Adaptive segments: roughly 1 segment per 2 pixels of arc length
  int segments = (int)(arcLength / 2.0f);
  if (segments < 4)
    segments = 4;

  // If amplitude is tiny, draw a normal ring arc (approximated)
  if (amplitude < 0.01f) {
    DrawRing(center, radius - thick / 2.0f, radius + thick / 2.0f, startAngle,
             endAngle, 0, color);
    // Caps are tricky with DrawRing, but let's stick to the wiggly path for
    // consistency if needed, or just assume standard drawing is better.
    // Actually, let's fallthrough to the loop with amplitude 0 for consistency
    // unless we want perfect straightness.
    // The loop is fine for 0 amplitude if segments are high enough.
  }

  Vector2 prevPos;
  bool first = true;

  for (int i = 0; i <= segments; i++) {
    float t = (float)i / segments;
    float angle = startAngle + t * sweep;
    float rad = angle * DEG2RAD;

    // Distance along the arc needs to be calculated correctly for continuous
    // phase For a circular path, distance = angle * radius (in radians)
    float currentDist = angle * DEG2RAD * radius;

    // Sine wave offset (radial)
    float offset = sinf(currentDist * frequency + phase) * amplitude;

    // Taper
    float taper = 1.0f;
    if (t < 0.1f)
      taper = t / 0.1f;
    else if (t > 0.9f)
      taper = (1.0f - t) / 0.1f;
    offset *= taper;

    float r = radius + offset;
    float x = center.x + cosf(rad) * r;
    float y = center.y + sinf(rad) * r;
    Vector2 currentPos = {x, y};

    if (!first) {
      DrawLineEx(prevPos, currentPos, thick, color);
      DrawCircleV(currentPos, thick / 2.0f, color); // Smooth joins
    } else {
      first = false;
    }
    prevPos = currentPos;
  }
}

void ProgressIndicator::Circular(Rectangle bounds, float value,
                                 bool indeterminate, Color color,
                                 float wiggleAmplitude,
                                 float wiggleWavelength) {
  ColorScheme &scheme = Theme::GetColorScheme();
  Color activeColor = (color.a == 0) ? scheme.primary : color;

  Vector2 center = {bounds.x + bounds.width / 2.0f,
                    bounds.y + bounds.height / 2.0f};
  float radius = std::min(bounds.width, bounds.height) / 2.0f;
  // Reduce radius by half thickness so stroke stays inside/centered
  float thickness = 4.0f;
  radius -= thickness / 2.0f;

  // Wiggle parameters
  float amplitude = wiggleAmplitude;

  // Calculate frequency from wavelength
  // Frequency = 2*PI / Wavelength
  float frequency = 0.0f;
  if (wiggleWavelength > 0.1f) {
    frequency = 2.0f * PI / wiggleWavelength;
  }

  // Draw Track
  Color trackColor = scheme.surfaceContainerHighest;
  // Use a high segment count for a smooth circle (e.g., 60-120 depending on
  // size) Let's use 128 to be safe and smooth
  DrawRing(center, radius - thickness / 2.0f, radius + thickness / 2.0f, 0.0f,
           360.0f, 128, trackColor);

  float phase = (float)GetTime() * 10.0f; // Animated wiggle

  if (indeterminate) {
    double time = GetTime();
    float cycleDuration = 1.333f;
    float t = (float)fmod(time, cycleDuration) / cycleDuration;

    // MD3 circular indicator animation logic
    float headT = t;
    float tailT = t;

    // These eases should ideally map 0..1 to an angle
    // Standard android implementation rotates the whole thing plus
    // expands/contracts the arc

    // Simplification for brevity while keeping effect similar:
    // Expand arc then contract arc while rotating

    float arcLength;
    float rotation = (float)(time * 360.0f / cycleDuration);
    // Actually simpler:
    // "The indicator rotates 360 degrees every 1333ms"
    // "Arc grows from 10 to 270 degrees"

    // We already have some logic, let's keep it but fix drawing
    float expansion = sinf(t * PI);         // 0 -> 1 -> 0
    arcLength = 10.0f + 260.0f * expansion; // 10 to 270

    // Add extra rotation to simulate the 'catching up' look
    float extraRot = 0.0f;
    // This is a rough approx of the complex MD3 easing, good enough for game UI

    DrawWigglyArc(center, radius, rotation, rotation + arcLength, amplitude,
                  frequency, phase, activeColor, thickness);

  } else {
    float startAngle = -90.0f;
    float endAngle = startAngle + (value * 360.0f);
    DrawWigglyArc(center, radius, startAngle, endAngle, amplitude, frequency,
                  phase, activeColor, thickness);
  }
}

void ProgressIndicator::Linear(Rectangle bounds, float value,
                               bool indeterminate, Color color,
                               float wiggleAmplitude, float wiggleWavelength) {
  ColorScheme &scheme = Theme::GetColorScheme();
  Color activeColor = (color.a == 0) ? scheme.primary : color;
  Color trackColor = scheme.surfaceContainerHighest;

  // Draw Track (Straight)
  Renderer::DrawRoundedRectangle(bounds, bounds.height / 2.0f, trackColor);

  // Wiggle parameters
  float amplitude = wiggleAmplitude;

  // Calculate frequency from wavelength
  float frequency = 0.0f;
  if (wiggleWavelength > 0.1f) {
    frequency = 2.0f * PI / wiggleWavelength;
  }

  float phase = (float)GetTime() * 15.0f;

  if (indeterminate) {
    double time = GetTime();
    float width = bounds.width;
    float cycle = 2.0f;
    float t = (float)fmod(time, cycle) / cycle;

    float x1 = 0, w1 = 0;
    float x2 = 0, w2 = 0;

    // Line 1
    if (t < 0.75f) {
      float t1 = t / 0.75f;
      float head = EaseInOutCubic(t1);
      float tail = EaseInOutCubic(t1 - 0.2f);
      if (t1 < 0.2f)
        tail = 0;

      float start = tail * width;
      float end = head * width;
      x1 = bounds.x + start;
      w1 = end - start;
    }

    // Line 2
    if (t > 0.4f && t < 0.9f) {
      float t2 = (t - 0.4f) / 0.5f;
      float head = EaseInOutCubic(t2);
      float tail = EaseInOutCubic(t2 - 0.3f);
      if (t2 < 0.3f)
        tail = 0;

      float start = tail * width;
      float end = head * width;
      x2 = bounds.x + start;
      w2 = end - start;
    }

    auto DrawSegment = [&](float x, float w) {
      if (w <= 0)
        return;
      // Clip (simple bounds check for start/end)
      float startX = x;
      float endX = x + w;

      // Fully clip if outside
      if (endX < bounds.x || startX > bounds.x + bounds.width)
        return;

      // Clamp
      if (startX < bounds.x)
        startX = bounds.x;
      if (endX > bounds.x + bounds.width)
        endX = bounds.x + bounds.width;

      if (endX <= startX)
        return;

      Vector2 p1 = {startX, bounds.y + bounds.height / 2.0f};
      Vector2 p2 = {endX, bounds.y + bounds.height / 2.0f};

      DrawWigglyLine(p1, p2, amplitude, frequency, phase, activeColor,
                     bounds.height);

      // Dot at the end? Only if visible and not clipped substantially?
      // MD3 Linear indicator doesn't usually have a dot at the end unless it's
      // a specific style. The old code had it. Let's keep it if it's the right
      // edge of the segment.
      if (x + w <= bounds.x + bounds.width) {
        // DrawCircleV({x + w, bounds.y + bounds.height / 2.0f}, bounds.height
        // / 2.0f, activeColor);
      }
    };

    DrawSegment(x1, w1);
    DrawSegment(x2, w2);

  } else {
    // Determinate
    float w = bounds.width * std::clamp(value, 0.0f, 1.0f);
    if (w > 0) {
      Vector2 start = {bounds.x, bounds.y + bounds.height / 2.0f};
      Vector2 end = {bounds.x + w, bounds.y + bounds.height / 2.0f};
      DrawWigglyLine(start, end, amplitude, frequency, phase, activeColor,
                     bounds.height);

      // Optional dot at the very end
      // DrawCircleV(end, bounds.height / 2.0f, activeColor);
    }
  }
}

} // namespace raym3
