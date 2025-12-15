#include "raym3/input/NativeTextInput.h"
#include <raylib.h>

#ifdef __APPLE__
#include <Cocoa/Cocoa.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#include <functional>
#include <string>

// Custom NSTextView that forwards events and syncs with raym3
@interface Raym3NativeTextView : NSTextView
@property(nonatomic, assign) int maxLength;
@end

@implementation Raym3NativeTextView

- (void)textDidChange:(NSNotification *)notification {
  [super textDidChange:notification];
  // Trigger callback
  raym3::NativeTextInput::Update();
}

- (BOOL)shouldChangeTextInRange:(NSRange)range
              replacementString:(NSString *)string {
  if (self.maxLength > 0) {
    NSInteger newLength =
        [[self string] length] - range.length + [string length];
    if (newLength > self.maxLength) {
      return NO;
    }
  }
  return YES;
}

// Override to handle Cmd shortcuts (Cmd+C, Cmd+V, Cmd+X, Cmd+A, etc.)
- (BOOL)performKeyEquivalent:(NSEvent *)event {
  // Let NSTextView handle all Cmd key combinations natively
  if ([event modifierFlags] & NSEventModifierFlagCommand) {
    // Check for standard editing commands
    NSString *chars = [event charactersIgnoringModifiers];
    if ([chars length] == 1) {
      unichar character = [chars characterAtIndex:0];

      // Handle standard shortcuts
      switch (character) {
      case 'c': // Cmd+C
        [self copy:nil];
        return YES;
      case 'x': // Cmd+X
        [self cut:nil];
        return YES;
      case 'v': // Cmd+V
        [self paste:nil];
        return YES;
      case 'a': // Cmd+A
        [self selectAll:nil];
        return YES;
      case 'z': // Cmd+Z
        if ([event modifierFlags] & NSEventModifierFlagShift) {
          [[self undoManager] redo];
        } else {
          [[self undoManager] undo];
        }
        return YES;
      }
    }
  }
  return [super performKeyEquivalent:event];
}

// Ensure we can respond to these actions
- (BOOL)validateUserInterfaceItem:(id<NSValidatedUserInterfaceItem>)item {
  SEL action = [item action];

  if (action == @selector(copy:)) {
    return [[self string] length] > 0 && [self selectedRange].length > 0;
  } else if (action == @selector(cut:)) {
    return [[self string] length] > 0 && [self selectedRange].length > 0;
  } else if (action == @selector(paste:)) {
    return [[NSPasteboard generalPasteboard]
               stringForType:NSPasteboardTypeString] != nil;
  } else if (action == @selector(selectAll:)) {
    return [[self string] length] > 0;
  }

  return [super validateUserInterfaceItem:item];
}

@end

