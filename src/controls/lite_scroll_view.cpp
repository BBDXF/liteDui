/**
 * lite_scroll_view.cpp - 可滚动区域控件实现
 * 
 * 使用 canvas 裁剪技术管理子控件的显示区域
 */

#include "lite_scroll_view.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRRect.h"
#include <algorithm>

namespace liteDui {

LiteScrollView::LiteScrollView() {
    // 设置溢出为隐藏，确保子控件不会超出边界
    setOverflow(Overflow::Hidden);
    setBackgroundColor(Color::White());
    setBorderColor(Color::LightGray());
    setBorder(EdgeInsets::All(1.0f));
}

void LiteScrollView::setScrollDirection(ScrollDirection direction) {
    if (m_scrollDirection == direction) return;
    m_scrollDirection = direction;
    
    // 根据滚动方向重置不支持的滚动位置
    if (direction == ScrollDirection::Vertical) {
        m_scrollX = 0.0f;
    } else if (direction == ScrollDirection::Horizontal) {
        m_scrollY = 0.0f;
    }
    
    clampScroll();
    markDirty();
}

void LiteScrollView::setShowScrollbar(bool show) {
    if (m_showScrollbar == show) return;
    m_showScrollbar = show;
    markDirty();
}

void LiteScrollView::setScrollbarWidth(float width) {
    if (m_scrollbarWidth == width) return;
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
            // 计算子控件的右边界（包括位置和尺寸）
            float childRight = child->getLeft() + child->getLayoutWidth();
            maxWidth = std::max(maxWidth, childRight);
        }
    }
    // 内容宽度至少等于视口宽度
    return std::max(maxWidth, getViewportWidth());
}

float LiteScrollView::getContentHeight() const {
    float maxHeight = 0;
    for (size_t i = 0; i < getChildCount(); ++i) {
        auto child = getChildAt(i);
        if (child && child->getDisplay() != Display::None) {
            // 计算子控件的底边界（包括位置和尺寸）
            float childBottom = child->getTop() + child->getLayoutHeight();
            maxHeight = std::max(maxHeight, childBottom);
        }
    }
    // 内容高度至少等于视口高度
    return std::max(maxHeight, getViewportHeight());
}

float LiteScrollView::getViewportWidth() const {
    float w = getLayoutWidth() - getLayoutBorderLeft() - getLayoutBorderRight()
              - getLayoutPaddingLeft() - getLayoutPaddingRight();
    
    // 如果显示垂直滚动条，需要减去滚动条宽度
    if (m_showScrollbar && needVerticalScrollbar()) {
        w -= m_scrollbarWidth;
    }
    
    return std::max(0.0f, w);
}

float LiteScrollView::getViewportHeight() const {
    float h = getLayoutHeight() - getLayoutBorderTop() - getLayoutBorderBottom()
              - getLayoutPaddingTop() - getLayoutPaddingBottom();
    
    // 如果显示水平滚动条，需要减去滚动条高度
    if (m_showScrollbar && needHorizontalScrollbar()) {
        h -= m_scrollbarWidth;
    }
    
    return std::max(0.0f, h);
}

bool LiteScrollView::needVerticalScrollbar() const {
    if (m_scrollDirection != ScrollDirection::Vertical && 
        m_scrollDirection != ScrollDirection::Both) {
        return false;
    }
    
    float contentH = getContentHeight();
    float viewportH = getLayoutHeight() - getLayoutBorderTop() - getLayoutBorderBottom()
                      - getLayoutPaddingTop() - getLayoutPaddingBottom();
    
    return contentH > viewportH;
}

bool LiteScrollView::needHorizontalScrollbar() const {
    if (m_scrollDirection != ScrollDirection::Horizontal && 
        m_scrollDirection != ScrollDirection::Both) {
        return false;
    }
    
    float contentW = getContentWidth();
    float viewportW = getLayoutWidth() - getLayoutBorderLeft() - getLayoutBorderRight()
                      - getLayoutPaddingLeft() - getLayoutPaddingRight();
    
    return contentW > viewportW;
}

void LiteScrollView::clampScroll() {
    float contentW = getContentWidth();
    float contentH = getContentHeight();
    float viewportW = getViewportWidth();
    float viewportH = getViewportHeight();

    // 计算最大滚动距离
    float maxScrollX = std::max(0.0f, contentW - viewportW);
    float maxScrollY = std::max(0.0f, contentH - viewportH);

    // 限制滚动位置在有效范围内
    m_scrollX = std::max(0.0f, std::min(m_scrollX, maxScrollX));
    m_scrollY = std::max(0.0f, std::min(m_scrollY, maxScrollY));
    
    // 根据滚动方向强制某些方向的滚动为0
    if (m_scrollDirection == ScrollDirection::Vertical) {
        m_scrollX = 0.0f;
    } else if (m_scrollDirection == ScrollDirection::Horizontal) {
        m_scrollY = 0.0f;
    }
}

void LiteScrollView::render(SkCanvas* canvas) {
    if (!canvas) return;

    float w = getLayoutWidth();
    float h = getLayoutHeight();
    if (w <= 0 || h <= 0) return;

    // 绘制背景和边框
    drawBackground(canvas, 0, 0, w, h);
    drawBorder(canvas, 0, 0, w, h);
}

