/**
 * lite_combo_box.cpp - 下拉组合框控件实现
 */

#include "lite_combo_box.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRRect.h"
#include "include/core/SkPath.h"

namespace liteDui {

LiteComboBox::LiteComboBox() {
    setBackgroundColor(Color::White());
    setBorderColor(Color::fromRGB(200, 200, 200));
    setBorderRadius(EdgeInsets::All(4.0f));
    setBorder(EdgeInsets::All(1.0f));
    setPadding(EdgeInsets::Symmetric(8.0f, 6.0f));
    setHeight(32.0f);
}

void LiteComboBox::addItem(const std::string& text, const std::string& data) {
    m_items.push_back({text, data});
    markDirty();
}

void LiteComboBox::insertItem(size_t index, const std::string& text, const std::string& data) {
    if (index <= m_items.size()) {
        m_items.insert(m_items.begin() + index, {text, data});
        markDirty();
    }
}

void LiteComboBox::removeItem(size_t index) {
    if (index < m_items.size()) {
        m_items.erase(m_items.begin() + index);
        if (m_selectedIndex >= static_cast<int>(index)) m_selectedIndex--;
        markDirty();
    }
}

void LiteComboBox::clearItems() {
    m_items.clear();
    m_selectedIndex = -1;
    markDirty();
}

void LiteComboBox::setSelectedIndex(int index) {
    if (index >= -1 && index < static_cast<int>(m_items.size())) {
        m_selectedIndex = index;
        if (m_onSelectionChanged) m_onSelectionChanged(index);
        markDirty();
    }
}

std::string LiteComboBox::getSelectedText() const {
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_items.size())) {
        return m_items[m_selectedIndex].text;
    }
    return "";
}

void LiteComboBox::setMode(ComboBoxMode mode) { m_mode = mode; }
void LiteComboBox::setPlaceholder(const std::string& placeholder) { m_placeholder = placeholder; markDirty(); }
void LiteComboBox::setMaxVisibleItems(int max) { m_maxVisibleItems = max; }
void LiteComboBox::setDropdownColor(const Color& color) { m_dropdownColor = color; }
void LiteComboBox::setHoverColor(const Color& color) { m_hoverColor = color; }
void LiteComboBox::setOnSelectionChanged(std::function<void(int)> callback) { m_onSelectionChanged = callback; }

void LiteComboBox::render(SkCanvas* canvas) {
    float w = getLayoutWidth();
    float h = getLayoutHeight();

    drawBackground(canvas, 0, 0, w, h);
    drawBorder(canvas, 0, 0, w, h);

    // 绘制当前选中文本或占位符
    std::string displayText = m_selectedIndex >= 0 ? m_items[m_selectedIndex].text : m_placeholder;
    if (!displayText.empty()) {
        setText(displayText);
        setTextColor(m_selectedIndex >= 0 ? Color::Black() : Color::Gray());
        drawText(canvas, 8, 0, w - 28, h);
    }

    // 绘制下拉箭头
    SkPaint arrowPaint;
    arrowPaint.setAntiAlias(true);
    arrowPaint.setColor(Color::Gray().toARGB());
    arrowPaint.setStyle(SkPaint::kStroke_Style);
    arrowPaint.setStrokeWidth(2);
    float arrowX = w - 16;
    float arrowY = h / 2;
    canvas->drawLine(arrowX - 4, arrowY - 2, arrowX, arrowY + 2, arrowPaint);
    canvas->drawLine(arrowX, arrowY + 2, arrowX + 4, arrowY - 2, arrowPaint);

    // 绘制下拉列表
    if (m_isPopupShown && !m_items.empty()) {
        float itemHeight = 28.0f;
        int visibleCount = std::min(static_cast<int>(m_items.size()), m_maxVisibleItems);
        float dropdownHeight = visibleCount * itemHeight;

        SkPaint bgPaint;
        bgPaint.setAntiAlias(true);
        bgPaint.setColor(m_dropdownColor.toARGB());
        canvas->drawRect(SkRect::MakeXYWH(0, h, w, dropdownHeight), bgPaint);

        // 边框
        SkPaint borderPaint;
        borderPaint.setAntiAlias(true);
        borderPaint.setStyle(SkPaint::kStroke_Style);
        borderPaint.setColor(Color::fromRGB(200, 200, 200).toARGB());
        canvas->drawRect(SkRect::MakeXYWH(0, h, w, dropdownHeight), borderPaint);

        for (int i = 0; i < visibleCount; i++) {
            float itemY = h + i * itemHeight;
            
            if (i == m_hoverIndex) {
                SkPaint hoverPaint;
                hoverPaint.setColor(m_hoverColor.toARGB());
                canvas->drawRect(SkRect::MakeXYWH(0, itemY, w, itemHeight), hoverPaint);
            }

            setText(m_items[i].text);
            setTextColor(Color::Black());
            drawText(canvas, 8, itemY, w - 16, itemHeight);
        }
    }

    LiteContainer::render(canvas);
}

void LiteComboBox::onMousePressed(const MouseEvent& event) {
    float w = getLayoutWidth();
    float h = getLayoutHeight();

    // 转换为本地坐标
    float localX = event.x - getLeft();
    float localY = event.y - getTop();

    if (m_isPopupShown) {
        float itemHeight = 28.0f;
        int visibleCount = std::min(static_cast<int>(m_items.size()), m_maxVisibleItems);
        
        if (localY > h && localY < h + visibleCount * itemHeight) {
            int clickedIndex = static_cast<int>((localY - h) / itemHeight);
            if (clickedIndex >= 0 && clickedIndex < static_cast<int>(m_items.size())) {
                setSelectedIndex(clickedIndex);
            }
        }
        m_isPopupShown = false;
    } else {
        m_isPopupShown = true;
    }
    markDirty();
}

void LiteComboBox::onMouseMoved(const MouseEvent& event) {
    if (m_isPopupShown) {
        float h = getLayoutHeight();
        float itemHeight = 28.0f;
        
        // 转换为本地坐标
        float localY = event.y - getTop();
        
        if (localY > h) {
            int hoverIdx = static_cast<int>((localY - h) / itemHeight);
            if (hoverIdx != m_hoverIndex && hoverIdx < static_cast<int>(m_items.size())) {
                m_hoverIndex = hoverIdx;
                markDirty();
            }
        } else {
            m_hoverIndex = -1;
        }
    }
}

void LiteComboBox::onFocusLost() {
    if (m_isPopupShown) {
        m_isPopupShown = false;
        markDirty();
    }
}

} // namespace liteDui
