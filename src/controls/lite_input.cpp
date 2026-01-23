/**
 * lite_input.cpp - 输入框控件实现
 * 使用 skparagraph 实现精确文本测量和选择
 */

#include "lite_input.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "modules/skparagraph/include/ParagraphBuilder.h"
#include "modules/skparagraph/include/Paragraph.h"
#include <algorithm>
#include <cctype>

using namespace skia::textlayout;

namespace liteDui {

LiteInput::LiteInput() {
    setBackgroundColor(m_normalBgColor);
    setBorderColor(m_normalBorderColor);
    setBorderRadius(EdgeInsets::All(4.0f));
    setPadding(EdgeInsets::Symmetric(12.0f, 8.0f));
    setBorder(EdgeInsets::All(1.0f));
    setFontSize(14.0f);
    m_lastBlinkTime = std::chrono::steady_clock::now();
    m_cursorVisible = true;
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
    resetCursorBlink();
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
    resetCursorBlink();
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
    resetCursorBlink();
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
    
    resetCursorBlink();
    if (m_onTextChanged) m_onTextChanged(getText());
}

void LiteInput::setState(ControlState state) {
    if (m_state == state) return;
    
    bool wasFocused = (m_state == ControlState::Focused);
    bool isFocused = (state == ControlState::Focused);
    
    m_state = state;
    updateAppearance();
    
    // 获得焦点时重置光标闪烁
    if (isFocused) {
        resetCursorBlink();
    }
    
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

// 使用 skparagraph 构建段落
std::unique_ptr<Paragraph> LiteInput::buildParagraph(
    const std::string& text, const Color& color, float maxWidth) const {
    
    auto& fontMgr = getFontManager();
    auto fontCollection = fontMgr.getFontCollection();
    
    // 创建段落样式（输入框始终左对齐）
    ParagraphStyle paraStyle;
    paraStyle.setTextAlign(skia::textlayout::TextAlign::kLeft);
    
    // 创建文本样式
    auto textStyle = fontMgr.createTextStyle(color, getFontSize(), getFontFamily());
    
    auto builder = ParagraphBuilder::make(paraStyle, fontCollection);
    builder->pushStyle(textStyle);
    builder->addText(text.c_str());
    
    auto paragraph = builder->Build();
    paragraph->layout(maxWidth > 0 ? maxWidth : 10000.0f);
    
    return paragraph;
}

// 计算每个字符位置的 X 坐标（使用 skparagraph 的 getRectsForRange）
std::vector<float> LiteInput::getCharPositions(const std::string& text, float maxWidth) const {
    std::vector<float> positions(text.length() + 1, 0);
    
    if (text.empty()) return positions;
    
    auto paragraph = buildParagraph(text, getTextColor(), maxWidth);
    
    // 遍历每个字符位置
    size_t i = 0;
    while (i < text.length()) {
        // 确定 UTF-8 字符的字节数
        unsigned char c = text[i];
        size_t charLen = 1;
        if ((c & 0x80) == 0) charLen = 1;
        else if ((c & 0xE0) == 0xC0) charLen = 2;
        else if ((c & 0xF0) == 0xE0) charLen = 3;
        else if ((c & 0xF8) == 0xF0) charLen = 4;
        
        size_t nextPos = std::min(i + charLen, text.length());
        
        // 使用 getRectsForRange 获取字符的边界框
        auto rects = paragraph->getRectsForRange(
            0, nextPos, 
            RectHeightStyle::kTight, 
            RectWidthStyle::kTight);
        
        float width = 0;
        if (!rects.empty()) {
            width = rects.back().rect.fRight;
        }
        
        // 填充该字符所有字节位置为相同宽度
        for (size_t j = i + 1; j <= nextPos; ++j) {
            positions[j] = width;
        }
        i = nextPos;
    }
    
    return positions;
}

// 根据 X 坐标找到最近的字符位置
int LiteInput::xToCharIndex(const std::string& text, float x, float maxWidth) const {
    if (text.empty()) return 0;
    
    auto positions = getCharPositions(text, maxWidth);
    
    // 找到最接近 x 的位置
    for (size_t i = 0; i < positions.size() - 1; ++i) {
        float mid = (positions[i] + positions[i + 1]) / 2;
        if (x < mid) return static_cast<int>(i);
    }
    return static_cast<int>(text.length());
}

// 确保光标可见，调整滚动偏移
void LiteInput::ensureCursorVisible(float visibleWidth) {
    std::string displayText = getDisplayText();
    auto positions = getCharPositions(displayText, visibleWidth + m_scrollOffset + 100);
    
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
    
    if (m_inputType == InputType::Number) {
        if (codepoint > 127 || (!std::isdigit(codepoint) && codepoint != '-' && codepoint != '.')) return;
    }
    
    // Unicode codepoint 转 UTF-8
    std::string utf8;
    if (codepoint < 0x80) {
        utf8 = static_cast<char>(codepoint);
    } else if (codepoint < 0x800) {
        utf8 += static_cast<char>(0xC0 | (codepoint >> 6));
        utf8 += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint < 0x10000) {
        utf8 += static_cast<char>(0xE0 | (codepoint >> 12));
        utf8 += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        utf8 += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else {
        utf8 += static_cast<char>(0xF0 | (codepoint >> 18));
        utf8 += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
        utf8 += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        utf8 += static_cast<char>(0x80 | (codepoint & 0x3F));
    }
    
    insertText(utf8);
}

void LiteInput::handleSpecialKey(const KeyEvent& event) {
    bool shift = event.mods & 1; // Shift 键
    if(!event.pressed) return;
    
    switch (event.keyCode) {
    case 259: // Backspace
        if (hasSelection()) {
            deleteSelected();
        } else if (m_cursorPos > 0) {
            std::string current = getText();
            setText(current.substr(0, m_cursorPos - 1) + current.substr(m_cursorPos));
            m_cursorPos--;
            resetCursorBlink();
            if (m_onTextChanged) m_onTextChanged(getText());
        }
        break;
    case 261: // Delete
        if (hasSelection()) {
            deleteSelected();
        } else if (m_cursorPos < static_cast<int>(getText().length())) {
            std::string current = getText();
            setText(current.substr(0, m_cursorPos) + current.substr(m_cursorPos + 1));
            resetCursorBlink();
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
            resetCursorBlink();
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
            resetCursorBlink();
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
        resetCursorBlink();
        break;
    case 269: // End
        if (shift) {
            if (!hasSelection()) m_selectionStart = m_cursorPos;
            m_selectionEnd = static_cast<int>(getText().length());
        } else {
            clearSelection();
        }
        m_cursorPos = static_cast<int>(getText().length());
        resetCursorBlink();
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
    resetCursorBlink();
    if (m_onTextChanged) m_onTextChanged(getText());
}

void LiteInput::updateCursorBlink() {
    if (m_state != ControlState::Focused) {
        if (!m_cursorVisible) {
            m_cursorVisible = true;
            markDirty();
        }
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

void LiteInput::resetCursorBlink() {
    m_cursorVisible = true;
    m_lastBlinkTime = std::chrono::steady_clock::now();
    markDirty();
}

// 更新逻辑 - 处理光标闪烁
void LiteInput::update() {
    updateCursorBlink();
}

void LiteInput::render(SkCanvas* canvas) {
    // 绘制背景和边框
    float w = getLayoutWidth();
    float h = getLayoutHeight();
    if (w <= 0 || h <= 0) return;
    
    drawBackground(canvas, 0, 0, w, h);
    drawBorder(canvas, 0, 0, w, h);
    
    float padL = getLayoutPaddingLeft();
    float padT = getLayoutPaddingTop();
    float padR = getLayoutPaddingRight();
    float borderL = getLayoutBorderLeft();
    float borderT = getLayoutBorderTop();
    float borderR = getLayoutBorderRight();
    
    float textX = borderL + padL;
    float textY = borderT + padT;
    float visibleWidth = w - borderL - borderR - padL - padR;
    
    std::string displayText = getDisplayText();
    bool showPlaceholder = displayText.empty() && !m_placeholder.empty();
    
    // 确保光标可见
    if (m_state == ControlState::Focused) {
        ensureCursorVisible(visibleWidth);
    }
    
    // 设置裁剪区域
    canvas->save();
    
    // 计算字符位置
    auto positions = getCharPositions(displayText, visibleWidth + m_scrollOffset + 100);
    
    // 绘制选择高亮
    if (hasSelection() && !displayText.empty()) {
        int selStart = std::min(m_selectionStart, m_selectionEnd);
        int selEnd = std::max(m_selectionStart, m_selectionEnd);
        
        float startX = positions[selStart] - m_scrollOffset;
        float endX = positions[selEnd] - m_scrollOffset;
        
        // 获取文本高度
        auto paragraph = buildParagraph(displayText, getTextColor(), visibleWidth + m_scrollOffset + 100);
        float textHeight = paragraph->getHeight();
        
        SkPaint selPaint;
        selPaint.setColor(m_selectionColor.toARGB());
        canvas->drawRect(SkRect::MakeXYWH(textX + startX, textY, endX - startX, textHeight), selPaint);
    }
    
    // 绘制文本
    if (showPlaceholder) {
        auto paragraph = buildParagraph(m_placeholder, m_placeholderColor, visibleWidth);
        paragraph->paint(canvas, textX, textY);
    } else if (!displayText.empty()) {
        auto paragraph = buildParagraph(displayText, getTextColor(), visibleWidth + m_scrollOffset + 100);
        paragraph->paint(canvas, textX - m_scrollOffset, textY);
    }
    
    // 绘制光标 - 只在聚焦状态且光标可见时绘制
    if (m_state == ControlState::Focused && m_cursorVisible) {
        float cursorX = textX + positions[m_cursorPos] - m_scrollOffset;
        
        // 获取文本高度用于光标
        float textHeight = getFontSize() * 1.2f; // 默认行高
        if (!displayText.empty()) {
            auto paragraph = buildParagraph(displayText, getTextColor(), visibleWidth + m_scrollOffset + 100);
            textHeight = paragraph->getHeight();
        }
        
        SkPaint cursorPaint;
        cursorPaint.setColor(m_cursorColor.toARGB());
        cursorPaint.setStrokeWidth(1.5f);
        cursorPaint.setStyle(SkPaint::kStroke_Style);
        canvas->drawLine(cursorX, textY, cursorX, textY + textHeight, cursorPaint);
    }
    
    canvas->restore();
}

void LiteInput::onMousePressed(const MouseEvent& event) {
    if (m_state == ControlState::Disabled) return;
    setState(ControlState::Focused);
    
    float padL = getLayoutPaddingLeft();
    float borderL = getLayoutBorderLeft();
    float x = event.x - borderL - padL + m_scrollOffset;
    
    float visibleWidth = getLayoutWidth() - getLayoutBorderLeft() - getLayoutBorderRight() 
                        - getLayoutPaddingLeft() - getLayoutPaddingRight();
    
    std::string displayText = getDisplayText();
    m_cursorPos = xToCharIndex(displayText, x, visibleWidth + m_scrollOffset + 100);
    
    // 开始选择
    m_selectionStart = m_cursorPos;
    m_selectionEnd = m_cursorPos;
    m_isDragging = true;
    
    resetCursorBlink();
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
    handleSpecialKey(event);
}

void LiteInput::onCharInput(unsigned int codepoint) {
    if (m_state != ControlState::Focused || m_readOnly) return;
    handleCharInput(codepoint);
}

} // namespace liteDui
