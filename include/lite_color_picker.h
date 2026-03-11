/**
 * lite_color_picker.h - 颜色选择对话框
 */

#pragma once

#include "lite_dialog.h"
#include "lite_input.h"

namespace liteDui {

class LiteColorPicker;
using LiteColorPickerPtr = std::shared_ptr<LiteColorPicker>;

class LiteColorPicker : public LiteDialog {
public:
    LiteColorPicker();
    ~LiteColorPicker() override = default;
    
    void setColor(const Color& color);
    Color getColor() const { return m_selectedColor; }
    
    static LiteColorPickerPtr create(const std::string& title = "选择颜色",
                                     const Color& initialColor = Color::White());
    
    void render(SkCanvas* canvas) override;
    void onMousePressed(const MouseEvent& event) override;
    void onMouseMoved(const MouseEvent& event) override;

protected:
    void buildUI();
    void drawColorGrid(SkCanvas* canvas, float x, float y, float w, float h);
    void drawHueBar(SkCanvas* canvas, float x, float y, float w, float h);
    void drawPreview(SkCanvas* canvas, float x, float y, float w, float h);
    void updateFromHSV();
    void updateFromRGB();
    Color hsvToRgb(float h, float s, float v) const;
    void rgbToHsv(const Color& rgb, float& h, float& s, float& v) const;
    
    Color m_selectedColor = Color::White();
    float m_hue = 0.0f;
    float m_saturation = 1.0f;
    float m_value = 1.0f;
    
    std::shared_ptr<LiteInput> m_hexInput;
    std::shared_ptr<LiteInput> m_rInput;
    std::shared_ptr<LiteInput> m_gInput;
    std::shared_ptr<LiteInput> m_bInput;
    
    static constexpr float COLOR_GRID_SIZE = 200.0f;
    static constexpr float HUE_BAR_WIDTH = 30.0f;
    static constexpr float PREVIEW_SIZE = 60.0f;
    static constexpr float INPUT_WIDTH = 60.0f;
};

} // namespace liteDui
