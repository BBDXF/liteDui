/**
 * lite_input.cpp - 输入框控件实现
 * 参考 Skia plaintexteditor 实现精确文本测量和选择
 */

#include "lite_input.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkFont.h"
#include "include/core/SkFontMetrics.h"
#include <algorithm>
#include <cctype>
#include <vector>

namespace liteDui {

LiteInput::LiteInput() {
    setBackgroundColor(m_normalBgColor);
    setBorderColor(m_normalBorderColor);
    setBorderRadius(EdgeInsets::All(4.0f));
    setPadding(EdgeInsets::Symmetric(12.0f, 8.0f));
    setBorder(EdgeInsets::All(1.0f));
    setFontSize(14.0f);
    m_lastBlinkTime = std::chrono::steady_clock::now();
}

LiteInput::LiteInput(const std::string& placeholder) : LiteInput() {
    m_placeholder = placeholder;
}

void LiteInput::setInputType(InputType type) {
    m_inputType = type;
    markDirty();
}

void LiteInput::setPlaceholder(const std::string& placeholder) {
    m_placeholder = placeholder;
    markDirty();
}

void LiteInput::setValue(const std::string& value) {
    setText(value);
    m_cursorPos = std::min(m_cursorPos, static_cast<int>(value.length()));
    clearSelection();
    if (m_onTextChanged) m_onTextChanged(value);
}

void LiteInput::setReadOnly(bool readOnly) {
    m_readOnly = readOnly;
}

void LiteInput::setMaxLength(int maxLength) {
    m_maxLength = maxLength;
    if (maxLength > 0 && getText().length() > static_cast<size_t>(maxLength)) {
        setText(getText().substr(0, maxLength));
        m_cursorPos = std::min(m_cursorPos, maxLength);
    }
}

void LiteInput::setDisabled(bool disabled) {
    setState(disabled ? ControlState::Disabled : ControlState::Normal);
}

void LiteInput::setCursorPosition(int pos) {
    int len = static_cast<int>(getText().length());
    m_cursorPos = std::max(0, std::min(pos, len));
    clearSelection();
    markDirty();
}

void LiteInput::selectAll() {
    m_selectionStart = 0;
    m_selectionEnd = static_cast<int>(getText().length());
    markDirty();
}

void LiteInput::clearSelection() {
    m_selectionStart = -1;
    m_selectionEnd = -1;
    markDirty();
}

bool LiteInput::hasSelection() const {
    return m_selectionStart >= 0 && m_selectionEnd >= 0 && m_selectionStart != m_selectionEnd;
}

std::string LiteInput::getSelectedText() const {
    if (!hasSelection()) return "";
    int start = std::min(m_selectionStart, m_selectionEnd);
    int end = std::max(m_selectionStart, m_selectionEnd);
    return getText().substr(start, end - start);
}

void LiteInput::setOnTextChanged(TextChangedCallback callback) {
    m_onTextChanged = callback;
}

void LiteInput::setOnFocusChanged(FocusChangedCallback callback) {
    m_onFocusChanged = callback;
}

void LiteInput::clear() {
    setText("");
    m_cursorPos = 0;
    m_scrollOffset = 0;
    clearSelection();
    if (m_onTextChanged) m_onTextChanged("");
}

void LiteInput::insertText(const std::string& text) {
    if (text.empty() || m_readOnly) return;
    
    deleteSelected();
    
    std::string current = getText();
    if (m_maxLength > 0) {
        int available = m_maxLength - static_cast<int>(current.length());
        if (available <= 0) return;
        std::string toInsert = text.substr(0, available);
        std::string newText = current.substr(0, m_cursorPos) + toInsert + current.substr(m_cursorPos);
        setText(newText);
        m_cursorPos += static_cast<int>(toInsert.length());
    } else {
        std::string newText = current.substr(0, m_cursorPos) + text + current.substr(m_cursorPos);
        setText(newText);
        m_cursorPos += static_cast<int>(text.length());
    }
    
    if (m_onTextChanged) m_onTextChanged(getText());
}

void LiteInput::setState(ControlState state) {
    if (m_state == state) return;
    
    bool wasFocused = (m_state == ControlState::Focused);
    bool isFocused = (state == ControlState::Focused);
    
    m_state = state;
    updateAppearance();
    
    if (wasFocused != isFocused && m_onFocusChanged) {
        m_onFocusChanged(isFocused);
    }
}

void LiteInput::updateAppearance() {
    switch (m_state) {
    case ControlState::Normal:
    case ControlState::Hover:
        setBackgroundColor(m_normalBgColor);
        setBorderColor(m_normalBorderColor);
        break;
    case ControlState::Focused:
        setBackgroundColor(m_focusedBgColor);
        setBorderColor(m_focusedBorderColor);
        break;
    case ControlState::Disabled:
        setBackgroundColor(m_disabledBgColor);
        setBorderColor(m_disabledBorderColor);
        break;
    default:
        break;
    }
    markDirty();
}

std::string LiteInput::getDisplayText() const {
    if (m_inputType == InputType::Password) {
        return std::string(getText().length(), '*');
    }
    return getText();
}

// 使用 SkFont 测量文本宽度
float LiteInput::measureTextWidth(const SkFont& font, const std::string& text) const {
    if (text.empty()) return 0;
    return font.measureText(text.c_str(), text.length(), SkTextEncoding::kUTF8);
}

// 计算每个字符的 X 位置（累积宽度）
std::vector<float> LiteInput::getCharPositions(const SkFont& font, const std::string& text) const {
    std::vector<float> positions;
    positions.reserve(text.length() + 1);
    positions.push_back(0);
    
    for (size_t i = 0; i < text.length(); ++i) {
        float width = font.measureText(text.c_str(), i + 1, SkTextEncoding::kUTF8);
        positions.push_back(width);
    }
    return positions;
}

// 根据 X 坐标找到最近的字符位置
int LiteInput::xToCharIndex(const SkFont& font, const std::string& text, float x) const {
    if (text.empty()) return 0;
    
    auto positions = getCharPositions(font, text);
    
    // 找到最接近 x 的位置
    for (size_t i = 0; i < positions.size() - 1; ++i) {
        float mid = (positions[i] + positions[i + 1]) / 2;
        if (x < mid) return static_cast<int>(i);
    }
    return static_cast<int>(text.length());
}

// 确保光标可见，调整滚动偏移
void LiteInput::ensureCursorVisible(const SkFont& font, float visibleWidth) {
    std::string displayText = getDisplayText();
    auto positions = getCharPositions(font, displayText);
    
    float cursorX = (m_cursorPos < static_cast<int>(positions.size())) 
                    ? positions[m_cursorPos] : positions.back();
    
    // 光标在可见区域左边
    if (cursorX - m_scrollOffset < 0) {
        m_scrollOffset = cursorX;
    }
    // 光标在可见区域右边
    else if (cursorX - m_scrollOffset > visibleWidth) {
        m_scrollOffset = cursorX - visibleWidth;
    }
    
    // 确保滚动偏移不为负
    m_scrollOffset = std::max(0.0f, m_scrollOffset);
}

void LiteInput::handleCharInput(unsigned int codepoint) {
    if (m_state != ControlState::Focused || m_readOnly) return;
    if (codepoint < 32 || codepoint == 127) return;
    
    char c = static_cast<char>(codepoint);
    if (m_inputType == InputType::Number) {
        if (!std::isdigit(c) && c != '-' && c != '.') return;
    }
    
    insertText(std::string(1, c));
}

void LiteInput::handleSpecialKey(const KeyEvent& event) {
    bool shift = event.mods & 1; // Shift 键
    
    switch (event.keyCode) {
    case 259: // Backspace
        if (hasSelection()) {
            deleteSelected();
        } else if (m_cursorPos > 0) {
            std::string current = getText();
            setText(current.substr(0, m_cursorPos - 1) + current.substr(m_cursorPos));
            m_cursorPos--;
            if (m_onTextChanged) m_onTextChanged(getText());
        }
        break;
    case 261: // Delete
        if (hasSelection()) {
            deleteSelected();
        } else if (m_cursorPos < static_cast<int>(getText().length())) {
            std::string current = getText();
            setText(current.substr(0, m_cursorPos) + current.substr(m_cursorPos + 1));
            if (m_onTextChanged) m_onTextChanged(getText());
        }
        break;
    case 263: // Left
        if (m_cursorPos > 0) {
            if (shift) {
                if (!hasSelection()) m_selectionStart = m_cursorPos;
                m_cursorPos--;
                m_selectionEnd = m_cursorPos;
            } else {
                m_cursorPos--;
                clearSelection();
            }
        }
        break;
    case 262: // Right
        if (m_cursorPos < static_cast<int>(getText().length())) {
            if (shift) {
                if (!hasSelection()) m_selectionStart = m_cursorPos;
                m_cursorPos++;
                m_selectionEnd = m_cursorPos;
            } else {
                m_cursorPos++;
                clearSelection();
            }
        }
        break;
    case 268: // Home
        if (shift) {
            if (!hasSelection()) m_selectionStart = m_cursorPos;
            m_selectionEnd = 0;
        } else {
            clearSelection();
        }
        m_cursorPos = 0;
        break;
    case 269: // End
        if (shift) {
            if (!hasSelection()) m_selectionStart = m_cursorPos;
            m_selectionEnd = static_cast<int>(getText().length());
        } else {
            clearSelection();
        }
        m_cursorPos = static_cast<int>(getText().length());
        break;
    }
    markDirty();
}

void LiteInput::deleteSelected() {
    if (!hasSelection()) return;
    
    int start = std::min(m_selectionStart, m_selectionEnd);
    int end = std::max(m_selectionStart, m_selectionEnd);
    std::string current = getText();
    setText(current.substr(0, start) + current.substr(end));
    m_cursorPos = start;
    clearSelection();
    if (m_onTextChanged) m_onTextChanged(getText());
}

void LiteInput::updateCursorBlink() {
    if (m_state != ControlState::Focused) {
        m_cursorVisible = true;
        return;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastBlinkTime);
    if (duration.count() > 500) {
        m_cursorVisible = !m_cursorVisible;
        m_lastBlinkTime = now;
        markDirty();
    }
}

void LiteInput::render(SkCanvas* canvas) {
    LiteContainer::render(canvas);
    updateCursorBlink();
    
    float padL = getLayoutPaddingLeft();
    float padT = getLayoutPaddingTop();
    float padR = getLayoutPaddingRight();
    float borderL = getLayoutBorderLeft();
    float borderT = getLayoutBorderTop();
    float borderR = getLayoutBorderRight();
    
    float textX = borderL + padL;
    float textY = borderT + padT;
    float visibleWidth = getLayoutWidth() - borderL - borderR - padL - padR;
    
    // 创建字体
    SkFont font;
    font.setSize(getFontSize());
    
    std::string displayText = getDisplayText();
    bool showPlaceholder = displayText.empty() && !m_placeholder.empty();
    
    // 确保光标可见
    if (m_state == ControlState::Focused) {
        ensureCursorVisible(font, visibleWidth);
    }
    
    // 设置裁剪区域
    canvas->save();
    SkRect clipRect = SkRect::MakeXYWH(textX, textY, visibleWidth, getFontSize() + 4);
    canvas->clipRect(clipRect);
    
    // 计算字符位置
    auto positions = getCharPositions(font, displayText);
    
    // 绘制选择高亮
    if (hasSelection() && !displayText.empty()) {
        int selStart = std::min(m_selectionStart, m_selectionEnd);
        int selEnd = std::max(m_selectionStart, m_selectionEnd);
        
        float startX = positions[selStart] - m_scrollOffset;
        float endX = positions[selEnd] - m_scrollOffset;
        
        SkPaint selPaint;
        selPaint.setColor(m_selectionColor.toARGB());
        canvas->drawRect(SkRect::MakeXYWH(textX + startX, textY, endX - startX, getFontSize()), selPaint);
    }
    
    // 绘制文本
    SkPaint textPaint;
    SkFontMetrics metrics;
    font.getMetrics(&metrics);
    float baseline = textY - metrics.fAscent;
    
    if (showPlaceholder) {
        textPaint.setColor(m_placeholderColor.toARGB());
        canvas->drawString(m_placeholder.c_str(), textX, baseline, font, textPaint);
    } else if (!displayText.empty()) {
        textPaint.setColor(getTextColor().toARGB());
        canvas->drawString(displayText.c_str(), textX - m_scrollOffset, baseline, font, textPaint);
    }
    
    // 绘制光标
    if (m_state == ControlState::Focused && m_cursorVisible) {
        float cursorX = textX + positions[m_cursorPos] - m_scrollOffset;
        
        SkPaint cursorPaint;
        cursorPaint.setColor(m_cursorColor.toARGB());
        cursorPaint.setStrokeWidth(1.0f);
        canvas->drawLine(cursorX, textY, cursorX, textY + getFontSize(), cursorPaint);
    }
    
    canvas->restore();
}

void LiteInput::onMousePressed(const MouseEvent& event) {
    if (m_state == ControlState::Disabled) return;
    setState(ControlState::Focused);
    
    float padL = getLayoutPaddingLeft();
    float borderL = getLayoutBorderLeft();
    float x = event.x - borderL - padL + m_scrollOffset;
    
    // 创建字体进行精确定位
    SkFont font;
    font.setSize(getFontSize());
    
    std::string displayText = getDisplayText();
    m_cursorPos = xToCharIndex(font, displayText, x);
    
    // 开始选择
    m_selectionStart = m_cursorPos;
    m_selectionEnd = m_cursorPos;
    m_isDragging = true;
    
    markDirty();
}

void LiteInput::onMouseReleased(const MouseEvent& event) {
    m_isDragging = false;
    if (m_selectionStart == m_selectionEnd) {
        clearSelection();
    }
}

void LiteInput::onKeyPressed(const KeyEvent& event) {
    if (m_state == ControlState::Disabled || m_readOnly) return;
    
    if (event.codepoint > 0) {
        handleCharInput(event.codepoint);
    } else {
        handleSpecialKey(event);
    }
}

} // namespace liteDui
