/**
 * lite_slider.h - 滑块控件
 */

#pragma once

#include "lite_container.h"

namespace liteDui {

class LiteSlider;
using LiteSliderPtr = std::shared_ptr<LiteSlider>;

enum class SliderOrientation { Horizontal, Vertical };
enum class SliderTickPosition { None, Above, Below, Both };

class LiteSlider : public LiteContainer {
public:
    LiteSlider();
    ~LiteSlider() override = default;

    void setValue(float value);
    float getValue() const { return m_value; }
    void setMinimum(float min);
    void setMaximum(float max);
    void setStep(float step);
    void setOrientation(SliderOrientation orientation);
    void setTickPosition(SliderTickPosition position);
    void setTickInterval(float interval);
    void setTrackColor(const Color& color);
    void setFillColor(const Color& color);
    void setThumbColor(const Color& color);
    void setThumbSize(float size);
    
    void setOnValueChanged(std::function<void(float)> callback);
    
    void render(SkCanvas* canvas) override;
    void onMousePressed(const MouseEvent& event) override;
    void onMouseReleased(const MouseEvent& event) override;
    void onMouseMoved(const MouseEvent& event) override;

private:
    float valueToPosition(float value) const;
    float positionToValue(float pos) const;
    
    float m_value = 0.0f;
    float m_minimum = 0.0f;
    float m_maximum = 100.0f;
    float m_step = 1.0f;
    SliderOrientation m_orientation = SliderOrientation::Horizontal;
    SliderTickPosition m_tickPosition = SliderTickPosition::None;
    float m_tickInterval = 10.0f;
    Color m_trackColor = Color::fromRGB(200, 200, 200);
    Color m_fillColor = Color::fromRGB(66, 133, 244);
    Color m_thumbColor = Color::White();
    float m_thumbSize = 16.0f;
    bool m_isDragging = false;
    std::function<void(float)> m_onValueChanged;
};

} // namespace liteDui
