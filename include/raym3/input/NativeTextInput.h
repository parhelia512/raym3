#pragma once

#include <functional>
#include <string>

namespace raym3 {

/**
 * @brief Platform-native text input backend
 *
 * This class manages an offscreen native text input control that provides
 * IME support, autocorrect, and other platform-specific text input features.
 * The raym3::TextField component mirrors the state of this native input
 * while providing custom Material Design rendering.
 */
class NativeTextInput {
public:
  /**
   * @brief Initialize the native text input system
   * @return true if native input is available on this platform
   */
  static bool Initialize();

  /**
   * @brief Shutdown the native text input system
   */
  static void Shutdown();

  /**
   * @brief Check if native text input is available
   */
  static bool IsAvailable();

  /**
   * @brief Activate native text input for a field
   * @param initialText Initial text content
   * @param maxLength Maximum text length
   * @param isPassword Whether this is a password field
   * @param isMultiline Whether this is a multiline field
   */
  static void Activate(const char *initialText, int maxLength,
                       bool isPassword = false, bool isMultiline = false);

  /**
   * @brief Deactivate native text input
   */
  static void Deactivate();

  /**
   * @brief Check if native input is currently active
   */
  static bool IsActive();

  /**
   * @brief Update native input state (call once per frame)
   */
  static void Update();

  /**
   * @brief Get current text from native input
   */
  static const char *GetText();

  /**
   * @brief Get cursor position in the text
   */
  static int GetCursorPosition();

  /**
   * @brief Get selection range (start, end)
   * @param outStart Output parameter for selection start
   * @param outEnd Output parameter for selection end
   * @return true if there is a selection
   */
  static bool GetSelection(int &outStart, int &outEnd);

  /**
   * @brief Set text change callback
   * Called whenever the text changes in the native input
   */
  static void
  SetTextChangedCallback(std::function<void(const char *)> callback);

  /**
   * @brief Set submit callback
   * Called when user presses Enter/Return
   */
  static void SetSubmitCallback(std::function<void()> callback);

  /**
   * @brief Set colors for the native text field
   * @param textR Text color red (0-255)
   * @param textG Text color green (0-255)
   * @param textB Text color blue (0-255)
   * @param textA Text color alpha (0-255)
   * @param bgR Background color red (0-255)
   * @param bgG Background color green (0-255)
   * @param bgB Background color blue (0-255)
   * @param bgA Background color alpha (0-255)
   */
  static void SetColors(unsigned char textR, unsigned char textG,
                        unsigned char textB, unsigned char textA,
                        unsigned char bgR, unsigned char bgG, unsigned char bgB,
                        unsigned char bgA);

  /**
   * @brief Set the position hint for IME composition window
   * This tells the OS where to display the IME candidate window
   */
  static void SetCompositionRect(float x, float y, float width, float height);

private:
  NativeTextInput() = default;
  ~NativeTextInput() = default;
};

} // namespace raym3
