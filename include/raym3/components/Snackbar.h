#pragma once

#include <functional>
#include <raylib.h>
#include <string>

namespace raym3 {

struct SnackbarAction {
  const char *label;
  std::function<void()> callback;
};

class SnackbarComponent {
public:
  /**
   * @brief Shows a snackbar message.
   *
   * @param message Text to display.
   * @param duration Duration in seconds. If <= 0, message persists until
   * dismissed or replaced.
   * @param action Optional action button.
   */
  static void Show(const char *message, float duration = 4.0f,
                   SnackbarAction action = {nullptr, nullptr});

  /**
   * @brief Hides the current snackbar immediately.
   */
  static void Hide();

  /**
   * @brief Renders the active snackbar. Should be called at the end of your
   * render loop.
   *
   * @param screenBounds The area where the snackbar can appear (usually screen
   * bounds).
   */
  static void Render(Rectangle screenBounds);

private:
  static bool isOpen_;
  static float timer_;
  static std::string message_;
  static SnackbarAction action_;

  static Rectangle GetBounds(Rectangle screenBounds);
};

} // namespace raym3
