#include "raym3/components/View3D.h"
#include "raym3/layout/Layout.h"
#include <algorithm>
#include <raylib.h>
#include <rlgl.h>

#if RAYM3_USE_INPUT_LAYERS
#include "raym3/input/InputLayer.h"
#endif

namespace raym3 {

// Embedded Shader Sources
static const char *fragShader330 = R"(
#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec2 resolution;
uniform float radius;

void main()
{
    vec4 texColor = texture(texture0, fragTexCoord);
    
    // Calculate pixel position
    // fragTexCoord is 0..1. 
    // In Raylib, texture coordinates might be flipped depending on how it's drawn, 
    // but for RenderTexture drawn with DrawTexturePro, it's usually standard.
    // However, RenderTextures are often Y-flipped in OpenGL. 
    // Raylib's DrawTextureRec handles the flip.
    
    vec2 pixelPos = fragTexCoord * resolution;
    vec2 center = resolution / 2.0;
    vec2 halfSize = resolution / 2.0;
    
    // SDF for rounded box
    // p is vector from center
    vec2 p = pixelPos - center;
    
    // b is the box half-size minus radius
    // We clamp radius to be at most half the smallest dimension to avoid artifacts
    float r = min(radius, min(halfSize.x, halfSize.y));
    vec2 b = halfSize - vec2(r);
    
    // Distance calculation
    float d = length(max(abs(p) - b, 0.0)) - r;
    
    // Anti-aliasing
    // We want to fade from 1.0 (inside) to 0.0 (outside) around d=0
    // smoothstep(0.0, 1.5, d) gives 0->1 transition over 1.5 pixels outside
    // 1.0 - ... gives 1->0 transition
    float alpha = 1.0 - smoothstep(-0.5, 0.5, d);
    
    // Discard fragments that are clearly outside the bounds to prevent overdraw
    // This ensures the viewport doesn't render outside its allocated area
    if (alpha <= 0.0) {
        discard;
    }
    
    finalColor = texColor * colDiffuse;
    finalColor.a *= alpha;
}
)";

static const char *fragShader100 = R"(
#version 100
precision mediump float;

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec2 resolution;
uniform float radius;

void main()
{
    vec4 texColor = texture2D(texture0, fragTexCoord);
    
    vec2 pixelPos = fragTexCoord * resolution;
    vec2 center = resolution / 2.0;
    vec2 halfSize = resolution / 2.0;
    
    vec2 p = pixelPos - center;
    float r = min(radius, min(halfSize.x, halfSize.y));
    vec2 b = halfSize - vec2(r);
    
    float d = length(max(abs(p) - b, 0.0)) - r;
    
    float alpha = 1.0 - smoothstep(-0.5, 0.5, d);
    
    // Discard fragments that are clearly outside the bounds to prevent overdraw
    if (alpha <= 0.0) {
        discard;
    }
    
    gl_FragColor = texColor * colDiffuse;
    gl_FragColor.a *= alpha;
}
)";

View3D::View3D() {
  // Initialize to zero/null
  target_ = {0};
  shader_ = {0};
}

View3D::~View3D() {
  if (target_.id != 0) {
    UnloadRenderTexture(target_);
  }
  if (shaderLoaded_) {
    UnloadShader(shader_);
  }
}

void View3D::LoadRoundedShader() {
  if (shaderLoaded_)
    return;

  // Check GLSL version support (simplified check)
  // Raylib doesn't expose a simple "IsGLSL330Supported" but we can try 330
  // first or check RLGL version. For simplicity, we'll assume desktop 330 for
  // now, or fallback if compilation fails? Raylib logs errors but returns
  // default shader if fails.

  // Actually, let's just try 330.
  shader_ = LoadShaderFromMemory(0, fragShader330);

  if (shader_.id == rlGetShaderIdDefault()) {
    // Fallback to 100
    shader_ = LoadShaderFromMemory(0, fragShader100);
  }

  shaderLocResolution_ = GetShaderLocation(shader_, "resolution");
  shaderLocRadius_ = GetShaderLocation(shader_, "radius");
  shaderLoaded_ = true;
}

void View3D::EnsureTextureSize(int width, int height) {
  if (target_.id == 0 || target_.texture.width != width ||
      target_.texture.height != height) {
    if (target_.id != 0) {
      UnloadRenderTexture(target_);
    }
    target_ = LoadRenderTexture(width, height);
    SetTextureFilter(target_.texture, TEXTURE_FILTER_BILINEAR);
  }
}

void View3D::Reset() {
  if (target_.id != 0) {
    UnloadRenderTexture(target_);
    target_ = {0};
  }
}

void View3D::SetCornerRadius(float radius) { cornerRadius_ = radius; }

