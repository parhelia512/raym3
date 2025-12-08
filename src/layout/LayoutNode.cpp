#include "raym3/layout/LayoutNode.h"

#if RAYM3_USE_YOGA
#include <yoga/YGNodeStyle.h>
#include <yoga/YGNodeLayout.h>
#include <yoga/YGEnums.h>
#endif

namespace raym3 {

LayoutNode::LayoutNode() {
#if RAYM3_USE_YOGA
    node_ = YGNodeNew();
#else
    node_ = nullptr;
    bounds_ = {0, 0, 0, 0};
    direction_ = LayoutDirection::Column;
    justify_ = JustifyContent::FlexStart;
    align_ = AlignItems::FlexStart;
    gap_ = 0.0f;
    padding_[0] = padding_[1] = padding_[2] = padding_[3] = 0.0f;
    margin_[0] = margin_[1] = margin_[2] = margin_[3] = 0.0f;
#endif
}

LayoutNode::~LayoutNode() {
#if RAYM3_USE_YOGA
    if (node_) {
        YGNodeFree(node_);
    }
#endif
}

LayoutNode::LayoutNode(LayoutNode&& other) noexcept
#if RAYM3_USE_YOGA
    : node_(other.node_) {
    other.node_ = nullptr;
#else
    : node_(other.node_),
      bounds_(other.bounds_),
      direction_(other.direction_),
      justify_(other.justify_),
      align_(other.align_),
      gap_(other.gap_) {
    for (int i = 0; i < 4; i++) {
        padding_[i] = other.padding_[i];
        margin_[i] = other.margin_[i];
    }
    other.node_ = nullptr;
#endif
}

LayoutNode& LayoutNode::operator=(LayoutNode&& other) noexcept {
    if (this != &other) {
#if RAYM3_USE_YOGA
        if (node_) {
            YGNodeFree(node_);
        }
        node_ = other.node_;
        other.node_ = nullptr;
#else
        node_ = other.node_;
        bounds_ = other.bounds_;
        direction_ = other.direction_;
        justify_ = other.justify_;
        align_ = other.align_;
        gap_ = other.gap_;
        for (int i = 0; i < 4; i++) {
            padding_[i] = other.padding_[i];
            margin_[i] = other.margin_[i];
        }
        other.node_ = nullptr;
#endif
    }
    return *this;
}

void LayoutNode::SetDirection(LayoutDirection direction) {
#if RAYM3_USE_YOGA
    YGFlexDirection flexDirection = (direction == LayoutDirection::Row) 
        ? YGFlexDirectionRow 
        : YGFlexDirectionColumn;
    YGNodeStyleSetFlexDirection(node_, flexDirection);
#else
    direction_ = direction;
#endif
}

void LayoutNode::SetJustifyContent(JustifyContent justify) {
#if RAYM3_USE_YOGA
    YGJustify justifyMode;
    switch (justify) {
        case JustifyContent::FlexStart:
            justifyMode = YGJustifyFlexStart;
            break;
        case JustifyContent::FlexEnd:
            justifyMode = YGJustifyFlexEnd;
            break;
        case JustifyContent::Center:
            justifyMode = YGJustifyCenter;
            break;
        case JustifyContent::SpaceBetween:
            justifyMode = YGJustifySpaceBetween;
            break;
        case JustifyContent::SpaceAround:
            justifyMode = YGJustifySpaceAround;
            break;
        case JustifyContent::SpaceEvenly:
            justifyMode = YGJustifySpaceEvenly;
            break;
    }
    YGNodeStyleSetJustifyContent(node_, justifyMode);
#else
    justify_ = justify;
#endif
}

void LayoutNode::SetAlignItems(AlignItems align) {
#if RAYM3_USE_YOGA
    YGAlign alignMode;
    switch (align) {
        case AlignItems::FlexStart:
            alignMode = YGAlignFlexStart;
            break;
        case AlignItems::FlexEnd:
            alignMode = YGAlignFlexEnd;
            break;
        case AlignItems::Center:
            alignMode = YGAlignCenter;
            break;
        case AlignItems::Stretch:
            alignMode = YGAlignStretch;
            break;
        case AlignItems::Baseline:
            alignMode = YGAlignBaseline;
            break;
    }
    YGNodeStyleSetAlignItems(node_, alignMode);
#else
    align_ = align;
#endif
}

void LayoutNode::SetGap(float gap) {
#if RAYM3_USE_YOGA
    YGNodeStyleSetGap(node_, YGGutterColumn, gap);
    YGNodeStyleSetGap(node_, YGGutterRow, gap);
#else
    gap_ = gap;
#endif
}

void LayoutNode::SetPadding(float padding) {
#if RAYM3_USE_YOGA
    YGNodeStyleSetPadding(node_, YGEdgeAll, padding);
#else
    padding_[0] = padding_[1] = padding_[2] = padding_[3] = padding;
#endif
}

void LayoutNode::SetPadding(float top, float right, float bottom, float left) {
#if RAYM3_USE_YOGA
    YGNodeStyleSetPadding(node_, YGEdgeTop, top);
    YGNodeStyleSetPadding(node_, YGEdgeRight, right);
    YGNodeStyleSetPadding(node_, YGEdgeBottom, bottom);
    YGNodeStyleSetPadding(node_, YGEdgeLeft, left);
#else
    padding_[0] = top;
    padding_[1] = right;
    padding_[2] = bottom;
    padding_[3] = left;
#endif
}

void LayoutNode::SetMargin(float margin) {
#if RAYM3_USE_YOGA
    YGNodeStyleSetMargin(node_, YGEdgeAll, margin);
#else
    margin_[0] = margin_[1] = margin_[2] = margin_[3] = margin;
#endif
}

void LayoutNode::SetMargin(float top, float right, float bottom, float left) {
#if RAYM3_USE_YOGA
    YGNodeStyleSetMargin(node_, YGEdgeTop, top);
    YGNodeStyleSetMargin(node_, YGEdgeRight, right);
    YGNodeStyleSetMargin(node_, YGEdgeBottom, bottom);
    YGNodeStyleSetMargin(node_, YGEdgeLeft, left);
#else
    margin_[0] = top;
    margin_[1] = right;
    margin_[2] = bottom;
    margin_[3] = left;
#endif
}

void LayoutNode::SetWidth(float width) {
#if RAYM3_USE_YOGA
    YGNodeStyleSetWidth(node_, width);
#else
    bounds_.width = width;
#endif
}

void LayoutNode::SetHeight(float height) {
#if RAYM3_USE_YOGA
    YGNodeStyleSetHeight(node_, height);
#else
    bounds_.height = height;
#endif
}

void LayoutNode::SetMinWidth(float width) {
#if RAYM3_USE_YOGA
    YGNodeStyleSetMinWidth(node_, width);
#endif
}

void LayoutNode::SetMinHeight(float height) {
#if RAYM3_USE_YOGA
    YGNodeStyleSetMinHeight(node_, height);
#endif
}

void LayoutNode::SetMaxWidth(float width) {
#if RAYM3_USE_YOGA
    YGNodeStyleSetMaxWidth(node_, width);
#endif
}

void LayoutNode::SetMaxHeight(float height) {
#if RAYM3_USE_YOGA
    YGNodeStyleSetMaxHeight(node_, height);
#endif
}

void LayoutNode::AddChild(LayoutNode* child) {
#if RAYM3_USE_YOGA
    if (child && child->node_) {
        YGNodeInsertChild(node_, child->node_, YGNodeGetChildCount(node_));
    }
#endif
}

void LayoutNode::RemoveChild(LayoutNode* child) {
#if RAYM3_USE_YOGA
    if (child && child->node_) {
        YGNodeRemoveChild(node_, child->node_);
    }
#endif
}

void LayoutNode::RemoveAllChildren() {
#if RAYM3_USE_YOGA
    YGNodeRemoveAllChildren(node_);
#endif
}

void LayoutNode::CalculateLayout(float width, float height) {
#if RAYM3_USE_YOGA
    YGNodeCalculateLayout(node_, width, height, YGDirectionLTR);
#else
    bounds_.width = width;
    bounds_.height = height;
#endif
}

Rectangle LayoutNode::GetLayoutBounds() const {
#if RAYM3_USE_YOGA
    return {
        YGNodeLayoutGetLeft(node_),
        YGNodeLayoutGetTop(node_),
        YGNodeLayoutGetWidth(node_),
        YGNodeLayoutGetHeight(node_)
    };
#else
    return bounds_;
#endif
}

float LayoutNode::GetLayoutX() const {
#if RAYM3_USE_YOGA
    return YGNodeLayoutGetLeft(node_);
#else
    return bounds_.x;
#endif
}

float LayoutNode::GetLayoutY() const {
#if RAYM3_USE_YOGA
    return YGNodeLayoutGetTop(node_);
#else
    return bounds_.y;
#endif
}

float LayoutNode::GetLayoutWidth() const {
#if RAYM3_USE_YOGA
    return YGNodeLayoutGetWidth(node_);
#else
    return bounds_.width;
#endif
}

float LayoutNode::GetLayoutHeight() const {
#if RAYM3_USE_YOGA
    return YGNodeLayoutGetHeight(node_);
#else
    return bounds_.height;
#endif
}

} // namespace raym3

