/**
 * lite_tooltip.cpp - Tooltip 提示控件实现
 */

#include "lite_tooltip.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRRect.h"
#include "include/core/SkFont.h"
#include "modules/skparagraph/include/Paragraph.h"
#include "modules/skparagraph/include/ParagraphBuilder.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include "modules/skparagraph/include/TextStyle.h"

namespace liteDui {

LiteTooltipOverlay::LiteTooltipOverlay() {
    setBackgroundColor(Color::Transparent());
}

void LiteTooltipOverlay::show(const std::string& text, float anchorX, float anchorY,
                               float anchorW, float anchorH, float windowW, float windowH) {
    m_tipText = text;
    if (m_tipText.empty()) return;

    // 测量文本尺寸
    setFontSize(m_tipFontSize);
    auto& fontMgr = getFontManager();
    
    skia::textlayout::TextStyle textStyle;
    textStyle.setFontSize(m_tipFontSize);
    textStyle.setColor(m_tipTextColor.toARGB());
    std::vector<SkString> families;
    families.push_back(SkString(getFontFamily().c_str()));
    textStyle.setFontFamilies(families);

    skia::textlayout::ParagraphStyle paraStyle;
    paraStyle.setTextStyle(textStyle);

    auto builder = skia::textlayout::ParagraphBuilder::make(
        paraStyle, fontMgr.getFontCollection());
    builder->pushStyle(textStyle);
    builder->addText(m_tipText.c_str(), m_tipText.size());
    auto paragraph = builder->Build();
    paragraph->layout(windowW);  // 用窗口宽度作为最大约束

    float textWidth = paragraph->getMaxIntrinsicWidth();
    float textHeight = paragraph->getHeight();

    m_tipWidth = textWidth + kPaddingH * 2;
    m_tipHeight = textHeight + kPaddingV * 2;

    // 位置计算：优先显示在控件下方
    m_tipX = anchorX;
    m_tipY = anchorY + anchorH + kGap;

    // 如果下方空间不足，翻转到上方
    if (m_tipY + m_tipHeight > windowH) {
        m_tipY = anchorY - m_tipHeight - kGap;
    }

    // 如果上方也不够，就贴着下方显示
    if (m_tipY < 0) {
        m_tipY = anchorY + anchorH + kGap;
    }

    // 水平方向：防止超出窗口右边界
    if (m_tipX + m_tipWidth > windowW) {
        m_tipX = windowW - m_tipWidth;
    }
    // 防止超出左边界
    if (m_tipX < 0) {
        m_tipX = 0;
    }
}

void LiteTooltipOverlay::render(SkCanvas* canvas) {
    if (m_tipText.empty() || m_tipWidth <= 0) return;

    // 绘制阴影
    SkPaint shadowPaint;
    shadowPaint.setAntiAlias(true);
    shadowPaint.setColor(Color::fromRGB(0, 0, 0, 60).toARGB());
    SkRRect shadowRRect = SkRRect::MakeRectXY(
        SkRect::MakeXYWH(m_tipX + 1, m_tipY + 1, m_tipWidth, m_tipHeight),
        kBorderRadius, kBorderRadius);
    canvas->drawRRect(shadowRRect, shadowPaint);

    // 绘制背景
    SkPaint bgPaint;
    bgPaint.setAntiAlias(true);
    bgPaint.setColor(m_tipBgColor.toARGB());
    SkRRect bgRRect = SkRRect::MakeRectXY(
        SkRect::MakeXYWH(m_tipX, m_tipY, m_tipWidth, m_tipHeight),
        kBorderRadius, kBorderRadius);
    canvas->drawRRect(bgRRect, bgPaint);

    // 绘制文本
    setFontSize(m_tipFontSize);
    setTextColor(m_tipTextColor);
    setText(m_tipText);
    drawText(canvas, m_tipX + kPaddingH, m_tipY + kPaddingV,
             m_tipWidth - kPaddingH * 2, m_tipHeight - kPaddingV * 2);
}

} // namespace liteDui