int View3D::Render(Rectangle bounds, std::function<void()> renderCallback) {
  if (!shaderLoaded_) {
    LoadRoundedShader();
  }

  int width = (int)bounds.width;
  int height = (int)bounds.height;

  // Early return if bounds are invalid - don't render or register input
  if (width <= 0 || height <= 0 || bounds.x < 0 || bounds.y < 0) {
    layerId_ = -1;
    return -1;
  }

#if RAYM3_USE_INPUT_LAYERS
  // Don't push/pop layer - use current layer like other raym3 components (Card, Button, etc.)
  // The caller (EditorLayout) manages layers, View3D just registers on current layer
  layerId_ = InputLayerManager::GetCurrentLayerId();
  
  // Only register viewport as blocking region if bounds are valid
  // This prevents the viewport from blocking input when it's hidden/shrunk
  if (bounds.width > 0 && bounds.height > 0) {
    InputLayerManager::RegisterBlockingRegion(bounds, true);
  }
#else
  layerId_ = -1;
#endif

  EnsureTextureSize(width, height);

  // 1. Render scene to texture
  // IMPORTANT: We must disable any active scissor (from UI layout) because
  // it uses Screen Coordinates, which don't map correctly to FBO Coordinates.
  EndScissorMode();

  BeginTextureMode(target_);
  ClearBackground(BLANK); // Clear with transparent
  if (renderCallback) {
    renderCallback();
  }
  EndTextureMode();

  // 2. Draw texture with rounded corner shader
  // Apply scissor to ensure viewport stays within its bounds
  // This works in conjunction with TabContent scissor (they intersect)
  // Double-check bounds are still valid before drawing (defensive check)
  if (bounds.width <= 0 || bounds.height <= 0) {
    return layerId_;
  }
  
  // Enable scissor mode to clip viewport to its exact bounds
  // Intersect with parent scissor (TabContent, scroll containers) to prevent overflow
  Rectangle parentScissor = Layout::GetActiveScissorBounds();
  float left = std::max(bounds.x, parentScissor.x);
  float top = std::max(bounds.y, parentScissor.y);
  float right = std::min(bounds.x + bounds.width, parentScissor.x + parentScissor.width);
  float bottom = std::min(bounds.y + bounds.height, parentScissor.y + parentScissor.height);
  
  // Apply DPI scaling for Scissor Mode
  // Raylib's BeginScissorMode expects physical pixels if the backing store is scaled
  float scaleX = (float)GetRenderWidth() / (float)GetScreenWidth();
  float scaleY = (float)GetRenderHeight() / (float)GetScreenHeight();
  
  bool hasValidScissor = (right > left && bottom > top);
  if (!hasValidScissor) {
    // If we have no valid visible area (e.g. scrolled off or empty bounds), don't render
    // This prevents drawing the full texture over everything if bounds are 0,0 or Layout failed
    layerId_ = -1;
    return -1;
  }

  // Use scaled coordinates for Scissor
  BeginScissorMode((int)(left * scaleX), (int)(top * scaleY), 
                  (int)((right - left) * scaleX), (int)((bottom - top) * scaleY));
  
  BeginShaderMode(shader_);

  // Pass texture resolution to shader (shader works in texture coordinate space)
  // The shader calculates rounded corners based on texture dimensions
  // Scissor mode ensures the viewport doesn't draw outside its screen bounds
  float resolution[2] = {(float)width, (float)height};
  SetShaderValue(shader_, shaderLocResolution_, resolution,
                 SHADER_UNIFORM_VEC2);
  SetShaderValue(shader_, shaderLocRadius_, &cornerRadius_,
                 SHADER_UNIFORM_FLOAT);

  // Draw texture flipped vertically because of OpenGL coordinates
  Rectangle source = {0.0f, 0.0f, (float)target_.texture.width,
                      -(float)target_.texture.height};
  Rectangle dest = bounds;
  Vector2 origin = {0.0f, 0.0f};

  DrawTexturePro(target_.texture, source, dest, origin, 0.0f, WHITE);

  EndShaderMode();
  
  // End scissor mode after drawing (only if we started it)
  EndScissorMode();

  return layerId_;
}

bool View3D::CanProcessContinuousInput(Rectangle bounds, int layerId) {
#if RAYM3_USE_INPUT_LAYERS
  // Use raym3 input system to check if viewport can process input
  // This handles bounds checking and layer blocking automatically
  // Use View3D's actual layer ID, not currentLayerId_ (which might be different)
  return InputLayerManager::ShouldProcessMouseInput(bounds, layerId);
#else
  // Fallback: simple bounds check
  Vector2 mousePos = GetMousePosition();
  return CheckCollisionPointRec(mousePos, bounds);
#endif
}

bool View3D::ShouldProcessInput(Rectangle bounds, int layerId) {
  Vector2 mousePos = GetMousePosition();
  bool mouseInBounds = CheckCollisionPointRec(mousePos, bounds);
  
  // Validate that mouse button was up then pressed within bounds
  // This ensures interactions only occur when the press started in the viewport
  bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
                      IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) ||
                      IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE);
  
  bool mouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT) ||
                   IsMouseButtonDown(MOUSE_BUTTON_RIGHT) ||
                   IsMouseButtonDown(MOUSE_BUTTON_MIDDLE);
  
#if RAYM3_USE_INPUT_LAYERS
  // Use input capture with requireStartInBounds to ensure drags must start in viewport
  // BeginInputCapture with requireStartInBounds=true validates:
  // 1. Mouse button was pressed (up -> down transition) this frame
  // 2. Mouse is currently within bounds
  // 3. No higher layer is blocking
  // 4. Interaction started on View3D's layer (validated by BeginInputCapture with layerId)
  if (mouseDown && !mousePressed) {
    // Mouse is down but wasn't just pressed - drag started elsewhere
    // Check if we already have capture (started in viewport on View3D's layer)
    if (InputLayerManager::IsInputCaptured() && 
        InputLayerManager::IsInputCapturedBy(bounds, layerId)) {
      // We already have capture, continue processing
      return true;
    }
    // No capture means drag didn't start in viewport, deny
    return false;
  }
  
  // Use input capture system for validation with View3D's layer ID
  return InputLayerManager::BeginInputCapture(bounds, true, layerId);
#else
  // Fallback: validate that mouse button was pressed (not just down) within bounds
  // This prevents interactions when mouse was already down before entering viewport
  if (mouseDown && !mousePressed) {
    // Mouse is down but wasn't just pressed - this means drag started elsewhere
    // Don't allow interaction unless mouse was just pressed in bounds
    return false;
  }
  
  // Only allow if mouse was just pressed (transition from up to down) within bounds
  return mouseInBounds && mousePressed;
#endif
}

} // namespace raym3
