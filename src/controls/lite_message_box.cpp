/**
 * lite_message_box.cpp - 消息框实现
 */

#include "lite_message_box.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRRect.h"
#include "include/core/SkPath.h"

namespace liteDui {

LiteMessageBox::LiteMessageBox() {
    setDialogSize(360.0f, 160.0f);
}

void LiteMessageBox::setIcon(MessageBoxIcon icon) { m_icon = icon; markDirty(); }
void LiteMessageBox::setMessage(const std::string& message) { m_message = message; markDirty(); }
void LiteMessageBox::setDetailedText(const std::string& text) { m_detailedText = text; markDirty(); }

LiteMessageBoxPtr LiteMessageBox::information(const std::string& title, const std::string& message, int buttons) {
    auto box = std::make_shared<LiteMessageBox>();
    box->setTitle(title);
    box->setMessage(message);
    box->setIcon(MessageBoxIcon::Information);
    box->setStandardButtons(buttons);
    return box;
}

LiteMessageBoxPtr LiteMessageBox::warning(const std::string& title, const std::string& message, int buttons) {
    auto box = std::make_shared<LiteMessageBox>();
    box->setTitle(title);
    box->setMessage(message);
    box->setIcon(MessageBoxIcon::Warning);
    box->setStandardButtons(buttons);
    return box;
}

LiteMessageBoxPtr LiteMessageBox::error(const std::string& title, const std::string& message, int buttons) {
    auto box = std::make_shared<LiteMessageBox>();
    box->setTitle(title);
    box->setMessage(message);
    box->setIcon(MessageBoxIcon::Error);
    box->setStandardButtons(buttons);
    return box;
}

LiteMessageBoxPtr LiteMessageBox::question(const std::string& title, const std::string& message, int buttons) {
    auto box = std::make_shared<LiteMessageBox>();
    box->setTitle(title);
    box->setMessage(message);
    box->setIcon(MessageBoxIcon::Question);
    box->setStandardButtons(buttons);
    return box;
}

void LiteMessageBox::render(SkCanvas* canvas) {
    // 先调用基类渲染对话框框架
    LiteDialog::render(canvas);

    float w = getLayoutWidth();
    float h = getLayoutHeight();
    float dlgX = (w - m_dialogWidth) / 2;
    float dlgY = (h - m_dialogHeight) / 2;

    // 内容区域
    float contentY = dlgY + TITLE_HEIGHT + 16;
    float contentH = m_dialogHeight - TITLE_HEIGHT - BUTTON_HEIGHT - 32;
    float iconSize = 40.0f;
    float textX = dlgX + 24;

    if (m_icon != MessageBoxIcon::None) {
        drawIcon(canvas, dlgX + 24, contentY + (contentH - iconSize) / 2, iconSize);
        textX = dlgX + 24 + iconSize + 16;
    }

    // 消息文本
    if (!m_message.empty()) {
        setText(m_message);
        setTextColor(Color::Black());
        drawText(canvas, textX - getLeft(), contentY - getTop(), m_dialogWidth - (textX - dlgX) - 24, contentH);
    }
}

void LiteMessageBox::drawIcon(SkCanvas* canvas, float x, float y, float size) {
    SkPaint paint;
    paint.setAntiAlias(true);
    float cx = x + size / 2;
    float cy = y + size / 2;
    float r = size / 2 - 2;

    switch (m_icon) {
    case MessageBoxIcon::Information:
        paint.setColor(Color::fromRGB(66, 133, 244).toARGB());
        canvas->drawCircle(cx, cy, r, paint);
        paint.setColor(Color::White().toARGB());
        paint.setStrokeWidth(3);
        paint.setStyle(SkPaint::kStroke_Style);
        canvas->drawLine(cx, cy - 4, cx, cy + 8, paint);
        paint.setStyle(SkPaint::kFill_Style);
        canvas->drawCircle(cx, cy - 10, 2.5f, paint);
        break;

    case MessageBoxIcon::Warning:
        paint.setColor(Color::fromRGB(251, 188, 5).toARGB());
        // 绘制三角形警告图标（使用三条线）
        canvas->drawLine(cx, y + 4, x + size - 4, y + size - 4, paint);
        canvas->drawLine(x + size - 4, y + size - 4, x + 4, y + size - 4, paint);
        canvas->drawLine(x + 4, y + size - 4, cx, y + 4, paint);
        paint.setColor(Color::Black().toARGB());
        paint.setStrokeWidth(2.5f);
        paint.setStyle(SkPaint::kStroke_Style);
        canvas->drawLine(cx, cy - 6, cx, cy + 4, paint);
        paint.setStyle(SkPaint::kFill_Style);
        canvas->drawCircle(cx, cy + 10, 2, paint);
        break;

    case MessageBoxIcon::Error:
        paint.setColor(Color::fromRGB(234, 67, 53).toARGB());
        canvas->drawCircle(cx, cy, r, paint);
        paint.setColor(Color::White().toARGB());
        paint.setStrokeWidth(3);
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeCap(SkPaint::kRound_Cap);
        canvas->drawLine(cx - 8, cy - 8, cx + 8, cy + 8, paint);
        canvas->drawLine(cx + 8, cy - 8, cx - 8, cy + 8, paint);
        break;

    case MessageBoxIcon::Question:
        paint.setColor(Color::fromRGB(66, 133, 244).toARGB());
        canvas->drawCircle(cx, cy, r, paint);
        setText("?");
        setFontSize(24.0f);
        setTextColor(Color::White());
        setTextAlign(TextAlign::Center);
        drawText(canvas, x - getLeft(), y - getTop() - 2, size, size);
        setFontSize(14.0f);
        setTextAlign(TextAlign::Left);
        break;

    default:
        break;
    }
}

} // namespace liteDui
