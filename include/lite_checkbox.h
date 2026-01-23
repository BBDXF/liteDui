/**
 * lite_checkbox.h - 复选框控件
 */

#pragma once

#include "lite_container.h"

namespace liteDui {

class LiteCheckbox;
using LiteCheckboxPtr = std::shared_ptr<LiteCheckbox>;

/**
 * LiteCheckbox - 复选框控件
 * 
 * 支持选中/未选中状态切换，带有标签文本
 */
class LiteCheckbox : public LiteContainer {
public:
    LiteCheckbox();
    explicit LiteCheckbox(const std::string& label);
    ~LiteCheckbox() override = default;

    // 选中状态
    void setChecked(bool checked);
    bool isChecked() const { return m_checked; }
    void toggle();

    // 标签文本
    void setLabel(const std::string& label);
    const std::string& getLabel() const { return m_label; }

    // 禁用状态
    void setDisabled(bool disabled);
    bool isDisabled() const { return m_state == ControlState::Disabled; }

    // 回调
    void setOnChanged(CheckedChangedCallback callback);

    // 样式设置
    void setCheckColor(const Color& color) { m_checkColor = color; markDirty(); }
    Color getCheckColor() const { return m_checkColor; }
    
    void setBoxSize(float size) { m_boxSize = size; markDirty(); }
    float getBoxSize() const { return m_boxSize; }

    void setBoxBorderColor(const Color& color) { m_boxBorderColor = color; markDirty(); }
    void setBoxBackgroundColor(const Color& color) { m_boxBackgroundColor = color; markDirty(); }
    void setCheckedBackgroundColor(const Color& color) { m_checkedBackgroundColor = color; markDirty(); }

    // 重写渲染
    void render(SkCanvas* canvas) override;

    // 事件处理
    void onMousePressed(const MouseEvent& event) override;
    void onMouseEntered(const MouseEvent& event) override;
    void onMouseExited(const MouseEvent& event) override;

private:
    void updateAppearance();
    void drawCheckbox(SkCanvas* canvas, float x, float y);
    void drawCheckmark(SkCanvas* canvas, float x, float y, float size);
    void drawLabel(SkCanvas* canvas, float x, float y, float maxWidth);

    bool m_checked = false;
    std::string m_label;
    ControlState m_state = ControlState::Normal;
    
    // 样式
    float m_boxSize = 18.0f;
    float m_labelGap = 8.0f;  // 复选框和标签之间的间距
    Color m_checkColor = Color::White();
    Color m_boxBorderColor = Color::fromRGB(180, 180, 180);
    Color m_boxBackgroundColor = Color::White();
    Color m_checkedBackgroundColor = Color::fromRGB(66, 133, 244);
    Color m_hoverBorderColor = Color::fromRGB(66, 133, 244);
    Color m_disabledColor = Color::fromRGB(200, 200, 200);

    CheckedChangedCallback m_onChanged;
};

} // namespace liteDui
