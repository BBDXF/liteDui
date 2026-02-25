/**
 * lite_tooltip.h - Tooltip 提示控件
 */

#pragma once

#include "lite_container.h"

namespace liteDui {

class LiteTooltipOverlay;
using LiteTooltipOverlayPtr = std::shared_ptr<LiteTooltipOverlay>;

/**
 * LiteTooltipOverlay - Tooltip 气泡渲染层
 * 
 * 作为独立的渲染层（不放入 overlay 栈），在所有内容之上绘制 tooltip 气泡。
 * 由 LiteWindow 统一管理生命周期。
 */
class LiteTooltipOverlay : public LiteContainer {
public:
    LiteTooltipOverlay();
    ~LiteTooltipOverlay() override = default;

    /**
     * 设置 tooltip 显示内容和锚点位置
     * @param text 提示文本
     * @param anchorX 目标控件的绝对 X 坐标
     * @param anchorY 目标控件的绝对 Y 坐标
     * @param anchorW 目标控件宽度
     * @param anchorH 目标控件高度
     * @param windowW 窗口宽度
     * @param windowH 窗口高度
     */
    void show(const std::string& text, float anchorX, float anchorY,
              float anchorW, float anchorH, float windowW, float windowH);

    void render(SkCanvas* canvas) override;

    // 样式设置
    void setTipBackgroundColor(const Color& color) { m_tipBgColor = color; }
    void setTipTextColor(const Color& color) { m_tipTextColor = color; }
    void setTipFontSize(float size) { m_tipFontSize = size; }

private:
    std::string m_tipText;
    float m_tipX = 0;
    float m_tipY = 0;
    float m_tipWidth = 0;
    float m_tipHeight = 0;

    // 样式
    Color m_tipBgColor = Color::fromRGB(50, 50, 50);
    Color m_tipTextColor = Color::White();
    float m_tipFontSize = 12.0f;

    // 常量
    static constexpr float kPaddingH = 8.0f;
    static constexpr float kPaddingV = 4.0f;
    static constexpr float kBorderRadius = 4.0f;
    static constexpr float kGap = 4.0f;
};

} // namespace liteDui