namespace raym3 {

// Static state
struct NativeTextInputState {
  bool initialized = false;
  bool active = false;
  Raym3NativeTextView *textView = nullptr;
  NSScrollView *scrollView = nullptr;
  NSWindow *raylibWindow = nullptr;
  std::string currentText;
  int cursorPosition = 0;
  int selectionStart = -1;
  int selectionEnd = -1;
  std::function<void(const char *)> textChangedCallback;
  std::function<void()> submitCallback;
};

static NativeTextInputState *g_state = nullptr;

bool NativeTextInput::Initialize() {
  if (g_state && g_state->initialized) {
    return true;
  }

  if (!g_state) {
    g_state = new NativeTextInputState();
  }

  @autoreleasepool {
    // Get the Raylib/GLFW window
    GLFWwindow *glfwWindow = glfwGetCurrentContext();
    if (!glfwWindow) {
      return false;
    }

    g_state->raylibWindow = glfwGetCocoaWindow(glfwWindow);
    if (!g_state->raylibWindow) {
      return false;
    }

    g_state->initialized = true;
  }

  return true;
}

void NativeTextInput::Shutdown() {
  if (!g_state) {
    return;
  }

  @autoreleasepool {
    if (g_state->scrollView) {
      [g_state->scrollView removeFromSuperview];
      g_state->scrollView = nil;
      g_state->textView = nil;
    }
  }

  delete g_state;
  g_state = nullptr;
}

bool NativeTextInput::IsAvailable() { return true; }

void NativeTextInput::Activate(const char *initialText, int maxLength,
                               bool isPassword, bool isMultiline) {
  if (!g_state || !g_state->initialized) {
    if (!Initialize()) {
      return;
    }
  }

  @autoreleasepool {
    // Create text view if it doesn't exist
    if (!g_state->textView) {
      NSRect frame = NSMakeRect(0, 0, 100, 20);

      // Create scroll view (required container for NSTextView)
      g_state->scrollView = [[NSScrollView alloc] initWithFrame:frame];
      [g_state->scrollView setBorderType:NSNoBorder];
      [g_state->scrollView setHasVerticalScroller:NO];
      [g_state->scrollView setHasHorizontalScroller:NO];
      [g_state->scrollView setDrawsBackground:NO];

      // Create text view
      NSSize contentSize = [g_state->scrollView contentSize];
      g_state->textView = [[Raym3NativeTextView alloc]
          initWithFrame:NSMakeRect(0, 0, contentSize.width,
                                   contentSize.height)];

      // Style to be completely transparent and borderless
      [g_state->textView setDrawsBackground:NO];
      [g_state->textView setRichText:NO];
      [g_state->textView setFieldEditor:YES];
      [g_state->textView setHorizontallyResizable:YES];
      [g_state->textView setVerticallyResizable:NO];
      [[g_state->textView textContainer]
          setContainerSize:NSMakeSize(FLT_MAX, contentSize.height)];
      [[g_state->textView textContainer] setWidthTracksTextView:NO];

      // Set default font to match raym3
      NSFont *font = [NSFont systemFontOfSize:16.0];
      [g_state->textView setFont:font];

      // Add text view to scroll view
      [g_state->scrollView setDocumentView:g_state->textView];

      // Add scroll view to window
      [[g_state->raylibWindow contentView] addSubview:g_state->scrollView];
    }

    // Set initial text
    NSString *text =
        [NSString stringWithUTF8String:(initialText ? initialText : "")];
    [g_state->textView setString:text];
    [g_state->textView setMaxLength:maxLength];

    // Make it first responder to receive keyboard input
    [g_state->raylibWindow makeFirstResponder:g_state->textView];

    g_state->active = true;
    g_state->currentText = initialText ? initialText : "";
    g_state->cursorPosition = (int)g_state->currentText.length();
  }
}

void NativeTextInput::Deactivate() {
  if (!g_state || !g_state->active) {
    return;
  }

  @autoreleasepool {
    // Hide the text view
    if (g_state->scrollView) {
      [g_state->scrollView setHidden:YES];
    }

    // Trigger submit callback
    if (g_state->submitCallback) {
      g_state->submitCallback();
    }

    g_state->active = false;
  }
}

bool NativeTextInput::IsActive() { return g_state && g_state->active; }

void NativeTextInput::Update() {
  if (!g_state || !g_state->active || !g_state->textView) {
    return;
  }

  @autoreleasepool {
    // Get current text from text view
    NSString *text = [g_state->textView string];
    const char *utf8Text = [text UTF8String];
    std::string newText = utf8Text ? utf8Text : "";

    // Check if text changed
    if (newText != g_state->currentText) {
      g_state->currentText = newText;
      if (g_state->textChangedCallback) {
        g_state->textChangedCallback(g_state->currentText.c_str());
      }
    }

    // Get cursor position and selection
    NSRange selectedRange = [g_state->textView selectedRange];
    g_state->cursorPosition = (int)selectedRange.location;

    if (selectedRange.length > 0) {
      g_state->selectionStart = (int)selectedRange.location;
      g_state->selectionEnd =
          (int)(selectedRange.location + selectedRange.length);
    } else {
      g_state->selectionStart = -1;
      g_state->selectionEnd = -1;
    }
  }
}

const char *NativeTextInput::GetText() {
  if (!g_state) {
    return "";
  }
  return g_state->currentText.c_str();
}

int NativeTextInput::GetCursorPosition() {
  if (!g_state) {
    return 0;
  }
  return g_state->cursorPosition;
}

bool NativeTextInput::GetSelection(int &outStart, int &outEnd) {
  if (!g_state || g_state->selectionStart == -1) {
    return false;
  }
  outStart = g_state->selectionStart;
  outEnd = g_state->selectionEnd;
  return true;
}

void NativeTextInput::SetTextChangedCallback(
    std::function<void(const char *)> callback) {
  if (!g_state) {
    g_state = new NativeTextInputState();
  }
  g_state->textChangedCallback = callback;
}

void NativeTextInput::SetSubmitCallback(std::function<void()> callback) {
  if (!g_state) {
    g_state = new NativeTextInputState();
  }
  g_state->submitCallback = callback;
}

void NativeTextInput::SetCompositionRect(float x, float y, float width,
                                         float height) {
  if (!g_state || !g_state->active || !g_state->scrollView) {
    return;
  }

  @autoreleasepool {
    // Convert Raylib coordinates (top-left origin) to Cocoa coordinates
    // (bottom-left origin)
    NSWindow *window = g_state->raylibWindow;
    NSRect contentRect = [[window contentView] frame];

    // Raylib Y is from top, Cocoa Y is from bottom
    float cocoaY = contentRect.size.height - y - height;

    NSRect frame = NSMakeRect(x, cocoaY, width, height);
    [g_state->scrollView setFrame:frame];
    [g_state->scrollView setHidden:NO];
  }
}

void NativeTextInput::SetColors(unsigned char textR, unsigned char textG,
                                unsigned char textB, unsigned char textA,
                                unsigned char bgR, unsigned char bgG,
                                unsigned char bgB, unsigned char bgA) {
  if (!g_state || !g_state->textView) {
    return;
  }

  @autoreleasepool {
    // Convert to NSColor (0-255 to 0.0-1.0)
    NSColor *textColor = [NSColor colorWithRed:textR / 255.0
                                         green:textG / 255.0
                                          blue:textB / 255.0
                                         alpha:textA / 255.0];
    NSColor *bgColor = [NSColor colorWithRed:bgR / 255.0
                                       green:bgG / 255.0
                                        blue:bgB / 255.0
                                       alpha:bgA / 255.0];

    [g_state->textView setTextColor:textColor];
    // Keep background transparent - don't set background color or enable
    // drawing
  }
}

} // namespace raym3

#else

// Stub implementation for non-macOS platforms
namespace raym3 {

bool NativeTextInput::Initialize() { return false; }
void NativeTextInput::Shutdown() {}
bool NativeTextInput::IsAvailable() { return false; }
void NativeTextInput::Activate(const char *, int, bool, bool) {}
void NativeTextInput::Deactivate() {}
bool NativeTextInput::IsActive() { return false; }
void NativeTextInput::Update() {}
const char *NativeTextInput::GetText() { return ""; }
int NativeTextInput::GetCursorPosition() { return 0; }
bool NativeTextInput::GetSelection(int &, int &) { return false; }
void NativeTextInput::SetTextChangedCallback(
    std::function<void(const char *)>) {}
void NativeTextInput::SetSubmitCallback(std::function<void()>) {}
void NativeTextInput::SetColors(unsigned char, unsigned char, unsigned char,
                                unsigned char, unsigned char, unsigned char,
                                unsigned char, unsigned char) {}
void NativeTextInput::SetCompositionRect(float, float, float, float) {}

} // namespace raym3

#endif
