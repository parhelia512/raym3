#pragma once

#include <functional>
#include <raylib.h>

namespace raym3 {

class View3D {
public:
  View3D();
  ~View3D();

  // Render the content (usually 3D scene) into the given bounds with rounded
  // corners. The renderCallback should contain the drawing commands (e.g.,
  // BeginMode3D/EndMode3D).
  // Optional postProcessShader: if provided (shader.id != 0), applies it as a
  // post-process effect to the rendered scene before displaying with rounded corners.
  // setPostProcessUniforms: if provided, called after BeginShaderMode(postProcessShader)
  // and before DrawTexturePro so effect uniforms (resolution, time, etc.) are set
  // while the post-process shader is active.
  // Returns the layer ID that View3D is on (for input validation)
  int Render(Rectangle bounds, std::function<void()> renderCallback,
             Shader postProcessShader = {0},
             std::function<void(int width, int height)> setPostProcessUniforms = nullptr);

  void SetCornerRadius(float radius);
  
  // Force refresh of the render texture (clears and recreates it)
  void Reset();

  // Get the layer ID that this View3D instance is on
  // Returns -1 if not rendered yet or layers are disabled
  int GetLayerId() const { return layerId_; }

  // Check if input should be processed for this viewport
  // Requires the layer ID from Render() or GetLayerId()
  // Returns true only if mouse press started within bounds AND on View3D's layer
  static bool ShouldProcessInput(Rectangle bounds, int layerId);
  
  // Check if viewport can process continuous input (hover, wheel, keyboard)
  // Requires the layer ID from Render() or GetLayerId()
  // Returns true if mouse is over viewport and no higher layer blocks it
  static bool CanProcessContinuousInput(Rectangle bounds, int layerId);

private:
  RenderTexture2D target_ = {0};
  RenderTexture2D postProcessTarget_ = {0};
  Shader shader_ = {0};
  float cornerRadius_ = 16.0f; // Default radius
  int shaderLocResolution_ = -1;
  int shaderLocRadius_ = -1;
  bool shaderLoaded_ = false;
  int layerId_ = -1; // Layer ID when rendered, -1 if not rendered yet

  void LoadRoundedShader();
  void EnsureTextureSize(int width, int height);
};

} // namespace raym3
