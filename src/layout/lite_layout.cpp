/**
 * lite_layout.cpp - 基于Yoga的Flexbox布局实现
 */

#include "lite_layout.h"
#include "include/core/SkCanvas.h"
#include <algorithm>

namespace liteDui {

LiteLayout::LiteLayout() {
    m_yogaNode = YGNodeNew();
    YGNodeSetContext(m_yogaNode, this);
}

LiteLayout::~LiteLayout() {
    removeAllChildren();
    if (m_yogaNode) {
        YGNodeFree(m_yogaNode);
        m_yogaNode = nullptr;
    }
}

// 子节点管理
void LiteLayout::addChild(const LiteLayoutPtr& child) {
    if (!child) return;
    
    auto oldParent = child->getParent();
    if (oldParent) {
        oldParent->removeChild(child);
    }
    
    m_children.push_back(child);
    child->m_parent = shared_from_this();
    YGNodeInsertChild(m_yogaNode, child->m_yogaNode, YGNodeGetChildCount(m_yogaNode));
    markDirty();
}

void LiteLayout::removeChild(const LiteLayoutPtr& child) {
    if (!child) return;
    
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        YGNodeRemoveChild(m_yogaNode, child->m_yogaNode);
        child->m_parent.reset();
        m_children.erase(it);
        markDirty();
    }
}

void LiteLayout::removeAllChildren() {
    YGNodeRemoveAllChildren(m_yogaNode);
    for (auto& child : m_children) {
        child->m_parent.reset();
    }
    m_children.clear();
    markDirty();
}

size_t LiteLayout::getChildCount() const {
    return m_children.size();
}

LiteLayoutPtr LiteLayout::getChildAt(size_t index) const {
    if (index >= m_children.size()) return nullptr;
    return m_children[index];
}

// 尺寸设置
void LiteLayout::setWidth(const LayoutValue& width) {
    YGValue v = toYGValue(width);
    if (v.unit == YGUnitPercent) {
        YGNodeStyleSetWidthPercent(m_yogaNode, v.value);
    } else if (v.unit == YGUnitAuto) {
        YGNodeStyleSetWidthAuto(m_yogaNode);
    } else {
        YGNodeStyleSetWidth(m_yogaNode, v.value);
    }
}

void LiteLayout::setHeight(const LayoutValue& height) {
    YGValue v = toYGValue(height);
    if (v.unit == YGUnitPercent) {
        YGNodeStyleSetHeightPercent(m_yogaNode, v.value);
    } else if (v.unit == YGUnitAuto) {
        YGNodeStyleSetHeightAuto(m_yogaNode);
    } else {
        YGNodeStyleSetHeight(m_yogaNode, v.value);
    }
}

void LiteLayout::setMinWidth(const LayoutValue& minWidth) {
    YGValue v = toYGValue(minWidth);
    if (v.unit == YGUnitPercent) {
        YGNodeStyleSetMinWidthPercent(m_yogaNode, v.value);
    } else {
        YGNodeStyleSetMinWidth(m_yogaNode, v.value);
    }
}

void LiteLayout::setMinHeight(const LayoutValue& minHeight) {
    YGValue v = toYGValue(minHeight);
    if (v.unit == YGUnitPercent) {
        YGNodeStyleSetMinHeightPercent(m_yogaNode, v.value);
    } else {
        YGNodeStyleSetMinHeight(m_yogaNode, v.value);
    }
}

void LiteLayout::setMaxWidth(const LayoutValue& maxWidth) {
    YGValue v = toYGValue(maxWidth);
    if (v.unit == YGUnitPercent) {
        YGNodeStyleSetMaxWidthPercent(m_yogaNode, v.value);
    } else {
        YGNodeStyleSetMaxWidth(m_yogaNode, v.value);
    }
}

void LiteLayout::setMaxHeight(const LayoutValue& maxHeight) {
    YGValue v = toYGValue(maxHeight);
    if (v.unit == YGUnitPercent) {
        YGNodeStyleSetMaxHeightPercent(m_yogaNode, v.value);
    } else {
        YGNodeStyleSetMaxHeight(m_yogaNode, v.value);
    }
}

// Flex属性
void LiteLayout::setFlex(float flex) {
    YGNodeStyleSetFlex(m_yogaNode, flex);
}

void LiteLayout::setFlexGrow(float flexGrow) {
    YGNodeStyleSetFlexGrow(m_yogaNode, flexGrow);
}

void LiteLayout::setFlexShrink(float flexShrink) {
    YGNodeStyleSetFlexShrink(m_yogaNode, flexShrink);
}

void LiteLayout::setFlexBasis(const LayoutValue& flexBasis) {
    YGValue v = toYGValue(flexBasis);
    if (v.unit == YGUnitPercent) {
        YGNodeStyleSetFlexBasisPercent(m_yogaNode, v.value);
    } else if (v.unit == YGUnitAuto) {
        YGNodeStyleSetFlexBasisAuto(m_yogaNode);
    } else {
        YGNodeStyleSetFlexBasis(m_yogaNode, v.value);
    }
}

// 布局方向和对齐
void LiteLayout::setFlexDirection(FlexDirection direction) {
    YGNodeStyleSetFlexDirection(m_yogaNode, static_cast<YGFlexDirection>(static_cast<int>(direction)));
}

void LiteLayout::setJustifyContent(Align justify) {
    YGNodeStyleSetJustifyContent(m_yogaNode, static_cast<YGJustify>(static_cast<int>(justify)));
}

void LiteLayout::setAlignItems(Align align) {
    YGNodeStyleSetAlignItems(m_yogaNode, static_cast<YGAlign>(static_cast<int>(align)));
}

