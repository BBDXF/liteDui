/**
 * lite_scroll_view.h - 可滚动区域控件
 * 
 * 使用 canvas 裁剪管理子控件的显示区域，支持垂直和水平滚动
 */

#pragma once

#include "lite_container.h"

namespace liteDui {

class LiteScrollView;
using LiteScrollViewPtr = std::shared_ptr<LiteScrollView>;

/**
 * 滚动方向枚举
 */
enum class ScrollDirection {
    Vertical,       // 仅垂直滚动
    Horizontal,     // 仅水平滚动
    Both            // 双向滚动
};

/**
 * LiteScrollView - 可滚动区域控件
 * 
 * 使用 canvas 裁剪技术管理子控件的显示区域，支持垂直和水平滚动。
 * 通过 clipRect 限制子控件的渲染范围，实现滚动效果。
 */
class LiteScrollView : public LiteContainer {
public:
    LiteScrollView();
    ~LiteScrollView() override = default;

    // 滚动方向设置
    void setScrollDirection(ScrollDirection direction);
    ScrollDirection getScrollDirection() const { return m_scrollDirection; }

    // 滚动条设置
    void setShowScrollbar(bool show);
    bool isShowScrollbar() const { return m_showScrollbar; }
    void setScrollbarWidth(float width);
    float getScrollbarWidth() const { return m_scrollbarWidth; }
    void setScrollbarColor(const Color& color) { m_scrollbarColor = color; markDirty(); }
    void setScrollbarTrackColor(const Color& color) { m_scrollbarTrackColor = color; markDirty(); }

    // 滚动位置控制
    void scrollTo(float x, float y);
    void scrollBy(float dx, float dy);
    float getScrollX() const { return m_scrollX; }
    float getScrollY() const { return m_scrollY; }

    // 获取内容尺寸（虚函数，子类可重写以自定义内容大小计算）
    virtual float getContentWidth() const;
    virtual float getContentHeight() const;

    // 获取视口尺寸（可见区域大小）
    float getViewportWidth() const;
    float getViewportHeight() const;

    // 重写渲染方法
    void render(SkCanvas* canvas) override;
    void renderTree(SkCanvas* canvas) override;

    // 事件处理
    void onScroll(const ScrollEvent& event) override;
    void onMousePressed(const MouseEvent& event) override;
    void onMouseReleased(const MouseEvent& event) override;
    void onMouseMoved(const MouseEvent& event) override;

protected:
    /**
     * 渲染内容区域（子类可重写以自定义内容渲染）
     * 此方法在 canvas 已经应用裁剪和滚动偏移后调用
     */
    virtual void renderContent(SkCanvas* canvas);
    
    /**
     * 限制滚动范围，确保滚动位置在有效范围内
     */
    void clampScroll();

    /**
     * 计算滚动条的可见性
     */
    bool needVerticalScrollbar() const;
    bool needHorizontalScrollbar() const;

    // 滚动位置
    float m_scrollX = 0.0f;
    float m_scrollY = 0.0f;

    // 滚动条绘制方法 - 子类可访问
    void drawScrollbar(SkCanvas* canvas);
    void drawVerticalScrollbar(SkCanvas* canvas, float x, float y, float w, float h);
    void drawHorizontalScrollbar(SkCanvas* canvas, float x, float y, float w, float h);
    
    // 滚动条交互检测
    bool isPointInVerticalScrollbar(float x, float y) const;
    bool isPointInHorizontalScrollbar(float x, float y) const;
    SkRect getVerticalScrollbarThumbRect() const;
    SkRect getHorizontalScrollbarThumbRect() const;

    // 滚动条属性 - 子类可访问
    ScrollDirection m_scrollDirection = ScrollDirection::Vertical;
    bool m_showScrollbar = true;
    float m_scrollbarWidth = 8.0f;
    Color m_scrollbarColor = Color::fromRGB(150, 150, 150);
    Color m_scrollbarTrackColor = Color::fromRGB(230, 230, 230);

private:
    // 滚动条拖拽状态
    bool m_draggingVScrollbar = false;
    bool m_draggingHScrollbar = false;
    float m_dragStartPos = 0.0f;
    float m_dragStartScroll = 0.0f;
};

} // namespace liteDui
