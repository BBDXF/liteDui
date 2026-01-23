/**
 * lite_radio_button.cpp - 单选按钮控件实现
 */

#include "lite_radio_button.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "modules/skparagraph/include/ParagraphBuilder.h"
#include "modules/skparagraph/include/Paragraph.h"
#include <algorithm>

using namespace skia::textlayout;

namespace liteDui {

// ==================== LiteRadioGroup 实现 ====================

void LiteRadioGroup::addRadio(LiteRadioButton* radio) {
    if (!radio) return;
    
    auto it = std::find(m_radios.begin(), m_radios.end(), radio);
    if (it == m_radios.end()) {
        m_radios.push_back(radio);
    }
}

void LiteRadioGroup::removeRadio(LiteRadioButton* radio) {
    auto it = std::find(m_radios.begin(), m_radios.end(), radio);
    if (it != m_radios.end()) {
        m_radios.erase(it);
        if (m_selected == radio) {
            m_selected = nullptr;
        }
    }
}

void LiteRadioGroup::setSelected(LiteRadioButton* radio) {
    if (m_selected == radio) return;
    
    // 取消之前选中的
    if (m_selected) {
        m_selected->setSelected(false);
    }
    
    m_selected = radio;
    
    // 选中新的
    if (m_selected) {
        m_selected->setSelected(true);
    }
    
    // 触发回调
    if (m_onSelectionChanged) {
        m_onSelectionChanged(getSelectedIndex());
    }
}

int LiteRadioGroup::getSelectedIndex() const {
    if (!m_selected) return -1;
    
    auto it = std::find(m_radios.begin(), m_radios.end(), m_selected);
    if (it != m_radios.end()) {
        return static_cast<int>(std::distance(m_radios.begin(), it));
    }
    return -1;
}

void LiteRadioGroup::setOnSelectionChanged(SelectionChangedCallback callback) {
    m_onSelectionChanged = callback;
}

// ==================== LiteRadioButton 实现 ====================

LiteRadioButton::LiteRadioButton() {
    setBackgroundColor(Color::Transparent());
    setFlexDirection(FlexDirection::Row);
    setAlignItems(Align::Center);
}

LiteRadioButton::LiteRadioButton(const std::string& label) : LiteRadioButton() {
    m_label = label;
}

LiteRadioButton::~LiteRadioButton() {
    // 从组中移除自己
    if (m_group) {
        m_group->removeRadio(this);
    }
}

void LiteRadioButton::setSelected(bool selected) {
    if (m_selected == selected) return;
    m_selected = selected;
    markDirty();
    
    // 如果被选中且在组中，通知组
    if (selected && m_group && m_group->getSelected() != this) {
        m_group->setSelected(this);
    }
    
    // 触发选中回调
    if (selected && m_onSelected) {
        m_onSelected();
    }
}

void LiteRadioButton::setLabel(const std::string& label) {
    m_label = label;
    markDirty();
}

void LiteRadioButton::setDisabled(bool disabled) {
    ControlState newState = disabled ? ControlState::Disabled : ControlState::Normal;
    if (m_state == newState) return;
    m_state = newState;
    updateAppearance();
}

void LiteRadioButton::setGroup(LiteRadioGroup* group) {
    if (m_group == group) return;
    
    // 从旧组中移除
    if (m_group) {
        m_group->removeRadio(this);
    }
    
    m_group = group;
    
    // 添加到新组
    if (m_group) {
        m_group->addRadio(this);
    }
}

void LiteRadioButton::setOnSelected(std::function<void()> callback) {
    m_onSelected = callback;
}

void LiteRadioButton::updateAppearance() {
    markDirty();
}

void LiteRadioButton::render(SkCanvas* canvas) {
    if (!canvas) return;

    float w = getLayoutWidth();
    float h = getLayoutHeight();
    if (w <= 0 || h <= 0) return;

    // 绘制背景
    drawBackground(canvas, 0, 0, w, h);

    float borderL = getLayoutBorderLeft();
    float borderT = getLayoutBorderTop();
    float padL = getLayoutPaddingLeft();
    float padT = getLayoutPaddingTop();
    float padR = getLayoutPaddingRight();

    float contentX = borderL + padL;
    float contentY = borderT + padT;
    float contentW = w - borderL - getLayoutBorderRight() - padL - padR;

    // 计算单选按钮的垂直居中位置
    float radioY = contentY + (h - borderT - getLayoutBorderBottom() - padT - getLayoutPaddingBottom() - m_radioSize) / 2;

    // 绘制单选按钮
    drawRadio(canvas, contentX, radioY);

    // 绘制标签
    if (!m_label.empty()) {
        float labelX = contentX + m_radioSize + m_labelGap;
        float labelMaxWidth = contentW - m_radioSize - m_labelGap;
        if (labelMaxWidth > 0) {
            drawLabel(canvas, labelX, contentY, labelMaxWidth);
        }
    }

    // 绘制边框
    drawBorder(canvas, 0, 0, w, h);
}

void LiteRadioButton::drawRadio(SkCanvas* canvas, float x, float y) {
    SkPaint paint;
    paint.setAntiAlias(true);

    bool isDisabled = (m_state == ControlState::Disabled);
    bool isHover = (m_state == ControlState::Hover);

    float centerX = x + m_radioSize / 2;
    float centerY = y + m_radioSize / 2;
    float outerRadius = m_radioSize / 2;

    // 绘制外圆背景
    paint.setStyle(SkPaint::kFill_Style);
    paint.setColor(m_radioColor.toARGB());
    canvas->drawCircle(centerX, centerY, outerRadius, paint);

    // 绘制外圆边框
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(1.5f);
    if (isDisabled) {
        paint.setColor(m_disabledColor.toARGB());
    } else if (isHover || m_selected) {
        paint.setColor(m_hoverBorderColor.toARGB());
    } else {
        paint.setColor(m_radioBorderColor.toARGB());
    }
    canvas->drawCircle(centerX, centerY, outerRadius - 0.75f, paint);

    // 如果选中，绘制内圆
    if (m_selected) {
        paint.setStyle(SkPaint::kFill_Style);
        if (isDisabled) {
            paint.setColor(m_disabledColor.toARGB());
        } else {
            paint.setColor(m_selectedColor.toARGB());
        }
        float innerRadius = outerRadius * 0.5f;
        canvas->drawCircle(centerX, centerY, innerRadius, paint);
    }
}

void LiteRadioButton::drawLabel(SkCanvas* canvas, float x, float y, float maxWidth) {
    auto& fontMgr = getFontManager();
    auto fontCollection = fontMgr.getFontCollection();

    Color textColor = (m_state == ControlState::Disabled) 
                      ? m_disabledColor 
                      : getTextColor();

    ParagraphStyle paraStyle;
    paraStyle.setTextAlign(skia::textlayout::TextAlign::kLeft);

    auto textStyle = fontMgr.createTextStyle(textColor, getFontSize(), getFontFamily());

    auto builder = ParagraphBuilder::make(paraStyle, fontCollection);
    builder->pushStyle(textStyle);
    builder->addText(m_label.c_str());

    auto paragraph = builder->Build();
    paragraph->layout(maxWidth);

    // 垂直居中
    float textHeight = paragraph->getHeight();
    float containerHeight = getLayoutHeight() - getLayoutBorderTop() - getLayoutBorderBottom()
                           - getLayoutPaddingTop() - getLayoutPaddingBottom();
    float textY = y + (containerHeight - textHeight) / 2;

    paragraph->paint(canvas, x, textY);
}

void LiteRadioButton::onMousePressed(const MouseEvent& event) {
    if (m_state == ControlState::Disabled) return;
    
    if (event.button == MouseButton::Left) {
        if (m_group) {
            m_group->setSelected(this);
        } else {
            setSelected(true);
        }
    }
}

void LiteRadioButton::onMouseEntered(const MouseEvent& event) {
    if (m_state == ControlState::Disabled) return;
    m_state = ControlState::Hover;
    updateAppearance();
}

void LiteRadioButton::onMouseExited(const MouseEvent& event) {
    if (m_state == ControlState::Disabled) return;
    m_state = ControlState::Normal;
    updateAppearance();
}

} // namespace liteDui
