/**
 * lite_dialog.cpp - 对话框基类实现
 */

#include "lite_dialog.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRRect.h"
#include "include/core/SkPath.h"

namespace liteDui {

LiteDialog::LiteDialog() {
    setBackgroundColor(Color::fromRGB(0, 0, 0, 128));
}

void LiteDialog::setTitle(const std::string& title) { m_title = title; markDirty(); }
void LiteDialog::setModal(bool modal) { m_isModal = modal; }
void LiteDialog::setDialogSize(float width, float height) { m_dialogWidth = width; m_dialogHeight = height; markDirty(); }
void LiteDialog::setContent(LiteContainerPtr content) { m_content = content; markDirty(); }
void LiteDialog::setStandardButtons(int buttons) { m_standardButtons = buttons; markDirty(); }
void LiteDialog::setOnAccepted(std::function<void()> callback) { m_onAccepted = callback; }
void LiteDialog::setOnRejected(std::function<void()> callback) { m_onRejected = callback; }

void LiteDialog::show(LiteWindow* window) {
    if (!window) return;
    window->pushOverlay(std::dynamic_pointer_cast<LiteContainer>(shared_from_this()));
}

void LiteDialog::close() {
    if (m_window) {
        m_window->removeOverlay(std::dynamic_pointer_cast<LiteContainer>(shared_from_this()));
    }
}

void LiteDialog::accept() {
    m_result = DialogCode::Accepted;
    if (m_onAccepted) m_onAccepted();
    close();
}

void LiteDialog::reject() {
    m_result = DialogCode::Rejected;
    if (m_onRejected) m_onRejected();
    close();
}

void LiteDialog::render(SkCanvas* canvas) {
    float w = getLayoutWidth();
    float h = getLayoutHeight();

    // 模态背景
    if (m_isModal) {
        SkPaint overlayPaint;
        overlayPaint.setColor(SkColorSetARGB(128, 0, 0, 0));
        canvas->drawRect(SkRect::MakeXYWH(0, 0, w, h), overlayPaint);
    }

    // 对话框位置（相对坐标）
    float dlgX = (w - m_dialogWidth) / 2;
    float dlgY = (h - m_dialogHeight) / 2;

    // 对话框背景
    SkPaint bgPaint;
    bgPaint.setAntiAlias(true);
    bgPaint.setColor(Color::White().toARGB());
    canvas->drawRRect(SkRRect::MakeRectXY(SkRect::MakeXYWH(dlgX, dlgY, m_dialogWidth, m_dialogHeight), 8, 8), bgPaint);

    // 阴影效果
    SkPaint shadowPaint;
    shadowPaint.setAntiAlias(true);
    shadowPaint.setStyle(SkPaint::kStroke_Style);
    shadowPaint.setColor(SkColorSetARGB(30, 0, 0, 0));
    shadowPaint.setStrokeWidth(2);
    canvas->drawRRect(SkRRect::MakeRectXY(SkRect::MakeXYWH(dlgX - 1, dlgY - 1, m_dialogWidth + 2, m_dialogHeight + 2), 8, 8), shadowPaint);

    drawTitleBar(canvas, dlgX, dlgY, m_dialogWidth);
    drawButtons(canvas, dlgX, dlgY + m_dialogHeight - BUTTON_HEIGHT, m_dialogWidth);

    LiteContainer::render(canvas);
}

void LiteDialog::drawTitleBar(SkCanvas* canvas, float x, float y, float w) {
    // 标题栏背景
    SkPaint titleBgPaint;
    titleBgPaint.setColor(Color::fromRGB(245, 245, 245).toARGB());
    
    // 简化的圆角矩形顶部
    canvas->save();
    canvas->clipRRect(SkRRect::MakeRectXY(SkRect::MakeXYWH(x, y, w, TITLE_HEIGHT + 8), 8, 8));
    canvas->drawRect(SkRect::MakeXYWH(x, y, w, TITLE_HEIGHT), titleBgPaint);
    canvas->restore();

    // 标题文本
    if (!m_title.empty()) {
        setText(m_title);
        setTextColor(Color::Black());
        setFontSize(16.0f);
        drawText(canvas, x + 16 - getLeft(), y - getTop(), w - 32, TITLE_HEIGHT);
        setFontSize(14.0f);
    }
}

void LiteDialog::drawButtons(SkCanvas* canvas, float x, float y, float w) {
    std::vector<std::pair<int, std::string>> buttons;
    if (m_standardButtons & Ok) buttons.push_back({Ok, "确定"});
    if (m_standardButtons & Yes) buttons.push_back({Yes, "是"});
    if (m_standardButtons & No) buttons.push_back({No, "否"});
    if (m_standardButtons & Cancel) buttons.push_back({Cancel, "取消"});
    if (m_standardButtons & Close) buttons.push_back({Close, "关闭"});
    if (m_standardButtons & Apply) buttons.push_back({Apply, "应用"});

    if (buttons.empty()) return;

    float btnWidth = 80.0f;
    float btnHeight = 28.0f;
    float spacing = 12.0f;
    float totalWidth = buttons.size() * btnWidth + (buttons.size() - 1) * spacing;
    float startX = x + w - totalWidth - 16;
    float btnY = y + (BUTTON_HEIGHT - btnHeight) / 2;

    for (size_t i = 0; i < buttons.size(); i++) {
        float btnX = startX + i * (btnWidth + spacing);
        bool isHover = (m_hoverButton == buttons[i].first);
        bool isPrimary = (buttons[i].first == Ok || buttons[i].first == Yes);

        SkPaint btnPaint;
        btnPaint.setAntiAlias(true);
        if (isPrimary) {
            btnPaint.setColor(isHover ? Color::fromRGB(45, 100, 200).toARGB() : Color::fromRGB(66, 133, 244).toARGB());
        } else {
            btnPaint.setColor(isHover ? Color::fromRGB(230, 230, 230).toARGB() : Color::fromRGB(245, 245, 245).toARGB());
        }
        canvas->drawRRect(SkRRect::MakeRectXY(SkRect::MakeXYWH(btnX, btnY, btnWidth, btnHeight), 4, 4), btnPaint);

        // 边框
        SkPaint borderPaint;
        borderPaint.setAntiAlias(true);
        borderPaint.setStyle(SkPaint::kStroke_Style);
        borderPaint.setColor(isPrimary ? Color::fromRGB(66, 133, 244).toARGB() : Color::fromRGB(200, 200, 200).toARGB());
        canvas->drawRRect(SkRRect::MakeRectXY(SkRect::MakeXYWH(btnX, btnY, btnWidth, btnHeight), 4, 4), borderPaint);

        setText(buttons[i].second);
        setTextColor(isPrimary ? Color::White() : Color::Black());
        setTextAlign(TextAlign::Center);
        drawText(canvas, btnX - getLeft(), btnY - getTop(), btnWidth, btnHeight);
        setTextAlign(TextAlign::Left);
    }
}

int LiteDialog::getButtonAtPosition(float px, float py, float x, float y, float w) const {
    std::vector<int> buttons;
    if (m_standardButtons & Ok) buttons.push_back(Ok);
    if (m_standardButtons & Yes) buttons.push_back(Yes);
    if (m_standardButtons & No) buttons.push_back(No);
    if (m_standardButtons & Cancel) buttons.push_back(Cancel);
    if (m_standardButtons & Close) buttons.push_back(Close);
    if (m_standardButtons & Apply) buttons.push_back(Apply);

    float btnWidth = 80.0f;
    float btnHeight = 28.0f;
    float spacing = 12.0f;
    float totalWidth = buttons.size() * btnWidth + (buttons.size() - 1) * spacing;
    float startX = x + w - totalWidth - 16;
    float btnY = y + (BUTTON_HEIGHT - btnHeight) / 2;

    for (size_t i = 0; i < buttons.size(); i++) {
        float btnX = startX + i * (btnWidth + spacing);
        if (px >= btnX && px < btnX + btnWidth && py >= btnY && py < btnY + btnHeight) {
            return buttons[i];
        }
    }
    return -1;
}

void LiteDialog::onMousePressed(const MouseEvent& event) {
    float w = getLayoutWidth();
    float h = getLayoutHeight();
    float dlgX = (w - m_dialogWidth) / 2;
    float dlgY = (h - m_dialogHeight) / 2;

    // 转换为本地坐标
    float localX = event.x - getLeft();
    float localY = event.y - getTop();

    int btn = getButtonAtPosition(localX, localY, dlgX, dlgY + m_dialogHeight - BUTTON_HEIGHT, m_dialogWidth);
    if (btn == Ok || btn == Yes || btn == Apply) {
        accept();
    } else if (btn == Cancel || btn == No || btn == Close) {
        reject();
    }
}

void LiteDialog::onMouseMoved(const MouseEvent& event) {
    float w = getLayoutWidth();
    float h = getLayoutHeight();
    float dlgX = (w - m_dialogWidth) / 2;
    float dlgY = (h - m_dialogHeight) / 2;

    // 转换为本地坐标
    float localX = event.x - getLeft();
    float localY = event.y - getTop();

    int newHover = getButtonAtPosition(localX, localY, dlgX, dlgY + m_dialogHeight - BUTTON_HEIGHT, m_dialogWidth);
    if (newHover != m_hoverButton) {
        m_hoverButton = newHover;
        markDirty();
    }
}

} // namespace liteDui
