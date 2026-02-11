# Input Layer System

The Input Layer System provides Z-ordering and automatic input blocking, allowing you to create overlays, modals, and complex UI hierarchies where higher layers automatically block input to lower layers.

## Overview

The Input Layer System solves the problem of managing input in layered UIs. Without it, you'd need to manually check if clicks should go to a modal dialog or the content behind it. With input layers, higher Z-order layers automatically block input to lower layers.

## Enabling Input Layers

Input layers must be enabled at build time:

```cmake
set(RAYM3_USE_INPUT_LAYERS ON CACHE BOOL "" FORCE)
add_subdirectory(raym3)
```

## Basic Concepts

- **Layer ID**: Each layer has a numeric ID. Higher IDs render on top and block input to lower IDs.
- **Layer Stack**: Layers are managed with a stack (`PushLayer()` / `PopLayer()`).
- **Blocking Regions**: Components can register themselves as blocking regions.
- **Input Capture**: For drag operations, you can require that the drag starts within the component bounds.
- **Overlay Threshold**: Layers with `zOrder >= 100` bypass layout scissor clipping. Use for menus, tooltips, dialogs.

## Basic Usage

```cpp
raym3::BeginFrame();

// Layer 0 - Background content
raym3::Button("Background Button", {100, 100, 200, 40});

// Layer 1 - Overlay panel
raym3::PushLayer(1);

// Card automatically blocks input to layer 0
raym3::Card({200, 150, 400, 300}, raym3::CardVariant::Elevated);

// Components on the card work normally
raym3::Button("Overlay Button", {220, 170, 120, 40});
raym3::TextField(buffer, sizeof(buffer), {220, 230, 200, 56}, "Name");

raym3::PopLayer();

raym3::EndFrame();
```

## Automatic Input Blocking

Cards and certain components automatically register as blocking regions. When a card is rendered on a higher layer, it automatically blocks input to components on lower layers.

```cpp
raym3::BeginFrame();

// Layer 0
raym3::Button("Can't click me when modal is open", {100, 100, 200, 40});

// Layer 1 - Modal
raym3::PushLayer(1);
raym3::Card({150, 150, 500, 400}, raym3::CardVariant::Elevated);
// Card blocks input to layer 0 automatically

raym3::PopLayer();
raym3::EndFrame();
```

## Manual Blocking Regions

You can manually register blocking regions for custom components:

```cpp
#include "raym3/input/InputLayer.h"

raym3::BeginFrame();

raym3::PushLayer(1);

// Register a custom blocking region
Rectangle customPanel = {200, 200, 300, 200};
raym3::InputLayerManager::RegisterBlockingRegion(customPanel, true);

// Draw your custom panel
DrawRectangleRec(customPanel, GRAY);

raym3::PopLayer();
raym3::EndFrame();
```

## Input Capture for Drag Operations

For drag operations (like camera controls in a 3D viewport), you often want to ensure the drag started within the component bounds. This prevents accidental drags when the mouse moves over the viewport.

```cpp
#include "raym3/input/InputLayer.h"

Rectangle viewportBounds = {100, 100, 400, 300};

// Register as blocking region
raym3::InputLayerManager::RegisterBlockingRegion(viewportBounds, true);

// Only process input if drag started in viewport
bool canProcessInput = raym3::InputLayerManager::BeginInputCapture(
    viewportBounds, 
    true  // requireStartInBounds = true
);

if (canProcessInput) {
    // Process camera rotation/zoom
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        // Rotate camera based on mouse delta
        Vector2 mouseDelta = GetMouseDelta();
        // ... update camera
    }
}

// Input capture is automatically released when mouse button is released
```

## Layer Management

### PushLayer / PopLayer

```cpp
raym3::BeginFrame();

// Default layer (layer 0)
raym3::Button("Layer 0", {100, 100, 120, 40});

// Push to layer 1
raym3::PushLayer(1);
raym3::Card({200, 150, 400, 300}, raym3::CardVariant::Elevated);
raym3::Button("Layer 1", {220, 170, 120, 40});

// Push to layer 2 (nested)
raym3::PushLayer(2);
raym3::Card({300, 200, 200, 150}, raym3::CardVariant::Elevated);
raym3::Button("Layer 2", {320, 220, 100, 40});
raym3::PopLayer();  // Back to layer 1

raym3::PopLayer();  // Back to layer 0

raym3::EndFrame();
```

### Explicit Z-Order

You can specify explicit Z-order values:

```cpp
raym3::PushLayer(10);  // Explicit layer ID
// ... components on layer 10
raym3::PopLayer();
```

