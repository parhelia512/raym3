# Native Text Input for raym3

## Overview

The raym3 TextField component now supports **platform-native text input** as an optional backend. This provides:

- ✅ **Full IME support** (Input Method Editors for Japanese, Chinese, Korean, etc.)
- ✅ **Platform autocorrect** and spell-checking
- ✅ **Native text selection** and cursor behavior
- ✅ **Accessibility features** (screen readers, etc.)
- ✅ **Custom Material Design rendering** - You keep the beautiful raym3 UI!

## How It Works

### The Offscreen Input Pattern

Instead of using modal dialogs or replacing your custom UI, this implementation uses an **offscreen native text input control**:

```
┌─────────────────────────────────────┐
│  Your Beautiful raym3 TextField     │  ← Custom Material Design rendering
│  ┌──────────────────────────┐       │
│  │ Hello World_             │       │
│  └──────────────────────────┘       │
└─────────────────────────────────────┘
                ↕ mirrors state
┌─────────────────────────────────────┐
│  Offscreen Native Text Input        │  ← Invisible, handles IME/autocorrect
│  (positioned at -1000, -1000)       │
└─────────────────────────────────────┘
```

**Flow:**
1. User clicks on raym3::TextField
2. Offscreen native input receives focus
3. User types (OS handles IME, autocorrect, etc.)
4. Native input callbacks update the buffer
5. raym3::TextField renders the text with custom styling

## Building with Native Text Input

### CMake Option

Enable native text input when configuring:

```bash
cmake -DRAYM3_ENABLE_NATIVE_TEXT_INPUT=ON ..
```

Or disable it (uses custom input only):

```bash
cmake -DRAYM3_ENABLE_NATIVE_TEXT_INPUT=OFF ..
```

### Platform Support

| Platform | Status | Backend |
|----------|--------|---------|
| **macOS** | ✅ Implemented | NSTextView (Cocoa) |
| **Windows** | 🚧 Planned | Win32 Edit Control + IMM32 |
| **Linux** | 🚧 Planned | GTK3 GtkEntry or XIM |

## Usage

### Basic Example

```cpp
#include <raym3/components/TextField.h>

char buffer[256] = "Hello";
Rectangle bounds = {100, 100, 300, 56};

raym3::TextFieldOptions options;
options.useNativeInput = true;  // Enable native input!
options.placeholder = "Type something...";

if (raym3::TextFieldComponent::Render(buffer, sizeof(buffer), bounds, 
                                       "Label", options)) {
    // User pressed Enter
    printf("Submitted: %s\n", buffer);
}
```

### When to Use Native Input

**Use `useNativeInput = true` when:**
- Users need to type in non-Latin scripts (Japanese, Chinese, Korean, Arabic, etc.)
- You want platform autocorrect and spell-checking
- Accessibility is important (screen readers)
- You're building a chat app, text editor, or form-heavy application

**Use `useNativeInput = false` (default) when:**
- You only need basic Latin text input
- You want complete control over input behavior
- You're targeting platforms without native input support
- Performance is critical (native input has slight overhead)

### Advanced Example

```cpp
#include <raym3/components/TextField.h>
#include <raym3/input/NativeTextInput.h>

// Initialize native input system (call once at startup)
if (raym3::NativeTextInput::Initialize()) {
    printf("Native text input available!\n");
}

// Set up callbacks
raym3::NativeTextInput::SetTextChangedCallback([](const char* text) {
    printf("Text changed: %s\n", text);
});

raym3::NativeTextInput::SetSubmitCallback([]() {
    printf("User pressed Enter!\n");
});

// In your render loop
char buffer[256] = "";
Rectangle bounds = {100, 100, 300, 56};

raym3::TextFieldOptions options;
options.useNativeInput = true;
options.passwordMode = false;  // Native input respects this!

raym3::TextFieldComponent::Render(buffer, sizeof(buffer), bounds, 
                                   "Email", options);

// Cleanup (call at shutdown)
raym3::NativeTextInput::Shutdown();
```

