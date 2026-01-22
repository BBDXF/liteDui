/**
 * lite_button.h - 按钮控件
 */

#pragma once

#include "lite_container.h"

namespace liteDui {

class LiteButton;
using LiteButtonPtr = std::shared_ptr<LiteButton>;

/**
 * LiteButton - 按钮控件
 */
class LiteButton : public LiteContainer {
public:
    LiteButton();
    explicit LiteButton(const std::string& text);
    ~LiteButton() override = default;

    // 设置点击回调
    void setOnClick(MouseEventCallback callback);

    // 禁用状态
    void setDisabled(bool disabled);
    bool isDisabled() const { return m_state == ControlState::Disabled; }

    // 获取状态
    ControlState getState() const { return m_state; }

    // 状态颜色设置
    void setNormalBackgroundColor(const Color& color) { m_normalBgColor = color; updateAppearance(); }
    void setHoverBackgroundColor(const Color& color) { m_hoverBgColor = color; updateAppearance(); }
    void setPressedBackgroundColor(const Color& color) { m_pressedBgColor = color; updateAppearance(); }
    void setDisabledBackgroundColor(const Color& color) { m_disabledBgColor = color; updateAppearance(); }

    void setNormalTextColor(const Color& color) { m_normalTextColor = color; updateAppearance(); }
    void setHoverTextColor(const Color& color) { m_hoverTextColor = color; updateAppearance(); }
    void setPressedTextColor(const Color& color) { m_pressedTextColor = color; updateAppearance(); }
    void setDisabledTextColor(const Color& color) { m_disabledTextColor = color; updateAppearance(); }

    // 事件处理（公开以便外部调用）
    void onMousePressed(const MouseEvent& event) override;
    void onMouseReleased(const MouseEvent& event) override;
    void onMouseEntered(const MouseEvent& event) override;
    void onMouseExited(const MouseEvent& event) override;

private:
    void setState(ControlState state);
    void updateAppearance();

    ControlState m_state = ControlState::Normal;
    MouseEventCallback m_onClick;

    // 状态颜色
    Color m_normalBgColor = Color::LightGray();
    Color m_hoverBgColor = Color::Gray();
    Color m_pressedBgColor = Color::DarkGray();
    Color m_disabledBgColor = Color(0.8f, 0.8f, 0.8f, 1.0f);

    Color m_normalTextColor = Color::Black();
    Color m_hoverTextColor = Color::Black();
    Color m_pressedTextColor = Color::White();
    Color m_disabledTextColor = Color(0.5f, 0.5f, 0.5f, 1.0f);

    Color m_normalBorderColor = Color::Gray();
    Color m_hoverBorderColor = Color::DarkGray();
    Color m_pressedBorderColor = Color::Black();
    Color m_disabledBorderColor = Color(0.6f, 0.6f, 0.6f, 1.0f);
};

} // namespace liteDui