### Overlay Layers (zOrder >= 100)

Layers with `zOrder >= 100` call `EndScissorMode()` when pushed, so they render above layout clipping. Use for menus, tooltips, and dialogs that must not be clipped by scroll containers.

```cpp
raym3::PushLayer(100);  // Overlay - bypasses scissor
raym3::Menu(...);       // Menu renders above clipped content
raym3::PopLayer();
```

## Common Patterns

### Modal Dialog

```cpp
bool showModal = false;

raym3::BeginFrame();

// Background content (layer 0)
raym3::Button("Open Modal", {100, 100, 120, 40}, 
              []() { showModal = true; });

if (showModal) {
    // Modal (layer 1)
    raym3::PushLayer(1);
    
    // Backdrop (optional, can draw semi-transparent overlay)
    // DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), 
    //               ColorAlpha(BLACK, 0.5f));
    
    // Modal card
    raym3::Card({200, 200, 400, 300}, raym3::CardVariant::Elevated);
    
    // Modal content
    raym3::Text("Modal Title", {220, 220, 360, 40}, 24, scheme.onSurface);
    raym3::Button("Close", {220, 400, 100, 40}, 
                  []() { showModal = false; });
    
    raym3::PopLayer();
}

raym3::EndFrame();
```

### 3D Viewport with UI Overlay

```cpp
Rectangle viewportBounds = {100, 100, 600, 400};

raym3::BeginFrame();

// Register viewport as blocking region
raym3::InputLayerManager::RegisterBlockingRegion(viewportBounds, true);

// Check if we can process viewport input
bool canControlCamera = raym3::InputLayerManager::BeginInputCapture(
    viewportBounds, true);

if (canControlCamera) {
    // Process camera controls
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        // Rotate camera
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        // Pan camera
    }
}

// Render 3D viewport
BeginScissorMode((int)viewportBounds.x, (int)viewportBounds.y,
                 (int)viewportBounds.width, (int)viewportBounds.height);
BeginMode3D(camera);
// ... render 3D scene
EndMode3D();
EndScissorMode();

// UI overlay on top (layer 1)
raym3::PushLayer(1);
raym3::Card({viewportBounds.x + 10, viewportBounds.y + 10, 150, 100},
            raym3::CardVariant::Elevated);
raym3::Text("Viewport Controls", {viewportBounds.x + 20, viewportBounds.y + 20, 
                                  130, 30}, 16, scheme.onSurface);
raym3::Button("Reset", {viewportBounds.x + 20, viewportBounds.y + 60, 100, 30});
raym3::PopLayer();

raym3::EndFrame();
```

## API Reference

### Layer Management

- **`void PushLayer(int zOrder = 0)`** - Push a new layer onto the stack. If `zOrder` is 0, auto-increments. Otherwise uses the specified Z-order.
- **`void PopLayer()`** - Pop the current layer from the stack, returning to the previous layer.

### Blocking Regions

- **`void RegisterBlockingRegion(Rectangle bounds, bool blocksInput = true)`** - Register a region that blocks input to lower layers.

### Input Capture

- **`bool BeginInputCapture(Rectangle bounds, bool requireStartInBounds = true, int layerId = -1)`** - Begin input capture for drag operations. Returns `true` if input can be processed. If `requireStartInBounds` is `true`, the mouse button must have been pressed within the bounds.
- **`bool IsInputCaptured()`** - Check if input is currently captured.
- **`void ReleaseCapture()`** - Manually release input capture.

### Input Checking

- **`bool ShouldProcessMouseInput(Rectangle bounds, int layerId = -1)`** - Check if mouse input should be processed for the given bounds on the specified layer.

## Important Notes

1. **Frame-Based**: Input layers are reset each frame. You must call `PushLayer()` / `PopLayer()` every frame.

2. **Automatic Blocking**: Cards automatically block input. You don't need to manually register them.

3. **Input Capture**: Input capture is automatically released when all mouse buttons are released. You typically don't need to call `ReleaseCapture()` manually.

4. **Layer IDs**: Layer IDs are relative. Layer 1 blocks layer 0, layer 2 blocks layers 0 and 1, etc.

5. **Overlay Threshold**: Layers with `zOrder >= 100` bypass scissor clipping. Use for menus, tooltips, dialogs.

6. **Performance**: The input layer system has minimal overhead when disabled (`RAYM3_USE_INPUT_LAYERS=OFF`), as it compiles to no-ops.

See `examples/input_layers_test.cpp` for a complete working example.

