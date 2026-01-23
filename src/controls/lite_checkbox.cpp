/**
 * lite_checkbox.cpp - 复选框控件实现
 */

#include "lite_checkbox.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkRRect.h"
#include "modules/skparagraph/include/ParagraphBuilder.h"
#include "modules/skparagraph/include/Paragraph.h"

using namespace skia::textlayout;

namespace liteDui {

LiteCheckbox::LiteCheckbox() {
    setBackgroundColor(Color::Transparent());
    setFlexDirection(FlexDirection::Row);
    setAlignItems(Align::Center);
}

LiteCheckbox::LiteCheckbox(const std::string& label) : LiteCheckbox() {
    m_label = label;
}

void LiteCheckbox::setChecked(bool checked) {
    if (m_checked == checked) return;
    m_checked = checked;
    markDirty();
    
    if (m_onChanged) {
        m_onChanged(m_checked);
    }
}

void LiteCheckbox::toggle() {
    setChecked(!m_checked);
}

void LiteCheckbox::setLabel(const std::string& label) {
    m_label = label;
    markDirty();
}

void LiteCheckbox::setDisabled(bool disabled) {
    ControlState newState = disabled ? ControlState::Disabled : ControlState::Normal;
    if (m_state == newState) return;
    m_state = newState;
    updateAppearance();
}

void LiteCheckbox::setOnChanged(CheckedChangedCallback callback) {
    m_onChanged = callback;
}

void LiteCheckbox::updateAppearance() {
    markDirty();
}

void LiteCheckbox::render(SkCanvas* canvas) {
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

    // 计算复选框的垂直居中位置
    float boxY = contentY + (h - borderT - getLayoutBorderBottom() - padT - getLayoutPaddingBottom() - m_boxSize) / 2;

    // 绘制复选框
    drawCheckbox(canvas, contentX, boxY);

    // 绘制标签
    if (!m_label.empty()) {
        float labelX = contentX + m_boxSize + m_labelGap;
        float labelMaxWidth = contentW - m_boxSize - m_labelGap;
        if (labelMaxWidth > 0) {
            drawLabel(canvas, labelX, contentY, labelMaxWidth);
        }
    }

    // 绘制边框
    drawBorder(canvas, 0, 0, w, h);
}

void LiteCheckbox::drawCheckbox(SkCanvas* canvas, float x, float y) {
    SkPaint paint;
    paint.setAntiAlias(true);

    bool isDisabled = (m_state == ControlState::Disabled);
    bool isHover = (m_state == ControlState::Hover);

    // 绘制复选框背景
    paint.setStyle(SkPaint::kFill_Style);
    if (m_checked) {
        paint.setColor(isDisabled ? m_disabledColor.toARGB() : m_checkedBackgroundColor.toARGB());
    } else {
        paint.setColor(m_boxBackgroundColor.toARGB());
    }

    float radius = 3.0f;
    SkRRect boxRRect = SkRRect::MakeRectXY(
        SkRect::MakeXYWH(x, y, m_boxSize, m_boxSize), radius, radius);
    canvas->drawRRect(boxRRect, paint);

    // 绘制复选框边框
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(1.5f);
    if (isDisabled) {
        paint.setColor(m_disabledColor.toARGB());
    } else if (isHover || m_checked) {
        paint.setColor(m_hoverBorderColor.toARGB());
    } else {
        paint.setColor(m_boxBorderColor.toARGB());
    }
    canvas->drawRRect(boxRRect, paint);

    // 如果选中，绘制勾选标记
    if (m_checked) {
        drawCheckmark(canvas, x, y, m_boxSize);
    }
}

void LiteCheckbox::drawCheckmark(SkCanvas* canvas, float x, float y, float size) {
    SkPaint paint;
    paint.setColor(m_checkColor.toARGB());
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(2.0f);
    paint.setAntiAlias(true);
    paint.setStrokeCap(SkPaint::kRound_Cap);
    paint.setStrokeJoin(SkPaint::kRound_Join);

    // 绘制勾选标记（两条线段）
    // 第一条线：从左下到中间
    float x1 = x + size * 0.2f;
    float y1 = y + size * 0.5f;
    float x2 = x + size * 0.4f;
    float y2 = y + size * 0.7f;
    canvas->drawLine(x1, y1, x2, y2, paint);

    // 第二条线：从中间到右上
    float x3 = x + size * 0.8f;
    float y3 = y + size * 0.3f;
    canvas->drawLine(x2, y2, x3, y3, paint);
}

void LiteCheckbox::drawLabel(SkCanvas* canvas, float x, float y, float maxWidth) {
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

void LiteCheckbox::onMousePressed(const MouseEvent& event) {
    if (m_state == ControlState::Disabled) return;
    
    if (event.button == MouseButton::Left) {
        toggle();
    }
}

void LiteCheckbox::onMouseEntered(const MouseEvent& event) {
    if (m_state == ControlState::Disabled) return;
    m_state = ControlState::Hover;
    updateAppearance();
}

void LiteCheckbox::onMouseExited(const MouseEvent& event) {
    if (m_state == ControlState::Disabled) return;
    m_state = ControlState::Normal;
    updateAppearance();
}

} // namespace liteDui
