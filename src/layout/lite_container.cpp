/**
 * lite_container.cpp - 基于Skia的容器实现
 */

#include "lite_container.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRRect.h"
#include "include/core/SkFont.h"
#include "include/core/SkFontMgr.h"
#include "include/effects/SkDashPathEffect.h"
#include "modules/skparagraph/include/FontCollection.h"
#include "modules/skparagraph/include/ParagraphBuilder.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include "modules/skparagraph/include/TextStyle.h"
#include "include/ports/SkFontMgr_fontconfig.h"
#include "include/ports/SkFontScanner_FreeType.h"

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

    // 使用skparagraph绘制文本
    static sk_sp<SkFontMgr> fontMgr = SkFontMgr_New_FontConfig(nullptr, SkFontScanner_Make_FreeType());
    static auto fontCollection = sk_make_sp<FontCollection>();
    static bool initialized = false;
    if (!initialized) {
        fontCollection->setDefaultFontManager(fontMgr);
        fontCollection->enableFontFallback();
        initialized = true;
    }

    ParagraphStyle paraStyle;
    if (m_textAlign == TextAlign::Center) {
        paraStyle.setTextAlign(skia::textlayout::TextAlign::kCenter);
    } else if (m_textAlign == TextAlign::Right) {
        paraStyle.setTextAlign(skia::textlayout::TextAlign::kRight);
    } else {
        paraStyle.setTextAlign(skia::textlayout::TextAlign::kLeft);
    }

    TextStyle textStyle;
    textStyle.setColor(m_textColor.toARGB());
    textStyle.setFontSize(m_fontSize);
    textStyle.setFontFamilies({SkString(m_fontFamily.c_str())});

    auto builder = ParagraphBuilder::make(paraStyle, fontCollection);
    builder->pushStyle(textStyle);
    builder->addText(m_text.c_str());

    auto paragraph = builder->Build();
    paragraph->layout(textW);
    paragraph->paint(canvas, textX, textY);
}

} // namespace liteDui
