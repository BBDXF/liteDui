/**
 * lite_container.h - 基于Skia的容器基类
 */

#pragma once

#include "lite_layout.h"
#include "lite_font_manager.h"

// 前向声明
namespace skia::textlayout {
    class Paragraph;
}

namespace liteDui {

class LiteContainer;
using LiteContainerPtr = std::shared_ptr<LiteContainer>;

/**
 * LiteContainer - 容器类，支持背景、边框、文本绘制
 */
class LiteContainer : public LiteLayout {
public:
    LiteContainer();
    explicit LiteContainer(const std::string& name);
    virtual ~LiteContainer();

    // 背景属性
    void setBackgroundColor(const Color& color);
    Color getBackgroundColor() const { return m_backgroundColor; }

    // 边框属性
    void setBorderColor(const Color& color);
    void setBorderStyle(BorderStyle style);
    void setBorderRadius(const EdgeInsets& radius);
    Color getBorderColor() const { return m_borderColor; }
    const EdgeInsets& getBorderRadius() const { return m_borderRadius; }

    // 文本属性
    void setText(const std::string& text);
    void setTextColor(const Color& color);
    void setFontSize(float size);
    void setFontFamily(const std::string& family);
    void setTextAlign(TextAlign align);
    const std::string& getText() const { return m_text; }
    Color getTextColor() const { return m_textColor; }
    float getFontSize() const { return m_fontSize; }
    const std::string& getFontFamily() const { return m_fontFamily; }
    TextAlign getTextAlign() const { return m_textAlign; }

    // 重写渲染函数
    void render(SkCanvas* canvas) override;

    // 事件处理虚函数
    virtual void onMousePressed(const MouseEvent& event) {}
    virtual void onMouseReleased(const MouseEvent& event) {}
    virtual void onMouseMoved(const MouseEvent& event) {}
    virtual void onMouseEntered(const MouseEvent& event) {}
    virtual void onMouseExited(const MouseEvent& event) {}
    virtual void onKeyPressed(const KeyEvent& event) {}
    virtual void onCharInput(unsigned int codepoint) {}

protected:
    void drawBackground(SkCanvas* canvas, float x, float y, float w, float h);
    void drawBorder(SkCanvas* canvas, float x, float y, float w, float h);
    void drawText(SkCanvas* canvas, float x, float y, float w, float h);

    // 字体样式辅助方法 - 子类可直接使用
    /**
     * 获取字体管理器实例
     */
    LiteFontManager& getFontManager() const;

    /**
     * 创建当前容器配置的 TextStyle
     */
    skia::textlayout::TextStyle getTextStyle() const;

    /**
     * 创建当前容器配置的 ParagraphStyle
     */
    skia::textlayout::ParagraphStyle getParagraphStyle() const;

    /**
     * 创建当前容器配置的 SkFont（用于简单文本测量）
     */
    SkFont getFont() const;

    // 背景属性
    Color m_backgroundColor = Color::White();

    // 边框属性
    Color m_borderColor = Color::Transparent();
    BorderStyle m_borderStyle = BorderStyle::Solid;
    EdgeInsets m_borderRadius;

    // 文本属性
    std::string m_text;
    Color m_textColor = Color::Black();
    float m_fontSize = 14.0f;
    std::string m_fontFamily = LITE_DEFAULT_FONT_FAMILY;
    TextAlign m_textAlign = TextAlign::Left;
};

} // namespace liteDui
