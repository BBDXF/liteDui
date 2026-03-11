/**
 * lite_color_picker.cpp - 颜色选择对话框实现
 */

#include "lite_color_picker.h"
#include "lite_label.h"
#include "include/core/SkCanvas.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace liteDui {

LiteColorPicker::LiteColorPicker() {
    setDialogSize(400.0f, 380.0f);
    setTitle("选择颜色");
    setStandardButtons(Ok | Cancel);
    
    buildUI();
}

void LiteColorPicker::setColor(const Color& color) {
    m_selectedColor = color;
    rgbToHsv(color, m_hue, m_saturation, m_value);
    updateFromRGB();
    markDirty();
}

LiteColorPickerPtr LiteColorPicker::create(const std::string& title, const Color& initialColor) {
    auto picker = std::make_shared<LiteColorPicker>();
    picker->setTitle(title);
    picker->setColor(initialColor);
    return picker;
}

void LiteColorPicker::buildUI() {
    auto content = std::make_shared<LiteContainer>();
    content->setFlexDirection(FlexDirection::Column);
    content->setPadding(EdgeInsets::All(12));
    content->setGap(12);
    
    auto inputRow = std::make_shared<LiteContainer>();
    inputRow->setFlexDirection(FlexDirection::Row);
    inputRow->setHeight(32);
    inputRow->setGap(8);
    inputRow->setAlignItems(Align::Center);
    
    auto hexLabel = std::make_shared<LiteLabel>("Hex:");
    hexLabel->setWidth(40);
    inputRow->addChild(hexLabel);
    
    m_hexInput = std::make_shared<LiteInput>();
    m_hexInput->setWidth(INPUT_WIDTH + 20);
    m_hexInput->setHeight(28);
    inputRow->addChild(m_hexInput);
    
    auto rLabel = std::make_shared<LiteLabel>("R:");
    rLabel->setWidth(20);
    inputRow->addChild(rLabel);
    
    m_rInput = std::make_shared<LiteInput>();
    m_rInput->setWidth(INPUT_WIDTH);
    m_rInput->setHeight(28);
    inputRow->addChild(m_rInput);
    
    auto gLabel = std::make_shared<LiteLabel>("G:");
    gLabel->setWidth(20);
    inputRow->addChild(gLabel);
    
    m_gInput = std::make_shared<LiteInput>();
    m_gInput->setWidth(INPUT_WIDTH);
    m_gInput->setHeight(28);
    inputRow->addChild(m_gInput);
    
    auto bLabel = std::make_shared<LiteLabel>("B:");
    bLabel->setWidth(20);
    inputRow->addChild(bLabel);
    
    m_bInput = std::make_shared<LiteInput>();
    m_bInput->setWidth(INPUT_WIDTH);
    m_bInput->setHeight(28);
    inputRow->addChild(m_bInput);
    
    content->addChild(inputRow);
    setContent(content);
    updateFromHSV();
}

void LiteColorPicker::updateFromHSV() {
    m_selectedColor = hsvToRgb(m_hue, m_saturation, m_value);
    
    if (m_hexInput) {
        std::stringstream ss;
        ss << "#" << std::hex << std::setfill('0') << std::setw(2) 
           << static_cast<int>(m_selectedColor.r * 255)
           << std::setw(2) << static_cast<int>(m_selectedColor.g * 255)
           << std::setw(2) << static_cast<int>(m_selectedColor.b * 255);
        m_hexInput->setText(ss.str());
    }
    
    if (m_rInput) {
        m_rInput->setText(std::to_string(static_cast<int>(m_selectedColor.r * 255)));
    }
    if (m_gInput) {
        m_gInput->setText(std::to_string(static_cast<int>(m_selectedColor.g * 255)));
    }
    if (m_bInput) {
        m_bInput->setText(std::to_string(static_cast<int>(m_selectedColor.b * 255)));
    }
    
    markDirty();
}

void LiteColorPicker::updateFromRGB() {
    if (m_hexInput) {
        std::stringstream ss;
        ss << "#" << std::hex << std::setfill('0') << std::setw(2) 
           << static_cast<int>(m_selectedColor.r * 255)
           << std::setw(2) << static_cast<int>(m_selectedColor.g * 255)
           << std::setw(2) << static_cast<int>(m_selectedColor.b * 255);
        m_hexInput->setText(ss.str());
    }
    
    if (m_rInput) {
        m_rInput->setText(std::to_string(static_cast<int>(m_selectedColor.r * 255)));
    }
    if (m_gInput) {
        m_gInput->setText(std::to_string(static_cast<int>(m_selectedColor.g * 255)));
    }
    if (m_bInput) {
        m_bInput->setText(std::to_string(static_cast<int>(m_selectedColor.b * 255)));
    }
    
    markDirty();
}

Color LiteColorPicker::hsvToRgb(float h, float s, float v) const {
    float c = v * s;
    float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;
    
    float r, g, b;
    if (h < 60) {
        r = c; g = x; b = 0;
    } else if (h < 120) {
        r = x; g = c; b = 0;
    } else if (h < 180) {
        r = 0; g = c; b = x;
    } else if (h < 240) {
        r = 0; g = x; b = c;
    } else if (h < 300) {
        r = x; g = 0; b = c;
    } else {
        r = c; g = 0; b = x;
    }
    
    return Color(r + m, g + m, b + m, 1.0f);
}

