#pragma once

#include "raym3/layout/Layout.h"
#include "raym3/raym3.h"
#include <raylib.h>

namespace raym3 {

class LayoutCard {
public:
  // Begin a card container with layout support
  // Returns the inner bounds for content
  static Rectangle BeginCard(LayoutStyle style,
                             CardVariant variant = CardVariant::Elevated);

  // End the card container
  static void EndCard();

private:
  struct CardState {
    Rectangle bounds;
    CardVariant variant;
    int nodeId;
  };

  static CardState currentCard_;
};

} // namespace raym3
