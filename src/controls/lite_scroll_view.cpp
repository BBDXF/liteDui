/**
 * lite_scroll_view.cpp - 可滚动区域控件实现
 */

#include "lite_scroll_view.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRRect.h"
#include <algorithm>

namespace liteDui {

LiteScrollView::LiteScrollView() {
    setOverflow(Overflow::Hidden);
    setBackgroundColor(Color::White());
    setBorderColor(Color::LightGray());
    setBorder(EdgeInsets::All(1.0f));
}

void LiteScrollView::setScrollDirection(ScrollDirection direction) {
    m_scrollDirection = direction;
    clampScroll();
    markDirty();
}

void LiteScrollView::setShowScrollbar(bool show) {
    m_showScrollbar = show;
    markDirty();
}

void LiteScrollView::setScrollbarWidth(float width) {
    m_scrollbarWidth = width;
    markDirty();
}

void LiteScrollView::scrollTo(float x, float y) {
    m_scrollX = x;
    m_scrollY = y;
    clampScroll();
    markDirty();
}

void LiteScrollView::scrollBy(float dx, float dy) {
    m_scrollX += dx;
    m_scrollY += dy;
    clampScroll();
    markDirty();
}

float LiteScrollView::getContentWidth() const {
    float maxWidth = 0;
    for (size_t i = 0; i < getChildCount(); ++i) {
        auto child = getChildAt(i);
        if (child && child->getDisplay() != Display::None) {
            // 子控件的布局宽度已经包含了 margin，所以直接使用布局宽度
            // 但需要考虑子控件的 left 位置（由于 padding 等因素）
            float childRight = child->getLeft() + child->getLayoutWidth();
            maxWidth = std::max(maxWidth, childRight);
        }
    }
    // 如果内容宽度小于视口宽度，使用视口宽度
    return std::max(maxWidth, getViewportWidth());
}

float LiteScrollView::getContentHeight() const {
    float maxHeight = 0;
    for (size_t i = 0; i < getChildCount(); ++i) {
        auto child = getChildAt(i);
        if (child && child->getDisplay() != Display::None) {
            // 子控件的布局高度已经包含了 margin，所以直接使用布局高度
            // 但需要考虑子控件的 top 位置（由于 padding 等因素）
            float childBottom = child->getTop() + child->getLayoutHeight();
            maxHeight = std::max(maxHeight, childBottom);
        }
    }
    // 如果内容高度小于视口高度，使用视口高度
    return std::max(maxHeight, getViewportHeight());
}

float LiteScrollView::getViewportWidth() const {
    float w = getLayoutWidth() - getLayoutBorderLeft() - getLayoutBorderRight()
              - getLayoutPaddingLeft() - getLayoutPaddingRight();
    if (m_showScrollbar && (m_scrollDirection == ScrollDirection::Vertical || 
                            m_scrollDirection == ScrollDirection::Both)) {
        w -= m_scrollbarWidth;
    }
    return std::max(0.0f, w);
}

float LiteScrollView::getViewportHeight() const {
    float h = getLayoutHeight() - getLayoutBorderTop() - getLayoutBorderBottom()
              - getLayoutPaddingTop() - getLayoutPaddingBottom();
    if (m_showScrollbar && (m_scrollDirection == ScrollDirection::Horizontal || 
                            m_scrollDirection == ScrollDirection::Both)) {
        h -= m_scrollbarWidth;
    }
    return std::max(0.0f, h);
}

void LiteScrollView::clampScroll() {
    float contentW = getContentWidth();
    float contentH = getContentHeight();
    float viewportW = getViewportWidth();
    float viewportH = getViewportHeight();

    float maxScrollX = std::max(0.0f, contentW - viewportW);
    float maxScrollY = std::max(0.0f, contentH - viewportH);

    m_scrollX = std::max(0.0f, std::min(m_scrollX, maxScrollX));
    m_scrollY = std::max(0.0f, std::min(m_scrollY, maxScrollY));
}

void LiteScrollView::render(SkCanvas* canvas) {
    if (!canvas) return;

    float w = getLayoutWidth();
    float h = getLayoutHeight();
    if (w <= 0 || h <= 0) return;

    // 绘制背景和边框
    drawBackground(canvas, 0, 0, w, h);
    drawBorder(canvas, 0, 0, w, h);

    float borderL = getLayoutBorderLeft();
    float borderT = getLayoutBorderTop();
    float borderR = getLayoutBorderRight();
    float borderB = getLayoutBorderBottom();
    float padL = getLayoutPaddingLeft();
    float padT = getLayoutPaddingTop();
    float padR = getLayoutPaddingRight();
    float padB = getLayoutPaddingBottom();

    float contentX = borderL + padL;
    float contentY = borderT + padT;
    float viewportW = getViewportWidth();
    float viewportH = getViewportHeight();

    // 保存画布状态
    canvas->save();

    // 裁剪到视口区域
    SkRect clipRect = SkRect::MakeXYWH(contentX, contentY, viewportW, viewportH);
    canvas->clipRect(clipRect);

    // 应用滚动偏移
    canvas->translate(contentX - m_scrollX, contentY - m_scrollY);

    // 渲染内容
    renderContent(canvas);

    // 恢复画布状态
    canvas->restore();

    // 绘制滚动条
    if (m_showScrollbar) {
        drawScrollbar(canvas);
    }
}

void LiteScrollView::renderContent(SkCanvas* canvas) {
    // 渲染子控件
    for (size_t i = 0; i < getChildCount(); ++i) {
        auto child = getChildAt(i);
        if (child && child->getDisplay() != Display::None) {
            canvas->save();
            canvas->translate(child->getLeft(), child->getTop());
            // 使用 renderTree 而不是 render，以便正确渲染子控件的子控件
            child->renderTree(canvas);
            canvas->restore();
        }
    }
}

void LiteScrollView::drawScrollbar(SkCanvas* canvas) {
    float w = getLayoutWidth();
    float h = getLayoutHeight();
    float borderL = getLayoutBorderLeft();
    float borderT = getLayoutBorderTop();
    float borderR = getLayoutBorderRight();
    float borderB = getLayoutBorderBottom();
    float padL = getLayoutPaddingLeft();
    float padT = getLayoutPaddingTop();

    float contentX = borderL + padL;
    float contentY = borderT + padT;

    // 垂直滚动条
    if (m_scrollDirection == ScrollDirection::Vertical || 
        m_scrollDirection == ScrollDirection::Both) {
        float scrollbarX = w - borderR - m_scrollbarWidth;
        float scrollbarY = contentY;
        float scrollbarH = getViewportHeight();
        drawVerticalScrollbar(canvas, scrollbarX, scrollbarY, m_scrollbarWidth, scrollbarH);
    }

    // 水平滚动条
    if (m_scrollDirection == ScrollDirection::Horizontal || 
        m_scrollDirection == ScrollDirection::Both) {
        float scrollbarX = contentX;
        float scrollbarY = h - borderB - m_scrollbarWidth;
        float scrollbarW = getViewportWidth();
        drawHorizontalScrollbar(canvas, scrollbarX, scrollbarY, scrollbarW, m_scrollbarWidth);
    }
}

void LiteScrollView::drawVerticalScrollbar(SkCanvas* canvas, float x, float y, float w, float h) {
    float contentH = getContentHeight();
    float viewportH = getViewportHeight();
    
    if (contentH <= viewportH) return; // 不需要滚动条

    // 绘制轨道
    SkPaint trackPaint;
    trackPaint.setColor(m_scrollbarTrackColor.toARGB());
    trackPaint.setStyle(SkPaint::kFill_Style);
    trackPaint.setAntiAlias(true);
    
    SkRRect trackRRect = SkRRect::MakeRectXY(
        SkRect::MakeXYWH(x, y, w, h), w / 2, w / 2);
    canvas->drawRRect(trackRRect, trackPaint);

    // 计算滑块大小和位置
    float thumbRatio = viewportH / contentH;
    float thumbH = std::max(20.0f, h * thumbRatio);
    float scrollRange = h - thumbH;
    float maxScroll = contentH - viewportH;
    float thumbY = y + (maxScroll > 0 ? (m_scrollY / maxScroll) * scrollRange : 0);

    // 绘制滑块
    SkPaint thumbPaint;
    thumbPaint.setColor(m_scrollbarColor.toARGB());
    thumbPaint.setStyle(SkPaint::kFill_Style);
    thumbPaint.setAntiAlias(true);

    SkRRect thumbRRect = SkRRect::MakeRectXY(
        SkRect::MakeXYWH(x + 1, thumbY, w - 2, thumbH), (w - 2) / 2, (w - 2) / 2);
    canvas->drawRRect(thumbRRect, thumbPaint);
}

void LiteScrollView::drawHorizontalScrollbar(SkCanvas* canvas, float x, float y, float w, float h) {
    float contentW = getContentWidth();
    float viewportW = getViewportWidth();
    
    if (contentW <= viewportW) return; // 不需要滚动条

    // 绘制轨道
    SkPaint trackPaint;
    trackPaint.setColor(m_scrollbarTrackColor.toARGB());
    trackPaint.setStyle(SkPaint::kFill_Style);
    trackPaint.setAntiAlias(true);
    
    SkRRect trackRRect = SkRRect::MakeRectXY(
        SkRect::MakeXYWH(x, y, w, h), h / 2, h / 2);
    canvas->drawRRect(trackRRect, trackPaint);

    // 计算滑块大小和位置
    float thumbRatio = viewportW / contentW;
    float thumbW = std::max(20.0f, w * thumbRatio);
    float scrollRange = w - thumbW;
    float maxScroll = contentW - viewportW;
    float thumbX = x + (maxScroll > 0 ? (m_scrollX / maxScroll) * scrollRange : 0);

    // 绘制滑块
    SkPaint thumbPaint;
    thumbPaint.setColor(m_scrollbarColor.toARGB());
    thumbPaint.setStyle(SkPaint::kFill_Style);
    thumbPaint.setAntiAlias(true);

    SkRRect thumbRRect = SkRRect::MakeRectXY(
        SkRect::MakeXYWH(thumbX, y + 1, thumbW, h - 2), (h - 2) / 2, (h - 2) / 2);
    canvas->drawRRect(thumbRRect, thumbPaint);
}

bool LiteScrollView::isPointInVerticalScrollbar(float x, float y) const {
    if (!m_showScrollbar) return false;
    if (m_scrollDirection != ScrollDirection::Vertical && 
        m_scrollDirection != ScrollDirection::Both) return false;

    float w = getLayoutWidth();
    float borderR = getLayoutBorderRight();
    float borderT = getLayoutBorderTop();
    float padT = getLayoutPaddingTop();

    float scrollbarX = w - borderR - m_scrollbarWidth;
    float scrollbarY = borderT + padT;
    float scrollbarH = getViewportHeight();

    return x >= scrollbarX && x < scrollbarX + m_scrollbarWidth &&
           y >= scrollbarY && y < scrollbarY + scrollbarH;
}

bool LiteScrollView::isPointInHorizontalScrollbar(float x, float y) const {
    if (!m_showScrollbar) return false;
    if (m_scrollDirection != ScrollDirection::Horizontal && 
        m_scrollDirection != ScrollDirection::Both) return false;

    float h = getLayoutHeight();
    float borderL = getLayoutBorderLeft();
    float borderB = getLayoutBorderBottom();
    float padL = getLayoutPaddingLeft();

    float scrollbarX = borderL + padL;
    float scrollbarY = h - borderB - m_scrollbarWidth;
    float scrollbarW = getViewportWidth();

    return x >= scrollbarX && x < scrollbarX + scrollbarW &&
           y >= scrollbarY && y < scrollbarY + m_scrollbarWidth;
}

void LiteScrollView::onScroll(const ScrollEvent& event) {
    float scrollSpeed = 30.0f; // 每次滚轮滚动的像素数

    if (m_scrollDirection == ScrollDirection::Vertical ||
        m_scrollDirection == ScrollDirection::Both) {
        scrollBy(0, -event.yoffset * scrollSpeed);
    }
    if (m_scrollDirection == ScrollDirection::Horizontal ||
        m_scrollDirection == ScrollDirection::Both) {
        scrollBy(-event.xoffset * scrollSpeed, 0);
    }
}

void LiteScrollView::onMousePressed(const MouseEvent& event) {
    if (event.button != MouseButton::Left) return;

    // 检查是否点击了垂直滚动条
    if (isPointInVerticalScrollbar(event.x, event.y)) {
        m_draggingVScrollbar = true;
        m_dragStartPos = event.y;
        m_dragStartScroll = m_scrollY;
        return;
    }

    // 检查是否点击了水平滚动条
    if (isPointInHorizontalScrollbar(event.x, event.y)) {
        m_draggingHScrollbar = true;
        m_dragStartPos = event.x;
        m_dragStartScroll = m_scrollX;
        return;
    }

    // 传递给父类处理
    LiteContainer::onMousePressed(event);
}

void LiteScrollView::onMouseReleased(const MouseEvent& event) {
    m_draggingVScrollbar = false;
    m_draggingHScrollbar = false;
    LiteContainer::onMouseReleased(event);
}

void LiteScrollView::onMouseMoved(const MouseEvent& event) {
    if (m_draggingVScrollbar) {
        float contentH = getContentHeight();
        float viewportH = getViewportHeight();
        if (contentH > viewportH) {
            float thumbRatio = viewportH / contentH;
            float thumbH = std::max(20.0f, viewportH * thumbRatio);
            float scrollRange = viewportH - thumbH;
            float maxScroll = contentH - viewportH;
            
            float deltaY = event.y - m_dragStartPos;
            float scrollDelta = (scrollRange > 0) ? (deltaY / scrollRange) * maxScroll : 0;
            m_scrollY = m_dragStartScroll + scrollDelta;
            clampScroll();
            markDirty();
        }
        return;
    }

    if (m_draggingHScrollbar) {
        float contentW = getContentWidth();
        float viewportW = getViewportWidth();
        if (contentW > viewportW) {
            float thumbRatio = viewportW / contentW;
            float thumbW = std::max(20.0f, viewportW * thumbRatio);
            float scrollRange = viewportW - thumbW;
            float maxScroll = contentW - viewportW;
            
            float deltaX = event.x - m_dragStartPos;
            float scrollDelta = (scrollRange > 0) ? (deltaX / scrollRange) * maxScroll : 0;
            m_scrollX = m_dragStartScroll + scrollDelta;
            clampScroll();
            markDirty();
        }
        return;
    }

    LiteContainer::onMouseMoved(event);
}

} // namespace liteDui
