/**
 * lite_input.cpp - 输入框控件实现
 * 使用 skparagraph 实现精确文本测量和选择
 */

#include "lite_input.h"
#include "lite_utf8.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "modules/skparagraph/include/ParagraphBuilder.h"
#include "modules/skparagraph/include/Paragraph.h"
#include <algorithm>
#include <cctype>
#include <cstdio>

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
    if (maxLength > 0 && Utf8Helper::getCharCount(getText()) > maxLength) {
        // 按字符数截断
        auto codePoints = Utf8Helper::toCodePoints(getText());
        if (static_cast<int>(codePoints.size()) > maxLength) {
            int bytePos = Utf8Helper::codePointIndexToByte(codePoints, maxLength, static_cast<int>(getText().length()));
            setText(getText().substr(0, bytePos));
        }
        m_cursorPos = std::min(m_cursorPos, static_cast<int>(getText().length()));
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
    
    // 检查最大长度限制（按字符数）
    if (m_maxLength > 0) {
        int currentCharCount = Utf8Helper::getCharCount(current);
        int insertCharCount = Utf8Helper::getCharCount(text);
        int available = m_maxLength - currentCharCount;
        
        if (available <= 0) return;
        
        std::string toInsert = text;
        if (insertCharCount > available) {
            // 按字符数截断要插入的文本
            auto codePoints = Utf8Helper::toCodePoints(text);
            int bytePos = Utf8Helper::codePointIndexToByte(codePoints, available, static_cast<int>(text.length()));
            toInsert = text.substr(0, bytePos);
        }
        
        std::string newText = current.substr(0, m_cursorPos) + toInsert + current.substr(m_cursorPos);
        setText(newText);
        m_cursorPos += static_cast<int>(toInsert.length());
    } else {
        std::string newText = current.substr(0, m_cursorPos) + text + current.substr(m_cursorPos);
        setText(newText);
        m_cursorPos += static_cast<int>(text.length());
    }
    
    // 确保光标可见
    float visibleWidth = getLayoutWidth() - getLayoutBorderLeft() - getLayoutBorderRight()
                        - getLayoutPaddingLeft() - getLayoutPaddingRight();
    ensureCursorVisible(visibleWidth);
    
    resetCursorBlink();
    if (m_onTextChanged) m_onTextChanged(getText());
}

void LiteInput::setState(ControlState state) {
    if (m_state == state) return;
    
    bool wasFocused = (m_state == ControlState::Focused);
    bool isFocused = (state == ControlState::Focused);
    
    m_state = state;
    updateAppearance();
    
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
        // 密码模式：每个字符显示为 *
        int charCount = Utf8Helper::getCharCount(getText());
        return std::string(charCount, '*');
    }
    return getText();
}

// 使用 skparagraph 构建段落
std::unique_ptr<Paragraph> LiteInput::buildParagraph(
    const std::string& text, const Color& color, float maxWidth) const {
    
    auto& fontMgr = getFontManager();
    auto fontCollection = fontMgr.getFontCollection();
    
    ParagraphStyle paraStyle;
    paraStyle.setTextAlign(skia::textlayout::TextAlign::kLeft);
    
    auto textStyle = fontMgr.createTextStyle(color, getFontSize(), getFontFamily());
    
    auto builder = ParagraphBuilder::make(paraStyle, fontCollection);
    builder->pushStyle(textStyle);
    builder->addText(text.c_str());
    
    auto paragraph = builder->Build();
    paragraph->layout(maxWidth > 0 ? maxWidth : 10000.0f);
    
    return paragraph;
}