void LiteLayout::setAlignSelf(Align align) {
    YGNodeStyleSetAlignSelf(m_yogaNode, static_cast<YGAlign>(static_cast<int>(align)));
}

void LiteLayout::setAlignContent(Align align) {
    YGNodeStyleSetAlignContent(m_yogaNode, static_cast<YGAlign>(static_cast<int>(align)));
}

// 位置和定位
void LiteLayout::setPosition(PositionType positionType) {
    YGNodeStyleSetPositionType(m_yogaNode, static_cast<YGPositionType>(positionType));
}

// 边距和内边距
void LiteLayout::setMargin(const EdgeInsets& margin) {
    YGNodeStyleSetMargin(m_yogaNode, YGEdgeLeft, margin.left.value);
    YGNodeStyleSetMargin(m_yogaNode, YGEdgeTop, margin.top.value);
    YGNodeStyleSetMargin(m_yogaNode, YGEdgeRight, margin.right.value);
    YGNodeStyleSetMargin(m_yogaNode, YGEdgeBottom, margin.bottom.value);
}

void LiteLayout::setPadding(const EdgeInsets& padding) {
    YGNodeStyleSetPadding(m_yogaNode, YGEdgeLeft, padding.left.value);
    YGNodeStyleSetPadding(m_yogaNode, YGEdgeTop, padding.top.value);
    YGNodeStyleSetPadding(m_yogaNode, YGEdgeRight, padding.right.value);
    YGNodeStyleSetPadding(m_yogaNode, YGEdgeBottom, padding.bottom.value);
}

void LiteLayout::setBorder(const EdgeInsets& border) {
    YGNodeStyleSetBorder(m_yogaNode, YGEdgeLeft, border.left.value);
    YGNodeStyleSetBorder(m_yogaNode, YGEdgeTop, border.top.value);
    YGNodeStyleSetBorder(m_yogaNode, YGEdgeRight, border.right.value);
    YGNodeStyleSetBorder(m_yogaNode, YGEdgeBottom, border.bottom.value);
}

// 其他属性
void LiteLayout::setFlexWrap(FlexWrap wrap) {
    YGNodeStyleSetFlexWrap(m_yogaNode, static_cast<YGWrap>(static_cast<int>(wrap)));
}

void LiteLayout::setOverflow(Overflow overflow) {
    YGNodeStyleSetOverflow(m_yogaNode, static_cast<YGOverflow>(static_cast<int>(overflow)));
}

void LiteLayout::setDisplay(Display display) {
    YGNodeStyleSetDisplay(m_yogaNode, static_cast<YGDisplay>(static_cast<int>(display)));
}

void LiteLayout::setGap(float gap) {
    YGNodeStyleSetGap(m_yogaNode, YGGutterAll, gap);
}

// 布局计算
void LiteLayout::calculateLayout(float width, float height) {
    float w = width < 0 ? YGUndefined : width;
    float h = height < 0 ? YGUndefined : height;
    YGNodeCalculateLayout(m_yogaNode, w, h, YGDirectionLTR);
}

// 获取计算后的布局
float LiteLayout::getLeft() const {
    return YGNodeLayoutGetLeft(m_yogaNode);
}

float LiteLayout::getTop() const {
    return YGNodeLayoutGetTop(m_yogaNode);
}

float LiteLayout::getLayoutWidth() const {
    return YGNodeLayoutGetWidth(m_yogaNode);
}

float LiteLayout::getLayoutHeight() const {
    return YGNodeLayoutGetHeight(m_yogaNode);
}

float LiteLayout::getLayoutPaddingLeft() const {
    return YGNodeLayoutGetPadding(m_yogaNode, YGEdgeLeft);
}

float LiteLayout::getLayoutPaddingTop() const {
    return YGNodeLayoutGetPadding(m_yogaNode, YGEdgeTop);
}

float LiteLayout::getLayoutPaddingRight() const {
    return YGNodeLayoutGetPadding(m_yogaNode, YGEdgeRight);
}

float LiteLayout::getLayoutPaddingBottom() const {
    return YGNodeLayoutGetPadding(m_yogaNode, YGEdgeBottom);
}

float LiteLayout::getLayoutBorderLeft() const {
    return YGNodeLayoutGetBorder(m_yogaNode, YGEdgeLeft);
}

float LiteLayout::getLayoutBorderTop() const {
    return YGNodeLayoutGetBorder(m_yogaNode, YGEdgeTop);
}

float LiteLayout::getLayoutBorderRight() const {
    return YGNodeLayoutGetBorder(m_yogaNode, YGEdgeRight);
}

float LiteLayout::getLayoutBorderBottom() const {
    return YGNodeLayoutGetBorder(m_yogaNode, YGEdgeBottom);
}

Display LiteLayout::getDisplay() const {
    return static_cast<Display>(YGNodeStyleGetDisplay(m_yogaNode));
}

// 渲染树
void LiteLayout::renderTree(SkCanvas* canvas) {
    if (!canvas) return;
    
    canvas->save();
    canvas->translate(getLeft(), getTop());
    
    render(canvas);
    
    for (const auto& child : m_children) {
        if (child->getDisplay() != Display::None) {
            child->renderTree(canvas);
        }
    }
    
    canvas->restore();
}

// 辅助函数
YGValue LiteLayout::toYGValue(const LayoutValue& value) const {
    if (value.isAuto) {
        return YGValueAuto;
    } else if (value.isPercent) {
        return {value.value, YGUnitPercent};
    } else {
        return {value.value, YGUnitPoint};
    }
}

} // namespace liteDui
