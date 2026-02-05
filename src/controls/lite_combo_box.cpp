/**
 * lite_combo_box.cpp - 下拉组合框控件实现
 */

#include "lite_combo_box.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRRect.h"

namespace liteDui {

// ==================== ComboDropdownOverlay ====================

ComboDropdownOverlay::ComboDropdownOverlay(LiteComboBox* comboBox) 
    : m_comboBox(comboBox) {
    setBackgroundColor(Color::Transparent());
}

void ComboDropdownOverlay::render(SkCanvas* canvas) {
    if (!m_comboBox || m_comboBox->m_items.empty()) return;
    
    float comboX = m_comboBox->getAbsoluteLeft();
    float comboY = m_comboBox->getAbsoluteTop();
    float comboW = m_comboBox->getLayoutWidth();
    float comboH = m_comboBox->getLayoutHeight();
    
    float itemHeight = 28.0f;
    int visibleCount = std::min(static_cast<int>(m_comboBox->m_items.size()), m_comboBox->m_maxVisibleItems);
    float dropdownH = visibleCount * itemHeight;
    float dropdownY = comboY + comboH;
    
    // 绘制下拉列表背景
    SkPaint bgPaint;
    bgPaint.setAntiAlias(true);
    bgPaint.setColor(m_comboBox->m_dropdownColor.toARGB());
    canvas->drawRect(SkRect::MakeXYWH(comboX, dropdownY, comboW, dropdownH), bgPaint);
    
    // 边框
    SkPaint borderPaint;
    borderPaint.setAntiAlias(true);
    borderPaint.setStyle(SkPaint::kStroke_Style);
    borderPaint.setColor(Color::fromRGB(200, 200, 200).toARGB());
    canvas->drawRect(SkRect::MakeXYWH(comboX, dropdownY, comboW, dropdownH), borderPaint);
    
    // 绘制选项
    for (int i = 0; i < visibleCount; i++) {
        float itemY = dropdownY + i * itemHeight;
        
        if (i == m_hoverIndex) {
            SkPaint hoverPaint;
            hoverPaint.setColor(m_comboBox->m_hoverColor.toARGB());
            canvas->drawRect(SkRect::MakeXYWH(comboX, itemY, comboW, itemHeight), hoverPaint);
        }
        
        setText(m_comboBox->m_items[i].text);
        setTextColor(Color::Black());
        drawText(canvas, comboX + 8, itemY, comboW - 16, itemHeight);
    }
}

void ComboDropdownOverlay::onMousePressed(const MouseEvent& event) {
    if (!m_comboBox) return;
    
    float comboX = m_comboBox->getAbsoluteLeft();
    float comboY = m_comboBox->getAbsoluteTop();
    float comboW = m_comboBox->getLayoutWidth();
    float comboH = m_comboBox->getLayoutHeight();
    
    float itemHeight = 28.0f;
    int visibleCount = std::min(static_cast<int>(m_comboBox->m_items.size()), m_comboBox->m_maxVisibleItems);
    float dropdownY = comboY + comboH;
    float dropdownH = visibleCount * itemHeight;
    
    // 检查是否点击在下拉列表区域
    if (event.x >= comboX && event.x < comboX + comboW &&
        event.y >= dropdownY && event.y < dropdownY + dropdownH) {
        int clickedIndex = static_cast<int>((event.y - dropdownY) / itemHeight);
        if (clickedIndex >= 0 && clickedIndex < static_cast<int>(m_comboBox->m_items.size())) {
            m_comboBox->setSelectedIndex(clickedIndex);
        }
    }
    
    // 关闭下拉列表
    m_comboBox->hideDropdown();
}

void ComboDropdownOverlay::onMouseMoved(const MouseEvent& event) {
    if (!m_comboBox) return;
    
    float comboX = m_comboBox->getAbsoluteLeft();
    float comboY = m_comboBox->getAbsoluteTop();
    float comboW = m_comboBox->getLayoutWidth();
    float comboH = m_comboBox->getLayoutHeight();
    
    float itemHeight = 28.0f;
    float dropdownY = comboY + comboH;
    
    int newHover = -1;
    if (event.x >= comboX && event.x < comboX + comboW && event.y >= dropdownY) {
        newHover = static_cast<int>((event.y - dropdownY) / itemHeight);
        if (newHover >= static_cast<int>(m_comboBox->m_items.size())) {
            newHover = -1;
        }
    }
    
    if (newHover != m_hoverIndex) {
        m_hoverIndex = newHover;
        markDirty();
    }
}

// ==================== LiteComboBox ====================

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
}

void LiteComboBox::onMousePressed(const MouseEvent& event) {
    if (m_isPopupShown) {
        hideDropdown();
    } else {
        showDropdown();
    }
}

void LiteComboBox::showDropdown() {
    auto window = getWindow();
    if (!window) return;
    
    if (!m_dropdownOverlay) {
        m_dropdownOverlay = std::make_shared<ComboDropdownOverlay>(this);
    }
    
    window->pushOverlay(m_dropdownOverlay);
    m_isPopupShown = true;
    markDirty();
}

void LiteComboBox::hideDropdown() {
    auto window = getWindow();
    if (window && m_dropdownOverlay) {
        window->removeOverlay(m_dropdownOverlay);
    }
    m_isPopupShown = false;
    markDirty();
}

} // namespace liteDui