// 计算每个字符位置的 X 坐标
std::vector<float> LiteInput::getCharPositions(const std::string& text, float maxWidth) const {
    std::vector<float> positions(text.length() + 1, 0);
    
    if (text.empty()) return positions;
    
    auto paragraph = buildParagraph(text, getTextColor(), maxWidth);
    auto codePoints = Utf8Helper::toCodePoints(text);
    
    // 遍历每个码点，获取其结束位置
    for (size_t i = 0; i < codePoints.size(); ++i) {
        const auto& cp = codePoints[i];
        int endByte = cp.byteOffset + cp.byteLength;
        
        // 将 UTF-8 字节偏移转换为 UTF-16 代码单元索引
        // Skia 的 getRectsForRange API 使用 UTF-16 索引
        int utf16End = Utf8Helper::utf8ByteToUtf16Index(text, endByte);
        
        // 使用 getRectsForRange 获取从开始到当前字符结束的边界框
        auto rects = paragraph->getRectsForRange(
            0, utf16End,
            RectHeightStyle::kTight,
            RectWidthStyle::kTight);
        
        float width = 0;
        if (!rects.empty()) {
            width = rects.back().rect.fRight;
        }
        
        // 填充该字符所有字节位置为相同宽度
        for (int j = cp.byteOffset + 1; j <= endByte && j <= static_cast<int>(text.length()); ++j) {
            positions[j] = width;
        }
    }
    
    return positions;
}

// 根据 X 坐标找到最近的字符边界位置
int LiteInput::xToCharIndex(const std::string& text, float x, float maxWidth) const {
    if (text.empty()) return 0;
    
    auto codePoints = Utf8Helper::toCodePoints(text);
    if (codePoints.empty()) return 0;
    
    auto positions = getCharPositions(text, maxWidth);
    
    // 遍历每个字符边界，找到最接近 x 的位置
    for (size_t i = 0; i < codePoints.size(); ++i) {
        const auto& cp = codePoints[i];
        int startByte = cp.byteOffset;
        int endByte = cp.byteOffset + cp.byteLength;
        
        float startX = positions[startByte];
        float endX = positions[endByte];
        float mid = (startX + endX) / 2;
        
        if (x < mid) {
            return startByte;
        }
    }
    
    return static_cast<int>(text.length());
}

// 确保光标可见，调整滚动偏移
void LiteInput::ensureCursorVisible(float visibleWidth) {
    std::string displayText = getDisplayText();
    auto positions = getCharPositions(displayText, visibleWidth + m_scrollOffset + 100);
    
    float cursorX = (m_cursorPos < static_cast<int>(positions.size())) 
                    ? positions[m_cursorPos] : positions.back();
    
    if (cursorX - m_scrollOffset < 0) {
        m_scrollOffset = cursorX;
    } else if (cursorX - m_scrollOffset > visibleWidth) {
        m_scrollOffset = cursorX - visibleWidth;
    }
    
    m_scrollOffset = std::max(0.0f, m_scrollOffset);
}

void LiteInput::handleCharInput(unsigned int codepoint) {
    if (m_state != ControlState::Focused || m_readOnly) return;
    
    if (m_inputType == InputType::Number) {
        if (codepoint > 127 || (!std::isdigit(codepoint) && codepoint != '-' && codepoint != '.')) {
            return;
        }
    }
    
    insertText(Utf8Helper::codepointToUtf8(codepoint));
}

