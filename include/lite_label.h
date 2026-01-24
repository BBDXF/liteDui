/**
 * lite_label.h - 文本标签控件
 */

#pragma once

#include "lite_container.h"

namespace liteDui {

class LiteLabel;
using LiteLabelPtr = std::shared_ptr<LiteLabel>;

enum class LabelDisplayMode {
    SingleLine,
    MultiLine,
    Ellipsis
};

class LiteLabel : public LiteContainer {
public:
    LiteLabel();
    explicit LiteLabel(const std::string& text);
    ~LiteLabel() override = default;

    void setDisplayMode(LabelDisplayMode mode);
    LabelDisplayMode getDisplayMode() const { return m_displayMode; }
    
    void setVerticalAlign(Align align);
    void setWordWrap(bool wrap);
    void setMaxLines(int maxLines);
    void setUrl(const std::string& url);
    void setLinkColor(const Color& color);
    void setOnClick(MouseEventCallback callback);
    
    void render(SkCanvas* canvas) override;
    void onMousePressed(const MouseEvent& event) override;
    void onMouseEntered(const MouseEvent& event) override;
    void onMouseExited(const MouseEvent& event) override;

private:
    LabelDisplayMode m_displayMode = LabelDisplayMode::SingleLine;
    Align m_verticalAlign = Align::Center;
    bool m_wordWrap = false;
    int m_maxLines = 0;
    std::string m_url;
    Color m_linkColor = Color::fromRGB(66, 133, 244);
    bool m_isHovered = false;
    MouseEventCallback m_onClick;
};

} // namespace liteDui
