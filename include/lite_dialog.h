/**
 * lite_dialog.h - 对话框基类
 */

#pragma once

#include "lite_container.h"
#include "lite_window.h"

namespace liteDui {

class LiteDialog;
using LiteDialogPtr = std::shared_ptr<LiteDialog>;

enum class DialogCode { None, Accepted, Rejected };

enum StandardButton {
    NoButton = 0,
    Ok = 0x0001,
    Cancel = 0x0002,
    Yes = 0x0004,
    No = 0x0008,
    Close = 0x0010,
    Apply = 0x0020
};

class LiteDialog : public LiteContainer {
public:
    LiteDialog();
    ~LiteDialog() override = default;
    
    void show(LiteWindow* window);
    void close();
    
    void setTitle(const std::string& title);
    const std::string& getTitle() const { return m_title; }
    void setModal(bool modal);
    bool isModal() const { return m_isModal; }
    void setDialogSize(float width, float height);
    void setContent(LiteContainerPtr content);
    void setStandardButtons(int buttons);
    
    DialogCode getResult() const { return m_result; }
    void accept();
    void reject();
    
    void setOnAccepted(std::function<void()> callback);
    void setOnRejected(std::function<void()> callback);
    
    void render(SkCanvas* canvas) override;
    void onMouseReleased(const MouseEvent& event) override;
    void onMouseMoved(const MouseEvent& event) override;

protected:
    void drawTitleBar(SkCanvas* canvas, float x, float y, float w);
    void drawButtons(SkCanvas* canvas, float x, float y, float w);
    int getButtonAtPosition(float px, float py, float x, float y, float w) const;
    
    /** 独立绘制文本，不修改父类 m_text 等成员 */
    void drawStandaloneText(SkCanvas* canvas, const std::string& text,
                            const Color& color, float fontSize, TextAlign align,
                            float x, float y, float w, float h);
    
    std::string m_title;
    bool m_isModal = true;
    float m_dialogWidth = 400.0f;
    float m_dialogHeight = 200.0f;
    LiteContainerPtr m_content;
    int m_standardButtons = Ok | Cancel;
    DialogCode m_result = DialogCode::None;
    std::function<void()> m_onAccepted;
    std::function<void()> m_onRejected;
    int m_hoverButton = -1;
    
    static constexpr float TITLE_HEIGHT = 36.0f;
    static constexpr float BUTTON_HEIGHT = 40.0f;
};

} // namespace liteDui
