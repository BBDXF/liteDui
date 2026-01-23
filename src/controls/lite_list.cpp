/**
 * lite_list.cpp - 列表控件实现
 */

#include "lite_list.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "modules/skparagraph/include/ParagraphBuilder.h"
#include "modules/skparagraph/include/Paragraph.h"
#include <algorithm>

using namespace skia::textlayout;

namespace liteDui {

LiteList::LiteList() {
    setScrollDirection(ScrollDirection::Vertical);
    setBackgroundColor(Color::White());
    setBorderColor(Color::LightGray());
    setBorder(EdgeInsets::All(1.0f));
    setPadding(EdgeInsets::All(0));
}

void LiteList::addItem(const std::string& text, const std::string& id) {
    m_items.emplace_back(text, id);
    updateContentSize();
    markDirty();
}

void LiteList::insertItem(size_t index, const std::string& text, const std::string& id) {
    if (index > m_items.size()) {
        index = m_items.size();
    }
    m_items.insert(m_items.begin() + index, ListItem(text, id));
    updateContentSize();
    markDirty();
}

void LiteList::removeItem(size_t index) {
    if (index >= m_items.size()) return;
    m_items.erase(m_items.begin() + index);
    updateContentSize();
    markDirty();
}

void LiteList::clearItems() {
    m_items.clear();
    m_hoverIndex = -1;
    updateContentSize();
    markDirty();
}

ListItem* LiteList::getItem(size_t index) {
    if (index >= m_items.size()) return nullptr;
    return &m_items[index];
}

const ListItem* LiteList::getItem(size_t index) const {
    if (index >= m_items.size()) return nullptr;
    return &m_items[index];
}

void LiteList::setSelectionMode(ListSelectionMode mode) {
    if (m_selectionMode == mode) return;
    m_selectionMode = mode;
    
    // 如果切换到单选模式，只保留第一个选中项
    if (mode == ListSelectionMode::Single) {
        bool foundSelected = false;
        for (auto& item : m_items) {
            if (item.selected) {
                if (foundSelected) {
                    item.selected = false;
                } else {
                    foundSelected = true;
                }
            }
        }
    } else if (mode == ListSelectionMode::None) {
        clearSelection();
    }
    
    markDirty();
}

void LiteList::setSelectedIndex(int index) {
    if (m_selectionMode == ListSelectionMode::None) return;
    
    // 单选模式：清除其他选择
    if (m_selectionMode == ListSelectionMode::Single) {
        for (auto& item : m_items) {
            item.selected = false;
        }
    }
    
    if (index >= 0 && index < static_cast<int>(m_items.size())) {
        m_items[index].selected = true;
    }
    
    markDirty();
    
    if (m_onSelectionChanged) {
        m_onSelectionChanged(index);
    }
}

int LiteList::getSelectedIndex() const {
    for (size_t i = 0; i < m_items.size(); ++i) {
        if (m_items[i].selected) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

std::vector<int> LiteList::getSelectedIndices() const {
    std::vector<int> indices;
    for (size_t i = 0; i < m_items.size(); ++i) {
        if (m_items[i].selected) {
            indices.push_back(static_cast<int>(i));
        }
    }
    return indices;
}

void LiteList::clearSelection() {
    for (auto& item : m_items) {
        item.selected = false;
    }
    markDirty();
}

void LiteList::selectAll() {
    if (m_selectionMode != ListSelectionMode::Multiple) return;
    
    for (auto& item : m_items) {
        item.selected = true;
    }
    markDirty();
}

void LiteList::setOnSelectionChanged(SelectionChangedCallback callback) {
    m_onSelectionChanged = callback;
}

void LiteList::setOnItemClicked(std::function<void(int)> callback) {
    m_onItemClicked = callback;
}

void LiteList::setOnItemDoubleClicked(std::function<void(int)> callback) {
    m_onItemDoubleClicked = callback;
}

int LiteList::getItemIndexAtY(float y) const {
    if (m_items.empty()) return -1;
    
    float adjustedY = y + m_scrollY;
    int index = static_cast<int>(adjustedY / m_itemHeight);
    
    if (index < 0 || index >= static_cast<int>(m_items.size())) {
        return -1;
    }
    
    return index;
}

void LiteList::updateContentSize() {
    // 列表内容高度 = 项目数 × 项目高度
    // 这个值会被 LiteScrollView 的 getContentHeight() 使用
    // 但由于我们重写了 renderContent，需要确保滚动范围正确
}

void LiteList::render(SkCanvas* canvas) {
    // 调用父类渲染（包括背景、边框、滚动条）
    LiteScrollView::render(canvas);
}

void LiteList::renderContent(SkCanvas* canvas) {
    if (m_items.empty()) return;

    float viewportH = getViewportHeight();
    float viewportW = getViewportWidth();
    
    // 计算可见范围
    int firstVisible = static_cast<int>(m_scrollY / m_itemHeight);
    int lastVisible = static_cast<int>((m_scrollY + viewportH) / m_itemHeight);
    
    firstVisible = std::max(0, firstVisible);
    lastVisible = std::min(static_cast<int>(m_items.size()) - 1, lastVisible);

    // 只渲染可见的项目
    for (int i = firstVisible; i <= lastVisible; ++i) {
        float itemY = i * m_itemHeight;
        drawItem(canvas, i, itemY, viewportW);
    }
}

void LiteList::drawItem(SkCanvas* canvas, size_t index, float y, float width) {
    if (index >= m_items.size()) return;

    const ListItem& item = m_items[index];
    SkPaint paint;
    paint.setAntiAlias(true);

    // 绘制背景
    bool isSelected = item.selected;
    bool isHover = (static_cast<int>(index) == m_hoverIndex);

    if (isSelected) {
        paint.setColor(m_selectedColor.toARGB());
        paint.setStyle(SkPaint::kFill_Style);
        canvas->drawRect(SkRect::MakeXYWH(0, y, width, m_itemHeight), paint);
    } else if (isHover) {
        paint.setColor(m_hoverColor.toARGB());
        paint.setStyle(SkPaint::kFill_Style);
        canvas->drawRect(SkRect::MakeXYWH(0, y, width, m_itemHeight), paint);
    } else if (m_showAlternateRows && index % 2 == 1) {
        paint.setColor(m_alternateColor.toARGB());
        paint.setStyle(SkPaint::kFill_Style);
        canvas->drawRect(SkRect::MakeXYWH(0, y, width, m_itemHeight), paint);
    }

    // 绘制文本
    if (!item.text.empty()) {
        auto& fontMgr = getFontManager();
        auto fontCollection = fontMgr.getFontCollection();

        ParagraphStyle paraStyle;
        paraStyle.setTextAlign(skia::textlayout::TextAlign::kLeft);
        paraStyle.setMaxLines(1);

        auto textStyle = fontMgr.createTextStyle(getTextColor(), getFontSize(), getFontFamily());

        auto builder = ParagraphBuilder::make(paraStyle, fontCollection);
        builder->pushStyle(textStyle);
        builder->addText(item.text.c_str());

        auto paragraph = builder->Build();
        paragraph->layout(width - m_itemPadding * 2);

        // 垂直居中
        float textHeight = paragraph->getHeight();
        float textY = y + (m_itemHeight - textHeight) / 2;

        paragraph->paint(canvas, m_itemPadding, textY);
    }
}

void LiteList::onMousePressed(const MouseEvent& event) {
    // 先检查是否点击了滚动条
    if (isPointInVerticalScrollbar(event.x, event.y) || 
        isPointInHorizontalScrollbar(event.x, event.y)) {
        LiteScrollView::onMousePressed(event);
        return;
    }

    if (event.button != MouseButton::Left) return;
    if (m_selectionMode == ListSelectionMode::None) return;

    // 计算点击的项目索引
    float borderL = getLayoutBorderLeft();
    float borderT = getLayoutBorderTop();
    float padL = getLayoutPaddingLeft();
    float padT = getLayoutPaddingTop();

    float contentY = event.y - borderT - padT;
    int index = getItemIndexAtY(contentY);

    if (index >= 0 && index < static_cast<int>(m_items.size())) {
        if (m_selectionMode == ListSelectionMode::Single) {
            // 单选模式
            setSelectedIndex(index);
        } else if (m_selectionMode == ListSelectionMode::Multiple) {
            // 多选模式：切换选中状态
            m_items[index].selected = !m_items[index].selected;
            markDirty();
            
            if (m_onSelectionChanged) {
                m_onSelectionChanged(index);
            }
        }

        if (m_onItemClicked) {
            m_onItemClicked(index);
        }
    }
}

void LiteList::onMouseMoved(const MouseEvent& event) {
    // 先处理滚动条拖拽
    LiteScrollView::onMouseMoved(event);

    // 更新悬停项
    float borderT = getLayoutBorderTop();
    float padT = getLayoutPaddingTop();

    float contentY = event.y - borderT - padT;
    int newHoverIndex = getItemIndexAtY(contentY);

    if (newHoverIndex != m_hoverIndex) {
        m_hoverIndex = newHoverIndex;
        markDirty();
    }
}

void LiteList::onMouseExited(const MouseEvent& event) {
    if (m_hoverIndex != -1) {
        m_hoverIndex = -1;
        markDirty();
    }
    LiteScrollView::onMouseExited(event);
}

// 重写 getContentHeight 以返回列表内容的实际高度
float LiteList::getContentHeight() const {
    return m_items.size() * m_itemHeight;
}

} // namespace liteDui
