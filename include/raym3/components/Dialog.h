#pragma once

#include <raylib.h>

namespace raym3 {

class DialogComponent {
public:
  static bool Render(Rectangle bounds, const char *title, const char *message,
                     const char *buttons);

  static bool IsActive() { return isOpen_; }
  static bool IsRendering() { return isRendering_; }
  static int GetSelectedButtonIndex();

private:
  static int buttonCount_;
  static int selectedButton_;
  static bool isOpen_;
  static bool isRendering_;

  static void DrawBackdrop();
  static Rectangle GetDialogBounds(Rectangle screenBounds);
};

} // namespace raym3
