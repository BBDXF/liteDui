/**
 * lite_radio_button.h - 单选按钮控件
 */

#pragma once

#include "lite_container.h"
#include <vector>

namespace liteDui {

class LiteRadioButton;
class LiteRadioGroup;
using LiteRadioButtonPtr = std::shared_ptr<LiteRadioButton>;
using LiteRadioGroupPtr = std::shared_ptr<LiteRadioGroup>;

/**
 * LiteRadioGroup - 单选按钮组
 * 
 * 管理一组单选按钮，确保同一时间只有一个被选中
 */
class LiteRadioGroup {
public:
    LiteRadioGroup() = default;
    ~LiteRadioGroup() = default;

    // 添加/移除单选按钮
    void addRadio(LiteRadioButton* radio);
    void removeRadio(LiteRadioButton* radio);

    // 设置/获取选中的单选按钮
    void setSelected(LiteRadioButton* radio);
    LiteRadioButton* getSelected() const { return m_selected; }

    // 获取选中的索引
    int getSelectedIndex() const;

    // 设置选择变化回调
    void setOnSelectionChanged(SelectionChangedCallback callback);

private:
    std::vector<LiteRadioButton*> m_radios;
    LiteRadioButton* m_selected = nullptr;
    SelectionChangedCallback m_onSelectionChanged;
};

/**
 * LiteRadioButton - 单选按钮控件
 * 
 * 支持单选组管理，同组内只能选中一个
 */
class LiteRadioButton : public LiteContainer {
public:
    LiteRadioButton();
    explicit LiteRadioButton(const std::string& label);
    ~LiteRadioButton() override;

    // 选中状态
    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }

    // 标签文本
    void setLabel(const std::string& label);
    const std::string& getLabel() const { return m_label; }

    // 禁用状态
    void setDisabled(bool disabled);
    bool isDisabled() const { return m_state == ControlState::Disabled; }

    // 单选组
    void setGroup(LiteRadioGroup* group);
    LiteRadioGroup* getGroup() const { return m_group; }

    // 回调（当此按钮被选中时触发）
    void setOnSelected(std::function<void()> callback);

    // 样式设置
    void setRadioColor(const Color& color) { m_radioColor = color; markDirty(); }
    Color getRadioColor() const { return m_radioColor; }
    
    void setRadioSize(float size) { m_radioSize = size; markDirty(); }
    float getRadioSize() const { return m_radioSize; }

    void setRadioBorderColor(const Color& color) { m_radioBorderColor = color; markDirty(); }
    void setSelectedColor(const Color& color) { m_selectedColor = color; markDirty(); }

    // 重写渲染
    void render(SkCanvas* canvas) override;

    // 事件处理
    void onMousePressed(const MouseEvent& event) override;
    void onMouseEntered(const MouseEvent& event) override;
    void onMouseExited(const MouseEvent& event) override;

private:
    void updateAppearance();
    void drawRadio(SkCanvas* canvas, float x, float y);
    void drawLabel(SkCanvas* canvas, float x, float y, float maxWidth);

    bool m_selected = false;
    std::string m_label;
    ControlState m_state = ControlState::Normal;
    LiteRadioGroup* m_group = nullptr;
    
    // 样式
    float m_radioSize = 18.0f;
    float m_labelGap = 8.0f;  // 单选按钮和标签之间的间距
    Color m_radioColor = Color::White();
    Color m_radioBorderColor = Color::fromRGB(180, 180, 180);
    Color m_selectedColor = Color::fromRGB(66, 133, 244);
    Color m_hoverBorderColor = Color::fromRGB(66, 133, 244);
    Color m_disabledColor = Color::fromRGB(200, 200, 200);

    std::function<void()> m_onSelected;
};

} // namespace liteDui