void LiteColorPicker::rgbToHsv(const Color& rgb, float& h, float& s, float& v) const {
    float r = rgb.r, g = rgb.g, b = rgb.b;
    float max = std::max({r, g, b});
    float min = std::min({r, g, b});
    float delta = max - min;
    
    v = max;
    s = (max > 0.0f) ? (delta / max) : 0.0f;
    
    if (delta == 0.0f) {
        h = 0.0f;
    } else if (max == r) {
        h = 60.0f * std::fmod((g - b) / delta, 6.0f);
    } else if (max == g) {
        h = 60.0f * ((b - r) / delta + 2.0f);
    } else {
        h = 60.0f * ((r - g) / delta + 4.0f);
    }
    
    if (h < 0.0f) h += 360.0f;
}

void LiteColorPicker::render(SkCanvas* canvas) {
    LiteDialog::render(canvas);
    
    float w = getLayoutWidth();
    float h = getLayoutHeight();
    float dlgX = (w - m_dialogWidth) / 2;
    float dlgY = (h - m_dialogHeight) / 2;
    
    float contentY = dlgY + TITLE_HEIGHT + 12;
    float contentX = dlgX + 12;
    
    drawColorGrid(canvas, contentX, contentY + 44, COLOR_GRID_SIZE, COLOR_GRID_SIZE);
    drawHueBar(canvas, contentX + COLOR_GRID_SIZE + 12, contentY + 44, HUE_BAR_WIDTH, COLOR_GRID_SIZE);
    drawPreview(canvas, contentX + COLOR_GRID_SIZE + HUE_BAR_WIDTH + 24, contentY + 44, PREVIEW_SIZE, PREVIEW_SIZE);
}

void LiteColorPicker::drawColorGrid(SkCanvas* canvas, float x, float y, float w, float h) {
    const int steps = 20;
    float cellW = w / steps;
    float cellH = h / steps;
    
    for (int i = 0; i < steps; i++) {
        for (int j = 0; j < steps; j++) {
            float s = static_cast<float>(i) / (steps - 1);
            float v = 1.0f - static_cast<float>(j) / (steps - 1);
            Color color = hsvToRgb(m_hue, s, v);
            
            SkPaint paint;
            paint.setColor(color.toARGB());
            canvas->drawRect(SkRect::MakeXYWH(x + i * cellW, y + j * cellH, cellW, cellH), paint);
        }
    }
    
    SkPaint borderPaint;
    borderPaint.setStyle(SkPaint::kStroke_Style);
    borderPaint.setColor(Color::fromRGB(150, 150, 150).toARGB());
    canvas->drawRect(SkRect::MakeXYWH(x, y, w, h), borderPaint);
}

void LiteColorPicker::drawHueBar(SkCanvas* canvas, float x, float y, float w, float h) {
    const int steps = 36;
    float cellH = h / steps;
    
    for (int i = 0; i < steps; i++) {
        float hue = static_cast<float>(i) / steps * 360.0f;
        Color color = hsvToRgb(hue, 1.0f, 1.0f);
        
        SkPaint paint;
        paint.setColor(color.toARGB());
        canvas->drawRect(SkRect::MakeXYWH(x, y + i * cellH, w, cellH), paint);
    }
    
    SkPaint borderPaint;
    borderPaint.setStyle(SkPaint::kStroke_Style);
    borderPaint.setColor(Color::fromRGB(150, 150, 150).toARGB());
    canvas->drawRect(SkRect::MakeXYWH(x, y, w, h), borderPaint);
}

void LiteColorPicker::drawPreview(SkCanvas* canvas, float x, float y, float w, float h) {
    SkPaint paint;
    paint.setColor(m_selectedColor.toARGB());
    canvas->drawRect(SkRect::MakeXYWH(x, y, w, h), paint);
    
    SkPaint borderPaint;
    borderPaint.setStyle(SkPaint::kStroke_Style);
    borderPaint.setColor(Color::fromRGB(150, 150, 150).toARGB());
    canvas->drawRect(SkRect::MakeXYWH(x, y, w, h), borderPaint);
}

void LiteColorPicker::onMousePressed(const MouseEvent& event) {
    float w = getLayoutWidth();
    float h = getLayoutHeight();
    float dlgX = (w - m_dialogWidth) / 2;
    float dlgY = (h - m_dialogHeight) / 2;
    
    float contentY = dlgY + TITLE_HEIGHT + 12;
    float contentX = dlgX + 12;
    
    float localX = event.x - getLeft();
    float localY = event.y - getTop();
    
    float gridX = contentX;
    float gridY = contentY + 44;
    if (localX >= gridX && localX < gridX + COLOR_GRID_SIZE &&
        localY >= gridY && localY < gridY + COLOR_GRID_SIZE) {
        m_saturation = (localX - gridX) / COLOR_GRID_SIZE;
        m_value = 1.0f - (localY - gridY) / COLOR_GRID_SIZE;
        updateFromHSV();
        return;
    }
    
    float hueX = contentX + COLOR_GRID_SIZE + 12;
    float hueY = contentY + 44;
    if (localX >= hueX && localX < hueX + HUE_BAR_WIDTH &&
        localY >= hueY && localY < hueY + COLOR_GRID_SIZE) {
        m_hue = (localY - hueY) / COLOR_GRID_SIZE * 360.0f;
        updateFromHSV();
        return;
    }
    
    LiteDialog::onMousePressed(event);
}

void LiteColorPicker::onMouseMoved(const MouseEvent& event) {
    LiteDialog::onMouseMoved(event);
}

} // namespace liteDui
