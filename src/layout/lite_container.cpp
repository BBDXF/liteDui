/**
 * lite_container.cpp - 基于Skia的容器实现
 */

#include "lite_container.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRRect.h"
#include "include/effects/SkDashPathEffect.h"
#include "modules/skparagraph/include/ParagraphBuilder.h"

using namespace skia::textlayout;

namespace liteDui {

LiteContainer::LiteContainer() {
    setDisplay(Display::Flex);
    setFlexDirection(FlexDirection::Column);
}

LiteContainer::LiteContainer(const std::string& name) : LiteContainer() {
    setText(name);
}

LiteContainer::~LiteContainer() = default;

// 背景属性
void LiteContainer::setBackgroundColor(const Color& color) {
    m_backgroundColor = color;
    markDirty();
}

// 边框属性
void LiteContainer::setBorderColor(const Color& color) {
    m_borderColor = color;
    markDirty();
}

void LiteContainer::setBorderStyle(BorderStyle style) {
    m_borderStyle = style;
    markDirty();
}

void LiteContainer::setBorderRadius(const EdgeInsets& radius) {
    m_borderRadius = radius;
    markDirty();
}

// 文本属性
void LiteContainer::setText(const std::string& text) {
    m_text = text;
    markDirty();
}

void LiteContainer::setTextColor(const Color& color) {
    m_textColor = color;
    markDirty();
}

void LiteContainer::setFontSize(float size) {
    m_fontSize = size;
    markDirty();
}

void LiteContainer::setFontFamily(const std::string& family) {
    m_fontFamily = family;
    markDirty();
}

void LiteContainer::setTextAlign(TextAlign align) {
    m_textAlign = align;
    markDirty();
}

// 渲染
void LiteContainer::render(SkCanvas* canvas) {
    if (!canvas) return;

    float w = getLayoutWidth();
    float h = getLayoutHeight();
    if (w <= 0 || h <= 0) return;

    drawBackground(canvas, 0, 0, w, h);
    drawBorder(canvas, 0, 0, w, h);
    drawText(canvas, 0, 0, w, h);
}

void LiteContainer::drawBackground(SkCanvas* canvas, float x, float y, float w, float h) {
    if (m_backgroundColor.a <= 0) return;

    SkPaint paint;
    paint.setColor(m_backgroundColor.toARGB());
    paint.setStyle(SkPaint::kFill_Style);
    paint.setAntiAlias(true);

    float radius = m_borderRadius.left.value;
    if (radius > 0) {
        SkRRect rrect = SkRRect::MakeRectXY(SkRect::MakeXYWH(x, y, w, h), radius, radius);
        canvas->drawRRect(rrect, paint);
    } else {
        canvas->drawRect(SkRect::MakeXYWH(x, y, w, h), paint);
    }
}

void LiteContainer::drawBorder(SkCanvas* canvas, float x, float y, float w, float h) {
    float borderWidth = getLayoutBorderLeft();
    if (borderWidth <= 0 || m_borderColor.a <= 0) return;

    SkPaint paint;
    paint.setColor(m_borderColor.toARGB());
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(borderWidth);
    paint.setAntiAlias(true);

    if (m_borderStyle == BorderStyle::Dashed) {
        float intervals[] = {5.0f, 5.0f};
        paint.setPathEffect(SkDashPathEffect::Make(SkSpan(intervals), 0));
    } else if (m_borderStyle == BorderStyle::Dotted) {
        float intervals[] = {2.0f, 2.0f};
        paint.setPathEffect(SkDashPathEffect::Make(SkSpan(intervals), 0));
    }

    float half = borderWidth / 2;
    float radius = m_borderRadius.left.value;
    if (radius > 0) {
        SkRRect rrect = SkRRect::MakeRectXY(
            SkRect::MakeXYWH(x + half, y + half, w - borderWidth, h - borderWidth),
            radius, radius);
        canvas->drawRRect(rrect, paint);
    } else {
        canvas->drawRect(
            SkRect::MakeXYWH(x + half, y + half, w - borderWidth, h - borderWidth), paint);
    }
}

void LiteContainer::drawText(SkCanvas* canvas, float x, float y, float w, float h) {
    if (m_text.empty()) return;

    float padL = getLayoutPaddingLeft();
    float padT = getLayoutPaddingTop();
    float padR = getLayoutPaddingRight();
    float borderL = getLayoutBorderLeft();
    float borderT = getLayoutBorderTop();
    float borderR = getLayoutBorderRight();

    float textX = x + borderL + padL;
    float textY = y + borderT + padT;
    float textW = w - borderL - borderR - padL - padR;

    if (textW <= 0) return;

    // 使用 LiteFontManager 获取字体资源和样式
    auto& fontMgr = getFontManager();
    auto fontCollection = fontMgr.getFontCollection();
    auto paraStyle = getParagraphStyle();
    auto textStyle = getTextStyle();

    auto builder = ParagraphBuilder::make(paraStyle, fontCollection);
    builder->pushStyle(textStyle);
    builder->addText(m_text.c_str());

    auto paragraph = builder->Build();
    paragraph->layout(textW);
    paragraph->paint(canvas, textX, textY);
}

// 字体样式辅助方法实现
LiteFontManager& LiteContainer::getFontManager() const {
    return LiteFontManager::getInstance();
}

skia::textlayout::TextStyle LiteContainer::getTextStyle() const {
    return getFontManager().createTextStyle(m_textColor, m_fontSize, m_fontFamily);
}

skia::textlayout::ParagraphStyle LiteContainer::getParagraphStyle() const {
    return getFontManager().createParagraphStyle(m_textAlign);
}

SkFont LiteContainer::getFont() const {
    return getFontManager().createFont(m_fontSize, m_fontFamily);
}

} // namespace liteDui
