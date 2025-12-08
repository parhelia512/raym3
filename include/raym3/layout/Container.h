#pragma once

#include "raym3/layout/LayoutNode.h"
#include <raylib.h>
#include <vector>
#include <memory>

namespace raym3 {

class Container {
public:
    static void Begin(Rectangle bounds, LayoutDirection direction);
    static void End();
    
    static LayoutNode* GetCurrent();
    static Rectangle GetCurrentBounds();
    
private:
    struct ContainerState {
        Rectangle bounds;
        std::unique_ptr<LayoutNode> rootNode;
        LayoutDirection direction;
    };
    
    static std::vector<ContainerState> stack_;
};

} // namespace raym3

