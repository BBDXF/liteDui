/**
 * lite_label.cpp - 文本标签控件实现
 */

#include "lite_label.h"
#include "include/core/SkCanvas.h"
#include <modules/skparagraph/include/Paragraph.h>
#include <modules/skparagraph/include/ParagraphBuilder.h>

namespace liteDui {

LiteLabel::LiteLabel() {
    setBackgroundColor(Color::Transparent());
    setBorderColor(Color::Transparent());
}

LiteLabel::LiteLabel(const std::string& text) : LiteLabel() {
    setText(text);
}

void LiteLabel::setDisplayMode(LabelDisplayMode mode) {
    m_displayMode = mode;
    markDirty();
}

void LiteLabel::setVerticalAlign(Align align) {
    m_verticalAlign = align;
    markDirty();
}

void LiteLabel::setWordWrap(bool wrap) {
    m_wordWrap = wrap;
    markDirty();
}

void LiteLabel::setMaxLines(int maxLines) {
    m_maxLines = maxLines;
    markDirty();
}

void LiteLabel::setUrl(const std::string& url) {
    m_url = url;
}

void LiteLabel::setLinkColor(const Color& color) {
    m_linkColor = color;
    markDirty();
}

void LiteLabel::setOnClick(MouseEventCallback callback) {
    m_onClick = callback;
}

void LiteLabel::render(SkCanvas* canvas) {
    float w = getLayoutWidth();
    float h = getLayoutHeight();

    drawBackground(canvas, 0, 0, w, h);
    drawBorder(canvas, 0, 0, w, h);

    if (m_text.empty()) {
        return;
    }

    // 设置文本颜色（链接使用链接颜色）
    Color textColor = m_url.empty() ? m_textColor : (m_isHovered ? Color::fromRGB(26, 13, 171) : m_linkColor);

    auto& fontMgr = getFontManager();
    auto textStyle = getTextStyle();
    textStyle.setColor(SkColorSetARGB(
        static_cast<uint8_t>(textColor.a * 255),
        static_cast<uint8_t>(textColor.r * 255),
        static_cast<uint8_t>(textColor.g * 255),
        static_cast<uint8_t>(textColor.b * 255)));

    // 链接添加下划线
    if (!m_url.empty()) {
        textStyle.setDecoration(skia::textlayout::TextDecoration::kUnderline);
    }

    auto paraStyle = getParagraphStyle();
    if (m_displayMode == LabelDisplayMode::Ellipsis) {
        paraStyle.setEllipsis(u"...");
        paraStyle.setMaxLines(1);
    } else if (m_maxLines > 0) {
        paraStyle.setMaxLines(m_maxLines);
    }

    auto builder = skia::textlayout::ParagraphBuilder::make(paraStyle, fontMgr.getFontCollection());
    builder->pushStyle(textStyle);
    builder->addText(m_text.c_str(), m_text.size());
    auto paragraph = builder->Build();
    paragraph->layout(w);

    float textY = 0;
    float textHeight = paragraph->getHeight();
    if (m_verticalAlign == Align::Center) {
        textY = (h - textHeight) / 2;
    } else if (m_verticalAlign == Align::FlexEnd) {
        textY = h - textHeight;
    }

    paragraph->paint(canvas, 0, textY);
}

void LiteLabel::onMousePressed(const MouseEvent& event) {
    if (m_onClick) {
        m_onClick(event);
    }
}

void LiteLabel::onMouseEntered(const MouseEvent& event) {
    if (!m_url.empty()) {
        m_isHovered = true;
        markDirty();
    }
}

void LiteLabel::onMouseExited(const MouseEvent& event) {
    if (m_isHovered) {
        m_isHovered = false;
        markDirty();
    }
}

} // namespace liteDui
