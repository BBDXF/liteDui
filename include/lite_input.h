/**
 * lite_input.h - 输入框控件
 */

#pragma once

#include "lite_container.h"
#include <chrono>
#include <vector>

class SkFont;

namespace liteDui {

class LiteInput;
using LiteInputPtr = std::shared_ptr<LiteInput>;

// 输入类型
enum class InputType {
    Text,
    Password,
    Number
};

/**
 * LiteInput - 输入框控件
 */
class LiteInput : public LiteContainer {
public:
    LiteInput();
    explicit LiteInput(const std::string& placeholder);
    ~LiteInput() override = default;

    // 基本属性
    void setInputType(InputType type);
    void setPlaceholder(const std::string& placeholder);
    void setValue(const std::string& value);
    void setReadOnly(bool readOnly);
    void setMaxLength(int maxLength);
    void setDisabled(bool disabled);

    InputType getInputType() const { return m_inputType; }
    const std::string& getPlaceholder() const { return m_placeholder; }
    const std::string& getValue() const { return getText(); }
    bool isReadOnly() const { return m_readOnly; }
    bool isDisabled() const { return m_state == ControlState::Disabled; }
    ControlState getState() const { return m_state; }

    // 光标和选择
    void setCursorPosition(int pos);
    int getCursorPosition() const { return m_cursorPos; }
    void selectAll();
    void clearSelection();
    bool hasSelection() const;
    std::string getSelectedText() const;

    // 样式
    void setPlaceholderColor(const Color& color) { m_placeholderColor = color; markDirty(); }
    void setCursorColor(const Color& color) { m_cursorColor = color; markDirty(); }
    void setSelectionColor(const Color& color) { m_selectionColor = color; markDirty(); }

    // 回调
    void setOnTextChanged(TextChangedCallback callback);
    void setOnFocusChanged(FocusChangedCallback callback);

    // 操作
    void clear();
    void insertText(const std::string& text);

    // 重写渲染
    void render(SkCanvas* canvas) override;

    // 事件处理（公开以便外部调用）
    void onMousePressed(const MouseEvent& event) override;
    void onMouseReleased(const MouseEvent& event) override;
    void onKeyPressed(const KeyEvent& event) override;
    void onCharInput(unsigned int codepoint) override;

private:
    void setState(ControlState state);
    void updateAppearance();
    std::string getDisplayText() const;
    void handleCharInput(unsigned int codepoint);
    void handleSpecialKey(const KeyEvent& event);
    void deleteSelected();
    void updateCursorBlink();
    
    // 文本测量辅助方法
    float measureTextWidth(const SkFont& font, const std::string& text) const;
    std::vector<float> getCharPositions(const SkFont& font, const std::string& text) const;
    int xToCharIndex(const SkFont& font, const std::string& text, float x) const;
    void ensureCursorVisible(const SkFont& font, float visibleWidth);

    ControlState m_state = ControlState::Normal;
    InputType m_inputType = InputType::Text;
    std::string m_placeholder;
    bool m_readOnly = false;
    int m_maxLength = -1;

    // 光标
    int m_cursorPos = 0;
    int m_selectionStart = -1;
    int m_selectionEnd = -1;
    bool m_cursorVisible = true;
    bool m_isDragging = false;
    float m_scrollOffset = 0.0f;
    std::chrono::steady_clock::time_point m_lastBlinkTime;

    // 样式
    Color m_placeholderColor = Color(0.6f, 0.6f, 0.6f, 1.0f);
    Color m_cursorColor = Color::Black();
    Color m_selectionColor = Color(0.2f, 0.6f, 1.0f, 0.3f);

    // 状态颜色
    Color m_normalBgColor = Color::White();
    Color m_focusedBgColor = Color::White();
    Color m_disabledBgColor = Color(0.95f, 0.95f, 0.95f, 1.0f);
    Color m_normalBorderColor = Color::LightGray();
    Color m_focusedBorderColor = Color(0.2f, 0.6f, 1.0f, 1.0f);
    Color m_disabledBorderColor = Color(0.8f, 0.8f, 0.8f, 1.0f);

    // 回调
    TextChangedCallback m_onTextChanged;
    FocusChangedCallback m_onFocusChanged;
};

} // namespace liteDui