## API Reference

### NativeTextInput Class

```cpp
namespace raym3 {

class NativeTextInput {
public:
    // Initialize the native text input system
    static bool Initialize();
    
    // Shutdown the native text input system
    static void Shutdown();
    
    // Check if native input is available on this platform
    static bool IsAvailable();
    
    // Activate native input for a field
    static void Activate(const char* initialText, int maxLength,
                        bool isPassword = false, bool isMultiline = false);
    
    // Deactivate native input
    static void Deactivate();
    
    // Check if native input is currently active
    static bool IsActive();
    
    // Update native input state (call once per frame)
    static void Update();
    
    // Get current text from native input
    static const char* GetText();
    
    // Get cursor position
    static int GetCursorPosition();
    
    // Get selection range
    static bool GetSelection(int& outStart, int& outEnd);
    
    // Set text change callback
    static void SetTextChangedCallback(std::function<void(const char*)> callback);
    
    // Set submit callback (Enter key)
    static void SetSubmitCallback(std::function<void()> callback);
    
    // Set IME composition window position
    static void SetCompositionRect(float x, float y, float width, float height);
};

}
```

## Implementation Details

### macOS (Cocoa)

The macOS implementation uses an offscreen `NSTextView` positioned at `(-1000, -1000)`:

- Creates a borderless, transparent `NSWindow`
- Hosts an `NSTextView` that receives keyboard focus
- Intercepts text input events and forwards them to raym3
- Supports full IME composition (Japanese kana → kanji conversion, etc.)
- Respects `maxLength`, `passwordMode`, and other options

### Windows (Planned)

Will use Win32 `Edit` control with IMM32 for IME:

```cpp
// Pseudocode
HWND hwndEdit = CreateWindowEx(WS_EX_TRANSPARENT, "EDIT", ...);
SetWindowPos(hwndEdit, HWND_TOP, -1000, -1000, 100, 20, SWP_NOACTIVATE);
// Hook WM_CHAR, WM_IME_COMPOSITION, etc.
```

### Linux (Planned)

Will use GTK3 or XIM:

```cpp
// Pseudocode (GTK3)
GtkWidget* entry = gtk_entry_new();
gtk_window_move(GTK_WINDOW(window), -1000, -1000);
g_signal_connect(entry, "changed", G_CALLBACK(on_text_changed), NULL);
```

## Fallback Behavior

If native input is not available or disabled:
- `useNativeInput` flag is ignored
- TextField uses the existing custom implementation
- All features work as before (selection, undo/redo, clipboard, etc.)

## Performance

- **Initialization**: ~1ms (creates offscreen window)
- **Per-frame overhead**: ~0.1ms (polls native input state)
- **Memory**: ~4KB (native window + text buffer)

## Troubleshooting

### "Native text input not available"

Check if it's enabled in CMake:
```bash
cmake -L | grep NATIVE_TEXT_INPUT
```

### IME window appears in wrong position

Call `SetCompositionRect()` to position the IME candidate window:
```cpp
raym3::NativeTextInput::SetCompositionRect(bounds.x, bounds.y + bounds.height, 
                                           bounds.width, 20);
```

### Text not updating

Make sure to call `NativeTextInput::Update()` once per frame, or let TextField handle it automatically when `useNativeInput = true`.

## Future Enhancements

- [ ] Windows implementation (Win32 + IMM32)
- [ ] Linux implementation (GTK3 or XIM)
- [ ] Android/iOS support (native mobile keyboards)
- [ ] Multiline text input support
- [ ] Rich text formatting hints
- [ ] Voice input integration

## Contributing

To add support for a new platform:

1. Create `src/input/NativeTextInput_<platform>.cpp`
2. Implement all methods from `NativeTextInput.h`
3. Add platform detection in `CMakeLists.txt`
4. Test with IME (Japanese, Chinese, etc.)
5. Submit a PR!

## License

Same as raym3 (check main LICENSE file)
