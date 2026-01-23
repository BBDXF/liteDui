/**
 * lite_tab_view.cpp - 标签页控件实现
 */

#include "lite_tab_view.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkRRect.h"
#include "modules/skparagraph/include/ParagraphBuilder.h"
#include "modules/skparagraph/include/Paragraph.h"
#include <algorithm>

using namespace skia::textlayout;

namespace liteDui {

LiteTabView::LiteTabView() {
    setBackgroundColor(Color::White());
    setFlexDirection(FlexDirection::Column);
    // 为标签栏预留顶部空间
    setPadding(EdgeInsets(0, m_tabHeight, 0, 0));
}

void LiteTabView::addTab(const std::string& title, LiteContainerPtr content) {
    m_tabs.emplace_back(title, content);
    
    // 将内容添加为子控件（但初始时隐藏）
    if (content) {
        content->setDisplay(Display::None);
        addChild(content);
    }
    
    if (m_tabs.size() == 1) {
        m_selectedIndex = 0;
        // 显示第一个标签的内容
        if (content) {
            content->setDisplay(Display::Flex);
        }
    }
    markDirty();
}

void LiteTabView::insertTab(size_t index, const std::string& title, LiteContainerPtr content) {
    if (index > m_tabs.size()) {
        index = m_tabs.size();
    }
    m_tabs.insert(m_tabs.begin() + index, TabItem(title, content));
    
    // 调整选中索引
    if (static_cast<int>(index) <= m_selectedIndex) {
        m_selectedIndex++;
    }
    markDirty();
}

void LiteTabView::removeTab(size_t index) {
    if (index >= m_tabs.size()) return;
    
    m_tabs.erase(m_tabs.begin() + index);
    
    // 调整选中索引
    if (m_tabs.empty()) {
        m_selectedIndex = -1;
    } else if (static_cast<int>(index) <= m_selectedIndex) {
        m_selectedIndex = std::max(0, m_selectedIndex - 1);
    }
    markDirty();
}

void LiteTabView::clearTabs() {
    m_tabs.clear();
    m_selectedIndex = -1;
    m_hoverIndex = -1;
    markDirty();
}

TabItem* LiteTabView::getTab(size_t index) {
    if (index >= m_tabs.size()) return nullptr;
    return &m_tabs[index];
}

const TabItem* LiteTabView::getTab(size_t index) const {
    if (index >= m_tabs.size()) return nullptr;
    return &m_tabs[index];
}

void LiteTabView::setTabContent(size_t index, LiteContainerPtr content) {
    if (index >= m_tabs.size()) return;
    m_tabs[index].content = content;
    markDirty();
}

LiteContainerPtr LiteTabView::getTabContent(size_t index) const {
    if (index >= m_tabs.size()) return nullptr;
    return m_tabs[index].content;
}

void LiteTabView::setSelectedIndex(int index) {
    if (index < 0 || index >= static_cast<int>(m_tabs.size())) return;
    if (m_selectedIndex == index) return;
    if (!m_tabs[index].enabled) return;
    
    // 隐藏之前选中的内容
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_tabs.size())) {
        auto oldContent = m_tabs[m_selectedIndex].content;
        if (oldContent) {
            oldContent->setDisplay(Display::None);
        }
    }
    
    m_selectedIndex = index;
    
    // 显示新选中的内容
    auto newContent = m_tabs[m_selectedIndex].content;
    if (newContent) {
        newContent->setDisplay(Display::Flex);
    }
    
    markDirty();
    
    if (m_onTabChanged) {
        m_onTabChanged(index);
    }
}

void LiteTabView::setTabHeight(float height) {
    if (m_tabHeight == height) return;
    m_tabHeight = height;
    // 更新 padding 以适应新的标签高度
    if (m_tabPosition == TabPosition::Top) {
        setPadding(EdgeInsets(0, m_tabHeight, 0, 0));
    } else {
        setPadding(EdgeInsets(0, 0, 0, m_tabHeight));
    }
    markDirty();
}

void LiteTabView::setTabPosition(TabPosition position) {
    if (m_tabPosition == position) return;
    m_tabPosition = position;
    // 更新 padding 以适应新的标签位置
    if (m_tabPosition == TabPosition::Top) {
        setPadding(EdgeInsets(0, m_tabHeight, 0, 0));
    } else {
        setPadding(EdgeInsets(0, 0, 0, m_tabHeight));
    }
    markDirty();
}

void LiteTabView::setOnTabChanged(SelectionChangedCallback callback) {
    m_onTabChanged = callback;
}

float LiteTabView::measureTabWidth(const std::string& title) const {
    if (title.empty()) return m_tabPadding * 2;

    auto& fontMgr = getFontManager();
    auto fontCollection = fontMgr.getFontCollection();

    ParagraphStyle paraStyle;
    auto textStyle = fontMgr.createTextStyle(m_normalTextColor, getFontSize(), getFontFamily());

    auto builder = ParagraphBuilder::make(paraStyle, fontCollection);
    builder->pushStyle(textStyle);
    builder->addText(title.c_str());

    auto paragraph = builder->Build();
    paragraph->layout(10000);

    return paragraph->getMaxIntrinsicWidth() + m_tabPadding * 2;
}

