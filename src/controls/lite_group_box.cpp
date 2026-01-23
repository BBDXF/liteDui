/**
 * lite_group_box.cpp - 分组框控件实现
 */

#include "lite_group_box.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkRRect.h"
#include "modules/skparagraph/include/ParagraphBuilder.h"
#include "modules/skparagraph/include/Paragraph.h"

using namespace skia::textlayout;

namespace liteDui {

LiteGroupBox::LiteGroupBox() {
    setBackgroundColor(Color::Transparent());
    setFlexDirection(FlexDirection::Column);
    // 为标题预留顶部空间
    setPadding(EdgeInsets(10, m_titleHeight + 5, 10, 10));
}

LiteGroupBox::LiteGroupBox(const std::string& title) : LiteGroupBox() {
    m_title = title;
}

void LiteGroupBox::setTitle(const std::string& title) {
    m_title = title;
    markDirty();
}

void LiteGroupBox::setContentPadding(const EdgeInsets& padding) {
    // 保持顶部为标题高度 + 用户指定的顶部内边距
    setPadding(EdgeInsets(
        padding.left.value,
        m_titleHeight + padding.top.value,
        padding.right.value,
        padding.bottom.value
    ));
}

float LiteGroupBox::measureTitleWidth() const {
    if (m_title.empty()) return 0;

    auto& fontMgr = getFontManager();
    auto fontCollection = fontMgr.getFontCollection();

    ParagraphStyle paraStyle;
    auto textStyle = fontMgr.createTextStyle(m_titleColor, m_titleFontSize, getFontFamily());

    auto builder = ParagraphBuilder::make(paraStyle, fontCollection);
    builder->pushStyle(textStyle);
    builder->addText(m_title.c_str());

    auto paragraph = builder->Build();
    paragraph->layout(10000);  // 足够大的宽度

    return paragraph->getMaxIntrinsicWidth();
}

void LiteGroupBox::render(SkCanvas* canvas) {
    if (!canvas) return;

    float w = getLayoutWidth();
    float h = getLayoutHeight();
    if (w <= 0 || h <= 0) return;

    // 绘制背景
    drawBackground(canvas, 0, 0, w, h);

    // 绘制边框（带标题缺口）
    drawFrame(canvas, 0, 0, w, h);

    // 绘制标题
    if (!m_title.empty()) {
        drawTitle(canvas, m_titlePadding + m_frameRadius, 0);
    }

    // 渲染子控件（由父类的 renderTree 处理）
}

void LiteGroupBox::drawFrame(SkCanvas* canvas, float x, float y, float w, float h) {
    SkPaint paint;
    paint.setColor(m_frameColor.toARGB());
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(m_frameWidth);
    paint.setAntiAlias(true);

    // 边框从标题中间开始
    float frameTop = y + m_titleHeight / 2;
    float frameHeight = h - m_titleHeight / 2;

    if (m_title.empty()) {
        // 没有标题时，绘制完整边框
        if (m_frameRadius > 0) {
            SkRRect rrect = SkRRect::MakeRectXY(
                SkRect::MakeXYWH(x + m_frameWidth / 2, frameTop + m_frameWidth / 2, 
                                 w - m_frameWidth, frameHeight - m_frameWidth),
                m_frameRadius, m_frameRadius);
            canvas->drawRRect(rrect, paint);
        } else {
            canvas->drawRect(
                SkRect::MakeXYWH(x + m_frameWidth / 2, frameTop + m_frameWidth / 2,
                                 w - m_frameWidth, frameHeight - m_frameWidth), paint);
        }
    } else {
        // 有标题时，绘制带缺口的边框
        float titleWidth = measureTitleWidth();
        float titleStartX = x + m_titlePadding;
        float titleEndX = titleStartX + titleWidth + m_titlePadding;

        // 绘制左边框
        canvas->drawLine(x + m_frameWidth / 2, frameTop + m_frameRadius,
                        x + m_frameWidth / 2, y + h - m_frameRadius, paint);
        
        // 绘制底边框
        canvas->drawLine(x + m_frameRadius, y + h - m_frameWidth / 2,
                        x + w - m_frameRadius, y + h - m_frameWidth / 2, paint);
        
        // 绘制右边框
        canvas->drawLine(x + w - m_frameWidth / 2, y + h - m_frameRadius,
                        x + w - m_frameWidth / 2, frameTop + m_frameRadius, paint);
        
        // 绘制顶边框（标题左侧部分）
        canvas->drawLine(x + m_frameRadius, frameTop + m_frameWidth / 2,
                        titleStartX, frameTop + m_frameWidth / 2, paint);
        
        // 绘制顶边框（标题右侧部分）
        canvas->drawLine(titleEndX, frameTop + m_frameWidth / 2,
                        x + w - m_frameRadius, frameTop + m_frameWidth / 2, paint);

        // 绘制圆角
        if (m_frameRadius > 0) {
            // 左上角
            SkRect arcRect = SkRect::MakeXYWH(x + m_frameWidth / 2, frameTop + m_frameWidth / 2,
                                              m_frameRadius * 2, m_frameRadius * 2);
            canvas->drawArc(arcRect, 180, 90, false, paint);
            
            // 右上角
            arcRect = SkRect::MakeXYWH(x + w - m_frameRadius * 2 - m_frameWidth / 2, 
                                       frameTop + m_frameWidth / 2,
                                       m_frameRadius * 2, m_frameRadius * 2);
            canvas->drawArc(arcRect, 270, 90, false, paint);
            
            // 左下角
            arcRect = SkRect::MakeXYWH(x + m_frameWidth / 2, 
                                       y + h - m_frameRadius * 2 - m_frameWidth / 2,
                                       m_frameRadius * 2, m_frameRadius * 2);
            canvas->drawArc(arcRect, 90, 90, false, paint);
            
            // 右下角
            arcRect = SkRect::MakeXYWH(x + w - m_frameRadius * 2 - m_frameWidth / 2,
                                       y + h - m_frameRadius * 2 - m_frameWidth / 2,
                                       m_frameRadius * 2, m_frameRadius * 2);
            canvas->drawArc(arcRect, 0, 90, false, paint);
        }
    }
}

void LiteGroupBox::drawTitle(SkCanvas* canvas, float x, float y) {
    if (m_title.empty()) return;

    auto& fontMgr = getFontManager();
    auto fontCollection = fontMgr.getFontCollection();

    ParagraphStyle paraStyle;
    paraStyle.setTextAlign(skia::textlayout::TextAlign::kLeft);

    auto textStyle = fontMgr.createTextStyle(m_titleColor, m_titleFontSize, getFontFamily());

    auto builder = ParagraphBuilder::make(paraStyle, fontCollection);
    builder->pushStyle(textStyle);
    builder->addText(m_title.c_str());

    auto paragraph = builder->Build();
    paragraph->layout(getLayoutWidth() - x * 2);

    // 垂直居中于标题区域
    float textHeight = paragraph->getHeight();
    float textY = y + (m_titleHeight - textHeight) / 2;

    paragraph->paint(canvas, x, textY);
}

} // namespace liteDui
