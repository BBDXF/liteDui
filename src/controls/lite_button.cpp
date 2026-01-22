/**
 * lite_button.cpp - 按钮控件实现
 */

#include "lite_button.h"

namespace liteDui {

LiteButton::LiteButton() {
    setBackgroundColor(m_normalBgColor);
    setBorderColor(m_normalBorderColor);
    setTextColor(m_normalTextColor);
    setBorderRadius(EdgeInsets::All(4.0f));
    setPadding(EdgeInsets::Symmetric(16.0f, 8.0f));
    setTextAlign(TextAlign::Center);
    setBorder(EdgeInsets::All(1.0f));
}

LiteButton::LiteButton(const std::string& text) : LiteButton() {
    setText(text);
}

void LiteButton::setOnClick(MouseEventCallback callback) {
    m_onClick = callback;
}

void LiteButton::setDisabled(bool disabled) {
    setState(disabled ? ControlState::Disabled : ControlState::Normal);
}

void LiteButton::setState(ControlState state) {
    if (m_state == state) return;
    m_state = state;
    updateAppearance();
}

void LiteButton::updateAppearance() {
    switch (m_state) {
    case ControlState::Normal:
        setBackgroundColor(m_normalBgColor);
        setBorderColor(m_normalBorderColor);
        setTextColor(m_normalTextColor);
        break;
    case ControlState::Hover:
        setBackgroundColor(m_hoverBgColor);
        setBorderColor(m_hoverBorderColor);
        setTextColor(m_hoverTextColor);
        break;
    case ControlState::Pressed:
        setBackgroundColor(m_pressedBgColor);
        setBorderColor(m_pressedBorderColor);
        setTextColor(m_pressedTextColor);
        break;
    case ControlState::Disabled:
        setBackgroundColor(m_disabledBgColor);
        setBorderColor(m_disabledBorderColor);
        setTextColor(m_disabledTextColor);
        break;
    default:
        break;
    }
    markDirty();
}

void LiteButton::onMousePressed(const MouseEvent& event) {
    if (m_state == ControlState::Disabled) return;
    if (event.button == MouseButton::Left) {
        setState(ControlState::Pressed);
    }
}

void LiteButton::onMouseReleased(const MouseEvent& event) {
    if (m_state == ControlState::Disabled) return;
    if (event.button == MouseButton::Left && m_state == ControlState::Pressed) {
        setState(ControlState::Hover);
        if (m_onClick) {
            m_onClick(event);
        }
    }
}

void LiteButton::onMouseEntered(const MouseEvent& event) {
    if (m_state == ControlState::Disabled) return;
    setState(ControlState::Hover);
}

void LiteButton::onMouseExited(const MouseEvent& event) {
    if (m_state == ControlState::Disabled) return;
    setState(ControlState::Normal);
}

} // namespace liteDui
