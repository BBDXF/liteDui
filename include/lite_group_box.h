/**
 * lite_group_box.h - 分组框控件
 */

#pragma once

#include "lite_container.h"

namespace liteDui {

class LiteGroupBox;
using LiteGroupBoxPtr = std::shared_ptr<LiteGroupBox>;

/**
 * LiteGroupBox - 分组框控件
 * 
 * 带有标题的容器，用于将相关控件分组显示
 */
class LiteGroupBox : public LiteContainer {
public:
    LiteGroupBox();
    explicit LiteGroupBox(const std::string& title);
    ~LiteGroupBox() override = default;

    // 标题
    void setTitle(const std::string& title);
    const std::string& getTitle() const { return m_title; }

    // 标题样式
    void setTitleColor(const Color& color) { m_titleColor = color; markDirty(); }
    Color getTitleColor() const { return m_titleColor; }
    
    void setTitleFontSize(float size) { m_titleFontSize = size; markDirty(); }
    float getTitleFontSize() const { return m_titleFontSize; }

    // 边框样式
    void setFrameColor(const Color& color) { m_frameColor = color; markDirty(); }
    Color getFrameColor() const { return m_frameColor; }
    
    void setFrameWidth(float width) { m_frameWidth = width; markDirty(); }
    float getFrameWidth() const { return m_frameWidth; }

    // 内容区域
    void setContentPadding(const EdgeInsets& padding);
    
    // 获取标题高度（用于布局计算）
    float getTitleHeight() const { return m_titleHeight; }

    // 重写渲染
    void render(SkCanvas* canvas) override;

private:
    void drawFrame(SkCanvas* canvas, float x, float y, float w, float h);
    void drawTitle(SkCanvas* canvas, float x, float y);
    float measureTitleWidth() const;

    std::string m_title;
    Color m_titleColor = Color::fromRGB(33, 33, 33);
    float m_titleFontSize = 14.0f;
    float m_titleHeight = 20.0f;
    float m_titlePadding = 8.0f;  // 标题左右内边距
    
    Color m_frameColor = Color::fromRGB(200, 200, 200);
    float m_frameWidth = 1.0f;
    float m_frameRadius = 4.0f;
};

} // namespace liteDui
