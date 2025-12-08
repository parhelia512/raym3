#pragma once

#include <raylib.h>
#include "raym3/types.h"

#ifndef RAYM3_USE_YOGA
#define RAYM3_USE_YOGA 0
#endif

#if RAYM3_USE_YOGA
#include <yoga/Yoga.h>
#endif

namespace raym3 {

class LayoutNode {
public:
    LayoutNode();
    ~LayoutNode();
    
    LayoutNode(const LayoutNode&) = delete;
    LayoutNode& operator=(const LayoutNode&) = delete;
    
    LayoutNode(LayoutNode&& other) noexcept;
    LayoutNode& operator=(LayoutNode&& other) noexcept;
    
#if RAYM3_USE_YOGA
    YGNodeRef GetNode() const { return node_; }
#else
    void* GetNode() const { return node_; }
#endif
    
    void SetDirection(LayoutDirection direction);
    void SetJustifyContent(JustifyContent justify);
    void SetAlignItems(AlignItems align);
    void SetGap(float gap);
    void SetPadding(float padding);
    void SetPadding(float top, float right, float bottom, float left);
    void SetMargin(float margin);
    void SetMargin(float top, float right, float bottom, float left);
    
    void SetWidth(float width);
    void SetHeight(float height);
    void SetMinWidth(float width);
    void SetMinHeight(float height);
    void SetMaxWidth(float width);
    void SetMaxHeight(float height);
    
    void AddChild(LayoutNode* child);
    void RemoveChild(LayoutNode* child);
    void RemoveAllChildren();
    
    void CalculateLayout(float width, float height);
    
    Rectangle GetLayoutBounds() const;
    float GetLayoutX() const;
    float GetLayoutY() const;
    float GetLayoutWidth() const;
    float GetLayoutHeight() const;
    
private:
#if RAYM3_USE_YOGA
    YGNodeRef node_;
#else
    void* node_;
    Rectangle bounds_;
    LayoutDirection direction_;
    JustifyContent justify_;
    AlignItems align_;
    float gap_;
    float padding_[4];
    float margin_[4];
#endif
};

} // namespace raym3
