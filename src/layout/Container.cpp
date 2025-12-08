#include "raym3/layout/Container.h"

namespace raym3 {

std::vector<Container::ContainerState> Container::stack_;

void Container::Begin(Rectangle bounds, LayoutDirection direction) {
    ContainerState state;
    state.bounds = bounds;
    state.direction = direction;
    state.rootNode = std::make_unique<LayoutNode>();
    state.rootNode->SetWidth(bounds.width);
    state.rootNode->SetHeight(bounds.height);
    state.rootNode->SetDirection(direction);
    
    stack_.push_back(std::move(state));
}

void Container::End() {
    if (!stack_.empty()) {
        auto& state = stack_.back();
        state.rootNode->CalculateLayout(state.bounds.width, state.bounds.height);
        stack_.pop_back();
    }
}

LayoutNode* Container::GetCurrent() {
    if (stack_.empty()) {
        return nullptr;
    }
    return stack_.back().rootNode.get();
}

Rectangle Container::GetCurrentBounds() {
    if (stack_.empty()) {
        return {0, 0, 0, 0};
    }
    return stack_.back().bounds;
}

} // namespace raym3