void LiteInput::handleSpecialKey(const KeyEvent& event) {
    if (!event.pressed) return;
    
    bool shift = event.mods & 1;
    std::string current = getText();
    
    switch (event.keyCode) {
    case 259: // Backspace
        if (hasSelection()) {
            deleteSelected();
        } else if (m_cursorPos > 0) {
            int prevPos = Utf8Helper::getPrevCharPos(current, m_cursorPos);
            setText(current.substr(0, prevPos) + current.substr(m_cursorPos));
            m_cursorPos = prevPos;
            resetCursorBlink();
            if (m_onTextChanged) m_onTextChanged(getText());
        }
        break;
        
    case 261: // Delete
        if (hasSelection()) {
            deleteSelected();
        } else if (m_cursorPos < static_cast<int>(current.length())) {
            int nextPos = Utf8Helper::getNextCharPos(current, m_cursorPos);
            setText(current.substr(0, m_cursorPos) + current.substr(nextPos));
            resetCursorBlink();
            if (m_onTextChanged) m_onTextChanged(getText());
        }
        break;
        
    case 263: // Left
        if (m_cursorPos > 0) {
            int newPos = Utf8Helper::getPrevCharPos(current, m_cursorPos);
            if (shift) {
                if (!hasSelection()) m_selectionStart = m_cursorPos;
                m_cursorPos = newPos;
                m_selectionEnd = m_cursorPos;
            } else {
                m_cursorPos = newPos;
                clearSelection();
            }
            resetCursorBlink();
        }
        break;
        
    case 262: // Right
        if (m_cursorPos < static_cast<int>(current.length())) {
            int newPos = Utf8Helper::getNextCharPos(current, m_cursorPos);
            if (shift) {
                if (!hasSelection()) m_selectionStart = m_cursorPos;
                m_cursorPos = newPos;
                m_selectionEnd = m_cursorPos;
            } else {
                m_cursorPos = newPos;
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
            m_selectionEnd = static_cast<int>(current.length());
        } else {
            clearSelection();
        }
        m_cursorPos = static_cast<int>(current.length());
        resetCursorBlink();
        break;
    }
    
    // 确保光标可见
    float visibleWidth = getLayoutWidth() - getLayoutBorderLeft() - getLayoutBorderRight()
                        - getLayoutPaddingLeft() - getLayoutPaddingRight();
    ensureCursorVisible(visibleWidth);
    
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

void LiteInput::update() {
    updateCursorBlink();
}

void LiteInput::render(SkCanvas* canvas) {
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
    
    if (m_state == ControlState::Focused) {
        ensureCursorVisible(visibleWidth);
    }
    
    canvas->save();
    
    auto positions = getCharPositions(displayText, visibleWidth + m_scrollOffset + 100);
    
    // 绘制选择高亮
    if (hasSelection() && !displayText.empty()) {
        int selStart = std::min(m_selectionStart, m_selectionEnd);
        int selEnd = std::max(m_selectionStart, m_selectionEnd);
        
        if (selStart < static_cast<int>(positions.size()) && selEnd < static_cast<int>(positions.size())) {
            float startX = positions[selStart] - m_scrollOffset;
            float endX = positions[selEnd] - m_scrollOffset;
            
            auto paragraph = buildParagraph(displayText, getTextColor(), visibleWidth + m_scrollOffset + 100);
            float textHeight = paragraph->getHeight();
            
            SkPaint selPaint;
            selPaint.setColor(m_selectionColor.toARGB());
            canvas->drawRect(SkRect::MakeXYWH(textX + startX, textY, endX - startX, textHeight), selPaint);
        }
    }
    
    // 绘制文本
    if (showPlaceholder) {
        auto paragraph = buildParagraph(m_placeholder, m_placeholderColor, visibleWidth);
        paragraph->paint(canvas, textX, textY);
    } else if (!displayText.empty()) {
        auto paragraph = buildParagraph(displayText, getTextColor(), visibleWidth + m_scrollOffset + 100);
        paragraph->paint(canvas, textX - m_scrollOffset, textY);
    }
    
    // 绘制光标
    if (m_state == ControlState::Focused && m_cursorVisible) {
        float cursorX = textX;
        if (m_cursorPos < static_cast<int>(positions.size())) {
            cursorX += positions[m_cursorPos] - m_scrollOffset;
        }

        // printf("Cursor: %d => %.2f.  %.2f\r\n", m_cursorPos, positions[m_cursorPos], cursorX);
        
        float textHeight = getFontSize() * 1.2f;
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
    
    float padL = getLayoutPaddingLeft();
    float borderL = getLayoutBorderLeft();
    float x = event.x - borderL - padL + m_scrollOffset;
    
    float visibleWidth = getLayoutWidth() - getLayoutBorderLeft() - getLayoutBorderRight()
                        - getLayoutPaddingLeft() - getLayoutPaddingRight();
    
    std::string displayText = getDisplayText();
    m_cursorPos = xToCharIndex(displayText, x, visibleWidth + m_scrollOffset + 100);
    
    m_selectionStart = m_cursorPos;
    m_selectionEnd = m_cursorPos;
    m_isDragging = true;
    
    // 确保光标可见
    ensureCursorVisible(visibleWidth);
    
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

void LiteInput::onFocusGained() {
    setState(ControlState::Focused);
    resetCursorBlink();
}

void LiteInput::onFocusLost() {
    setState(ControlState::Normal);
    clearSelection();
}

} // namespace liteDui
