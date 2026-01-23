/**
 * lite_layout.h - 基于Yoga的Flexbox布局基类
 */

#pragma once

#include "lite_common.h"
#include <yoga/YGNode.h>
#include <yoga/YGNodeStyle.h>
#include <yoga/YGNodeLayout.h>
#include <yoga/YGEnums.h>
#include <yoga/YGValue.h>
#include <memory>
#include <vector>

class SkCanvas;

namespace liteDui {

class LiteLayout;
using LiteLayoutPtr = std::shared_ptr<LiteLayout>;
using LiteLayoutWeakPtr = std::weak_ptr<LiteLayout>;

/**
 * LiteLayout - 布局基类
 */
class LiteLayout : public std::enable_shared_from_this<LiteLayout> {
public:
    LiteLayout();
    virtual ~LiteLayout();

    LiteLayout(const LiteLayout&) = delete;
    LiteLayout& operator=(const LiteLayout&) = delete;

    // 子节点管理
    void addChild(const LiteLayoutPtr& child);
    void removeChild(const LiteLayoutPtr& child);
    void removeAllChildren();
    size_t getChildCount() const;
    LiteLayoutPtr getChildAt(size_t index) const;
    LiteLayoutPtr getParent() const { return m_parent.lock(); }

    // 尺寸设置
    void setWidth(const LayoutValue& width);
    void setHeight(const LayoutValue& height);
    void setMinWidth(const LayoutValue& minWidth);
    void setMinHeight(const LayoutValue& minHeight);
    void setMaxWidth(const LayoutValue& maxWidth);
    void setMaxHeight(const LayoutValue& maxHeight);

    // Flex属性
    void setFlex(float flex);
    void setFlexGrow(float flexGrow);
    void setFlexShrink(float flexShrink);
    void setFlexBasis(const LayoutValue& flexBasis);

    // 布局方向和对齐
    void setFlexDirection(FlexDirection direction);
    void setJustifyContent(Align justify);
    void setAlignItems(Align align);
    void setAlignSelf(Align align);
    void setAlignContent(Align align);

    // 位置和定位
    void setPosition(PositionType positionType);

    // 边距和内边距
    void setMargin(const EdgeInsets& margin);
    void setPadding(const EdgeInsets& padding);
    void setBorder(const EdgeInsets& border);

    // 其他属性
    void setFlexWrap(FlexWrap wrap);
    void setOverflow(Overflow overflow);
    void setDisplay(Display display);
    void setGap(float gap);

    // 布局计算
    void calculateLayout(float width = -1, float height = -1);

    // 获取计算后的布局
    float getLeft() const;
    float getTop() const;
    float getLayoutWidth() const;
    float getLayoutHeight() const;
    float getLayoutPaddingLeft() const;
    float getLayoutPaddingTop() const;
    float getLayoutPaddingRight() const;
    float getLayoutPaddingBottom() const;
    float getLayoutBorderLeft() const;
    float getLayoutBorderTop() const;
    float getLayoutBorderRight() const;
    float getLayoutBorderBottom() const;

    // 脏标记
    bool isDirty() const { return m_dirty; }
    void markDirty();
    void clearDirty() { m_dirty = false; }
    void clearDirtyTree();

    // 更新逻辑（子类可重写，用于处理动画、光标闪烁等）
    virtual void update() {}
    void updateTree();

    // 渲染
    virtual void render(SkCanvas* canvas) {}
    void renderTree(SkCanvas* canvas);

    Display getDisplay() const;

protected:
    YGNodeRef m_yogaNode;
    std::vector<LiteLayoutPtr> m_children;
    LiteLayoutWeakPtr m_parent;
    bool m_dirty = true;

private:
    YGValue toYGValue(const LayoutValue& value) const;
};

} // namespace liteDui