int LiteTabView::getTabIndexAtX(float x) const {
    if (m_tabs.empty()) return -1;

    float currentX = 0;
    for (size_t i = 0; i < m_tabs.size(); ++i) {
        float tabWidth = measureTabWidth(m_tabs[i].title);
        if (x >= currentX && x < currentX + tabWidth) {
            return static_cast<int>(i);
        }
        currentX += tabWidth + m_tabGap;
    }
    return -1;
}

float LiteTabView::getContentTop() const {
    if (m_tabPosition == TabPosition::Top) {
        return m_tabHeight;
    }
    return 0;
}

float LiteTabView::getContentHeight() const {
    return getLayoutHeight() - m_tabHeight;
}

void LiteTabView::render(SkCanvas* canvas) {
    if (!canvas) return;

    float w = getLayoutWidth();
    float h = getLayoutHeight();
    if (w <= 0 || h <= 0) return;

    // 绘制背景
    drawBackground(canvas, 0, 0, w, h);

    // 绘制内容区背景和边框
    float contentTop = getContentTop();
    float contentHeight = getContentHeight();

    SkPaint bgPaint;
    bgPaint.setColor(m_selectedTabColor.toARGB());
    bgPaint.setStyle(SkPaint::kFill_Style);
    canvas->drawRect(SkRect::MakeXYWH(0, contentTop, w, contentHeight), bgPaint);

    SkPaint borderPaint;
    borderPaint.setColor(m_contentBorderColor.toARGB());
    borderPaint.setStyle(SkPaint::kStroke_Style);
    borderPaint.setStrokeWidth(1.0f);
    canvas->drawRect(SkRect::MakeXYWH(0.5f, contentTop + 0.5f, w - 1, contentHeight - 1), borderPaint);

    // 绘制标签栏
    drawTabs(canvas);
    
    // 注意：内容区的子控件由 renderTree 自动渲染
}

void LiteTabView::drawTabs(SkCanvas* canvas) {
    float w = getLayoutWidth();
    float tabY = (m_tabPosition == TabPosition::Top) ? 0 : getLayoutHeight() - m_tabHeight;

    // 绘制标签栏背景
    SkPaint bgPaint;
    bgPaint.setColor(m_normalTabColor.toARGB());
    bgPaint.setStyle(SkPaint::kFill_Style);
    canvas->drawRect(SkRect::MakeXYWH(0, tabY, w, m_tabHeight), bgPaint);

    // 绘制标签栏底部边框
    SkPaint borderPaint;
    borderPaint.setColor(m_tabBorderColor.toARGB());
    borderPaint.setStyle(SkPaint::kStroke_Style);
    borderPaint.setStrokeWidth(1.0f);
    
    float borderY = (m_tabPosition == TabPosition::Top) ? tabY + m_tabHeight : tabY;
    canvas->drawLine(0, borderY, w, borderY, borderPaint);

    // 绘制各个标签
    float currentX = 0;
    for (size_t i = 0; i < m_tabs.size(); ++i) {
        float tabWidth = measureTabWidth(m_tabs[i].title);
        drawTab(canvas, i, currentX, tabY, tabWidth, m_tabHeight);
        currentX += tabWidth + m_tabGap;
    }
}

