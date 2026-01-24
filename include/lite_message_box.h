/**
 * lite_message_box.h - 消息框
 */

#pragma once

#include "lite_dialog.h"

namespace liteDui {

class LiteMessageBox;
using LiteMessageBoxPtr = std::shared_ptr<LiteMessageBox>;

enum class MessageBoxIcon {
    None,
    Information,
    Warning,
    Error,
    Question
};

class LiteMessageBox : public LiteDialog {
public:
    LiteMessageBox();
    ~LiteMessageBox() override = default;
    
    void setIcon(MessageBoxIcon icon);
    MessageBoxIcon getIcon() const { return m_icon; }
    void setMessage(const std::string& message);
    const std::string& getMessage() const { return m_message; }
    void setDetailedText(const std::string& text);
    
    // 静态便捷方法
    static LiteMessageBoxPtr information(const std::string& title, const std::string& message, int buttons = Ok);
    static LiteMessageBoxPtr warning(const std::string& title, const std::string& message, int buttons = Ok);
    static LiteMessageBoxPtr error(const std::string& title, const std::string& message, int buttons = Ok);
    static LiteMessageBoxPtr question(const std::string& title, const std::string& message, int buttons = Yes | No);
    
    void render(SkCanvas* canvas) override;

private:
    void drawIcon(SkCanvas* canvas, float x, float y, float size);
    
    MessageBoxIcon m_icon = MessageBoxIcon::None;
    std::string m_message;
    std::string m_detailedText;
};

} // namespace liteDui
