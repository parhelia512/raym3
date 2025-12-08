#pragma once

#include <raylib.h>

namespace raym3 {

class ProgressIndicator {
public:
  static void Circular(Rectangle bounds, float value = 0.0f,
                       bool indeterminate = false, Color color = BLANK,
                       float wiggleAmplitude = 2.0f,
                       float wiggleWavelength = 20.0f);
  static void Linear(Rectangle bounds, float value = 0.0f,
                     bool indeterminate = false, Color color = BLANK,
                     float wiggleAmplitude = 2.0f,
                     float wiggleWavelength = 20.0f);
};

} // namespace raym3