void LiteScrollView::renderTree(SkCanvas* canvas) {
    if (!canvas) return;

    // 保存 canvas 状态并平移到控件位置
    canvas->save();
    canvas->translate(getLeft(), getTop());

    // 渲染背景和边框
    render(canvas);

    // 计算内容区域的位置和尺寸
    float borderL = getLayoutBorderLeft();
    float borderT = getLayoutBorderTop();
    float padL = getLayoutPaddingLeft();
    float padT = getLayoutPaddingTop();

    float contentX = borderL + padL;
    float contentY = borderT + padT;
    float viewportW = getViewportWidth();
    float viewportH = getViewportHeight();

    // 使用 canvas 裁剪限制内容渲染区域
    canvas->save();
    
    // 创建裁剪矩形，限制子控件只在视口内可见
    SkRect clipRect = SkRect::MakeXYWH(contentX, contentY, viewportW, viewportH);
    canvas->clipRect(clipRect, SkClipOp::kIntersect, true);
    
    // 平移 canvas 以实现滚动效果
    // 注意：滚动是负方向的，向下滚动时内容向上移动
    canvas->translate(contentX - m_scrollX, contentY - m_scrollY);

    // 渲染内容（子控件或自定义内容）
    renderContent(canvas);

    // 恢复 canvas 状态（取消裁剪和平移）
    canvas->restore();

    // 绘制滚动条（在裁剪区域之外）
    if (m_showScrollbar) {
        drawScrollbar(canvas);
    }

    // 恢复最外层 canvas 状态
    canvas->restore();
}

void LiteScrollView::renderContent(SkCanvas* canvas) {
    // 默认实现：渲染所有子控件
    for (size_t i = 0; i < getChildCount(); ++i) {
        auto child = getChildAt(i);
        if (child && child->getDisplay() != Display::None) {
            // 子控件的渲染已经在裁剪和滚动偏移的 canvas 上下文中
            child->renderTree(canvas);
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
    float padT = getLayoutPaddingTop();
    float padL = getLayoutPaddingLeft();

    float contentY = borderT + padT;
    float contentX = borderL + padL;

    // 绘制垂直滚动条
    if (needVerticalScrollbar()) {
        float scrollbarX = w - borderR - m_scrollbarWidth;
        float scrollbarY = contentY;
        float scrollbarH = getViewportHeight();
        drawVerticalScrollbar(canvas, scrollbarX, scrollbarY, m_scrollbarWidth, scrollbarH);
    }

    // 绘制水平滚动条
    if (needHorizontalScrollbar()) {
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

    // 绘制轨道背景
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

    // 绘制轨道背景
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

SkRect LiteScrollView::getVerticalScrollbarThumbRect() const {
    if (!needVerticalScrollbar()) return SkRect::MakeEmpty();
    
    float w = getLayoutWidth();
    float borderR = getLayoutBorderRight();
    float borderT = getLayoutBorderTop();
    float padT = getLayoutPaddingTop();
    
    float scrollbarX = w - borderR - m_scrollbarWidth;
    float scrollbarY = borderT + padT;
    float scrollbarH = getViewportHeight();
    
    float contentH = getContentHeight();
    float viewportH = getViewportHeight();
    float thumbRatio = viewportH / contentH;
    float thumbH = std::max(20.0f, scrollbarH * thumbRatio);
    float scrollRange = scrollbarH - thumbH;
    float maxScroll = contentH - viewportH;
    float thumbY = scrollbarY + (maxScroll > 0 ? (m_scrollY / maxScroll) * scrollRange : 0);
    
    return SkRect::MakeXYWH(scrollbarX, thumbY, m_scrollbarWidth, thumbH);
}

SkRect LiteScrollView::getHorizontalScrollbarThumbRect() const {
    if (!needHorizontalScrollbar()) return SkRect::MakeEmpty();
    
    float h = getLayoutHeight();
    float borderL = getLayoutBorderLeft();
    float borderB = getLayoutBorderBottom();
    float padL = getLayoutPaddingLeft();
    
    float scrollbarX = borderL + padL;
    float scrollbarY = h - borderB - m_scrollbarWidth;
    float scrollbarW = getViewportWidth();
    
    float contentW = getContentWidth();
    float viewportW = getViewportWidth();
    float thumbRatio = viewportW / contentW;
    float thumbW = std::max(20.0f, scrollbarW * thumbRatio);
    float scrollRange = scrollbarW - thumbW;
    float maxScroll = contentW - viewportW;
    float thumbX = scrollbarX + (maxScroll > 0 ? (m_scrollX / maxScroll) * scrollRange : 0);
    
    return SkRect::MakeXYWH(thumbX, scrollbarY, thumbW, m_scrollbarWidth);
}

bool LiteScrollView::isPointInVerticalScrollbar(float x, float y) const {
    if (!m_showScrollbar || !needVerticalScrollbar()) return false;

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
    if (!m_showScrollbar || !needHorizontalScrollbar()) return false;

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

    // 根据滚动方向处理滚轮事件
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

    // 传递给父类处理（用于子控件事件）
    LiteContainer::onMousePressed(event);
}

void LiteScrollView::onMouseReleased(const MouseEvent& event) {
    m_draggingVScrollbar = false;
    m_draggingHScrollbar = false;
    LiteContainer::onMouseReleased(event);
}

void LiteScrollView::onMouseMoved(const MouseEvent& event) {
    // 处理垂直滚动条拖拽
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

    // 处理水平滚动条拖拽
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

    // 传递给父类处理
    LiteContainer::onMouseMoved(event);
}

} // namespace liteDui