void LiteTabView::drawTab(SkCanvas* canvas, size_t index, float x, float y, float width, float height) {
    if (index >= m_tabs.size()) return;

    const TabItem& tab = m_tabs[index];
    bool isSelected = (static_cast<int>(index) == m_selectedIndex);
    bool isHover = (static_cast<int>(index) == m_hoverIndex);
    bool isEnabled = tab.enabled;

    SkPaint paint;
    paint.setAntiAlias(true);

    // 确定背景颜色
    Color bgColor;
    if (!isEnabled) {
        bgColor = m_normalTabColor;
    } else if (isSelected) {
        bgColor = m_selectedTabColor;
    } else if (isHover) {
        bgColor = m_hoverTabColor;
    } else {
        bgColor = m_normalTabColor;
    }

    // 绘制标签背景
    paint.setColor(bgColor.toARGB());
    paint.setStyle(SkPaint::kFill_Style);

    if (m_tabPosition == TabPosition::Top) {
        // 顶部标签：上方圆角
        SkRect rect = SkRect::MakeXYWH(x, y, width, height);
        float radii[8] = {m_tabRadius, m_tabRadius, m_tabRadius, m_tabRadius, 0, 0, 0, 0};
        SkRRect rrect;
        rrect.setRectRadii(rect, reinterpret_cast<const SkVector*>(radii));
        canvas->drawRRect(rrect, paint);
    } else {
        // 底部标签：下方圆角
        SkRect rect = SkRect::MakeXYWH(x, y, width, height);
        float radii[8] = {0, 0, 0, 0, m_tabRadius, m_tabRadius, m_tabRadius, m_tabRadius};
        SkRRect rrect;
        rrect.setRectRadii(rect, reinterpret_cast<const SkVector*>(radii));
        canvas->drawRRect(rrect, paint);
    }

    // 绘制选中标签的底部指示线
    if (isSelected) {
        paint.setColor(m_selectedTextColor.toARGB());
        paint.setStyle(SkPaint::kFill_Style);
        float indicatorHeight = 3.0f;
        float indicatorY = (m_tabPosition == TabPosition::Top) ? y + height - indicatorHeight : y;
        canvas->drawRect(SkRect::MakeXYWH(x, indicatorY, width, indicatorHeight), paint);
    }

    // 绘制标签文本
    if (!tab.title.empty()) {
        auto& fontMgr = getFontManager();
        auto fontCollection = fontMgr.getFontCollection();

        Color textColor = isEnabled ? (isSelected ? m_selectedTextColor : m_normalTextColor) 
                                    : Color::fromRGB(180, 180, 180);

        ParagraphStyle paraStyle;
        paraStyle.setTextAlign(skia::textlayout::TextAlign::kCenter);

        auto textStyle = fontMgr.createTextStyle(textColor, getFontSize(), getFontFamily());

        auto builder = ParagraphBuilder::make(paraStyle, fontCollection);
        builder->pushStyle(textStyle);
        builder->addText(tab.title.c_str());

        auto paragraph = builder->Build();
        paragraph->layout(width);

        float textHeight = paragraph->getHeight();
        float textY = y + (height - textHeight) / 2;

        paragraph->paint(canvas, x, textY);
    }
}

void LiteTabView::drawContent(SkCanvas* canvas) {
    float w = getLayoutWidth();
    float contentTop = getContentTop();
    float contentHeight = getContentHeight();

    // 绘制内容区背景
    SkPaint bgPaint;
    bgPaint.setColor(m_selectedTabColor.toARGB());
    bgPaint.setStyle(SkPaint::kFill_Style);
    canvas->drawRect(SkRect::MakeXYWH(0, contentTop, w, contentHeight), bgPaint);

    // 绘制内容区边框
    SkPaint borderPaint;
    borderPaint.setColor(m_contentBorderColor.toARGB());
    borderPaint.setStyle(SkPaint::kStroke_Style);
    borderPaint.setStrokeWidth(1.0f);
    canvas->drawRect(SkRect::MakeXYWH(0.5f, contentTop + 0.5f, w - 1, contentHeight - 1), borderPaint);

    // 渲染当前选中标签的内容
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_tabs.size())) {
        auto content = m_tabs[m_selectedIndex].content;
        if (content) {
            canvas->save();
            canvas->translate(0, contentTop);
            
            // 裁剪到内容区域
            canvas->clipRect(SkRect::MakeXYWH(0, 0, w, contentHeight));
            
            // 设置内容尺寸并计算布局
            content->setWidth(LayoutValue::Point(w));
            content->setHeight(LayoutValue::Point(contentHeight));
            content->calculateLayout(w, contentHeight);
            
            // 渲染内容
            content->renderTree(canvas);
            
            canvas->restore();
        }
    }
}

void LiteTabView::onMousePressed(const MouseEvent& event) {
    float tabY = (m_tabPosition == TabPosition::Top) ? 0 : getLayoutHeight() - m_tabHeight;
    
    // 检查是否点击了标签栏
    if (event.y >= tabY && event.y < tabY + m_tabHeight) {
        int index = getTabIndexAtX(event.x);
        if (index >= 0 && index < static_cast<int>(m_tabs.size()) && m_tabs[index].enabled) {
            setSelectedIndex(index);
        }
        return;
    }

    // 传递给内容区处理
    LiteContainer::onMousePressed(event);
}

void LiteTabView::onMouseMoved(const MouseEvent& event) {
    float tabY = (m_tabPosition == TabPosition::Top) ? 0 : getLayoutHeight() - m_tabHeight;
    
    // 检查是否在标签栏内
    if (event.y >= tabY && event.y < tabY + m_tabHeight) {
        int newHoverIndex = getTabIndexAtX(event.x);
        if (newHoverIndex != m_hoverIndex) {
            m_hoverIndex = newHoverIndex;
            markDirty();
        }
        return;
    }

    // 不在标签栏内，清除悬停状态
    if (m_hoverIndex != -1) {
        m_hoverIndex = -1;
        markDirty();
    }

    LiteContainer::onMouseMoved(event);
}

void LiteTabView::onMouseExited(const MouseEvent& event) {
    if (m_hoverIndex != -1) {
        m_hoverIndex = -1;
        markDirty();
    }
    LiteContainer::onMouseExited(event);
}

} // namespace liteDui
