# Scissor System

The Scissor System provides stack-based clipping regions for controlling what parts of the screen are rendered. It integrates with the Layout system and Input Layers.

## Overview

raym3 maintains a scissor stack. Each `PushScissor` intersects the new bounds with the current scissor; `PopScissor` restores the previous region. This allows nested clipping (e.g., scroll containers inside tabs).

## API

- **`void PushScissor(Rectangle bounds)`** - Push a clipping region onto the stack. Intersects with current scissor; ignores empty regions.
- **`void PopScissor()`** - Pop the current scissor and restore the previous one.
- **`void BeginScissor(Rectangle bounds)`** - Same as `PushScissor(bounds)`.
- **`Rectangle GetCurrentScissorBounds()`** - Get the active scissor rectangle (screen bounds if stack is empty).
- **`void SetScissorDebug(bool enabled)`** - Enable scissor debug overlay.
- **`bool IsScissorDebug()`** - Check if scissor debug is enabled.
- **`void DrawScissorDebug()`** - Draw green semi-transparent overlays with outlines for active scissor regions. Call after your frame content, before `EndFrame()`.

## Usage

```cpp
raym3::BeginFrame();

// Custom clipping region
raym3::PushScissor({100, 100, 400, 300});
// Content here is clipped to the rectangle
raym3::Button("Clipped Button", {150, 150, 120, 40});
raym3::PopScissor();

raym3::EndFrame();
```

## Integration with Layout

The Layout system uses the scissor stack for scroll containers. When `Layout::BeginScrollContainer` starts, it calls `BeginScissor` with the visible scroll area. When the scroll container ends, it calls `PopScissor`.

## Integration with Input Layers

Layers with `zOrder >= 100` (overlay threshold) call `EndScissorMode()` when pushed. This lets menus, tooltips, and dialogs render above layout clipping. When popping back to layer 0, scissor state is restored from the stack.

Use overlay layers (`PushLayer(100)` or higher) for:
- Dropdown menus
- Tooltips
- Modal dialogs
- Popovers

Use lower zOrder (0–99) for in-flow panels that should respect scroll clipping.

## Debugging

Enable scissor debug to visualize clipping:

```cpp
raym3::SetScissorDebug(true);

// ... your frame rendering ...

raym3::DrawScissorDebug();  // Call before EndFrame
raym3::EndFrame();
```

Active scissor regions are drawn with green fill and outlines so you can verify clipping behavior.
