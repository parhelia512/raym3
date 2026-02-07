#pragma once

#include "raym3/types.h"
#include <raylib.h>

namespace raym3 {

class SliderComponent {
public:
  static float Render(Rectangle bounds, float value, float min, float max,
                      const char *label = nullptr);
  static float Render(Rectangle bounds, float value, float min, float max,
                      const char *label, const SliderOptions &options);
  static void ResetFieldId();

private:
  static Rectangle GetTrackBounds(Rectangle bounds, bool hasLabel = false);
  static float GetValueFromPosition(Rectangle trackBounds, float x);
};

} // namespace raym3
