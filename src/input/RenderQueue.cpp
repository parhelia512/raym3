#include "raym3/input/RenderQueue.h"

#if RAYM3_USE_INPUT_LAYERS

#include "raym3/layout/Layout.h"
#include "raym3/raym3.h"
#include <algorithm>

namespace raym3 {

std::vector<RenderCommand> RenderQueue::renderQueue_;
int RenderQueue::currentLayerId_ = 0;
std::vector<int> RenderQueue::layerStack_ = {0};
int RenderQueue::registrationCounter_ = 0;
int RenderQueue::nextComponentId_ = 0;
std::vector<int> RenderQueue::inputBlockingLayers_;

void RenderQueue::Initialize() {
  Clear();
}

void RenderQueue::BeginFrame() {
  Clear();
  currentLayerId_ = 0;
  layerStack_ = {0};
  registrationCounter_ = 0;
  nextComponentId_ = 0;
}

void RenderQueue::Clear() {
  renderQueue_.clear();
  inputBlockingLayers_.clear();
}

void RenderQueue::PushLayer(int zOrder) {
  currentLayerId_++;
  layerStack_.push_back(currentLayerId_);
}

void RenderQueue::PopLayer() {
  if (layerStack_.empty()) {
    layerStack_ = {0};
    currentLayerId_ = 0;
    return;
  }
  if (layerStack_.size() > 1) {
    layerStack_.pop_back();
    currentLayerId_ = layerStack_.back();
  }
}

int RenderQueue::GetCurrentLayerId() {
  if (layerStack_.empty()) {
    return 0;
  }
  return layerStack_.back();
}

Rectangle RenderQueue::RegisterComponent(
    ComponentType type,
    std::function<void(Rectangle)> renderFunc,
    int layerId,
    bool consumesInput) {
  
  Rectangle bounds = Layout::Alloc(Layout::Flex(0));
  Rectangle clipRect = GetCurrentScissorBounds();

  RenderCommand cmd;
  cmd.type = type;
  cmd.bounds = bounds;
  cmd.clipRect = clipRect;
  cmd.layerId = (layerId == 0) ? currentLayerId_ : layerId;
  cmd.zOrder = cmd.layerId;
  cmd.consumesInput = consumesInput;
  cmd.registrationOrder = registrationCounter_++;
  cmd.renderFunc = [renderFunc, bounds, clipRect]() {
    if (clipRect.width > 0 && clipRect.height > 0)
      PushScissor(clipRect);
    renderFunc(bounds);
    if (clipRect.width > 0 && clipRect.height > 0)
      PopScissor();
  };

  renderQueue_.push_back(cmd);
  
  return bounds;
}

void RenderQueue::BuildInputBlockingMap() {
  inputBlockingLayers_.clear();
  inputBlockingLayers_.resize(renderQueue_.size(), -1);
  
  Vector2 mousePos = GetMousePosition();
  
  // Find the topmost layer under the mouse that consumes input
  int topmostLayerUnderMouse = -1;
  
  for (size_t i = 0; i < renderQueue_.size(); i++) {
    const auto& cmd = renderQueue_[i];
    if (cmd.consumesInput && CheckCollisionPointRec(mousePos, cmd.bounds)) {
      if (topmostLayerUnderMouse == -1 || cmd.zOrder > topmostLayerUnderMouse) {
        topmostLayerUnderMouse = cmd.zOrder;
      }
    }
  }
  
  // Now mark which components should receive input
  for (size_t i = 0; i < renderQueue_.size(); i++) {
    const auto& cmd = renderQueue_[i];
    if (topmostLayerUnderMouse != -1 && cmd.zOrder < topmostLayerUnderMouse) {
      // This component is blocked by a higher layer
      inputBlockingLayers_[i] = topmostLayerUnderMouse;
    }
  }
}

bool RenderQueue::ShouldReceiveInput(Rectangle bounds, int layerId) {
  // Build the input blocking map if not already done this frame
  if (inputBlockingLayers_.empty() && !renderQueue_.empty()) {
    BuildInputBlockingMap();
  }
  
  // Find this component in the queue
  Vector2 mousePos = GetMousePosition();
  if (!CheckCollisionPointRec(mousePos, bounds)) {
    return false;
  }
  
  // Check if blocked
  for (size_t i = 0; i < renderQueue_.size(); i++) {
    const auto& cmd = renderQueue_[i];
    if (cmd.bounds.x == bounds.x && cmd.bounds.y == bounds.y &&
        cmd.bounds.width == bounds.width && cmd.bounds.height == bounds.height &&
        cmd.layerId == layerId) {
      return inputBlockingLayers_[i] == -1;
    }
  }
  
  return true;
}

void RenderQueue::ExecuteRenderQueue() {
  // Build input blocking map first
  BuildInputBlockingMap();
  
  // Sort by Z-order (back to front)
  // Lower zOrder = rendered first (in back)
  // Higher zOrder = rendered last (in front)
  std::stable_sort(renderQueue_.begin(), renderQueue_.end(),
    [](const RenderCommand& a, const RenderCommand& b) {
      if (a.zOrder != b.zOrder) {
        return a.zOrder < b.zOrder;
      }
      // Same layer - maintain registration order
      return a.registrationOrder < b.registrationOrder;
    });
  
  for (auto& cmd : renderQueue_) {
    cmd.renderFunc();
  }
}

} // namespace raym3

#endif // RAYM3_USE_INPUT_LAYERS

