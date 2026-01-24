/**
 * lite_slider.cpp - 滑块控件实现
 */

#include "lite_slider.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRRect.h"
#include <cmath>

namespace liteDui {

LiteSlider::LiteSlider() {
    setBackgroundColor(Color::Transparent());
    setHeight(24.0f);
}

void LiteSlider::setValue(float value) {
    float newValue = std::max(m_minimum, std::min(m_maximum, value));
    if (m_step > 0) {
        newValue = std::round((newValue - m_minimum) / m_step) * m_step + m_minimum;
    }
    if (newValue != m_value) {
        m_value = newValue;
        if (m_onValueChanged) m_onValueChanged(m_value);
        markDirty();
    }
}

void LiteSlider::setMinimum(float min) { m_minimum = min; markDirty(); }
void LiteSlider::setMaximum(float max) { m_maximum = max; markDirty(); }
void LiteSlider::setStep(float step) { m_step = step; }
void LiteSlider::setOrientation(SliderOrientation orientation) { m_orientation = orientation; markDirty(); }
void LiteSlider::setTickPosition(SliderTickPosition position) { m_tickPosition = position; markDirty(); }
void LiteSlider::setTickInterval(float interval) { m_tickInterval = interval; markDirty(); }
void LiteSlider::setTrackColor(const Color& color) { m_trackColor = color; markDirty(); }
void LiteSlider::setFillColor(const Color& color) { m_fillColor = color; markDirty(); }
void LiteSlider::setThumbColor(const Color& color) { m_thumbColor = color; markDirty(); }
void LiteSlider::setThumbSize(float size) { m_thumbSize = size; markDirty(); }
void LiteSlider::setOnValueChanged(std::function<void(float)> callback) { m_onValueChanged = callback; }

float LiteSlider::valueToPosition(float value) const {
    float range = m_maximum - m_minimum;
    if (range <= 0) return 0;
    return (value - m_minimum) / range;
}

float LiteSlider::positionToValue(float pos) const {
    return m_minimum + pos * (m_maximum - m_minimum);
}

void LiteSlider::render(SkCanvas* canvas) {
    float w = getLayoutWidth();
    float h = getLayoutHeight();

    float trackHeight = 4.0f;
    float trackY = (h - trackHeight) / 2;
    float thumbRadius = m_thumbSize / 2;

    // 绘制轨道背景
    SkPaint trackPaint;
    trackPaint.setAntiAlias(true);
    trackPaint.setColor(m_trackColor.toARGB());
    canvas->drawRRect(SkRRect::MakeRectXY(SkRect::MakeXYWH(0, trackY, w, trackHeight), 2, 2), trackPaint);

    // 绘制填充部分
    float progress = valueToPosition(m_value);
    float fillWidth = (w - m_thumbSize) * progress;
    SkPaint fillPaint;
    fillPaint.setAntiAlias(true);
    fillPaint.setColor(m_fillColor.toARGB());
    canvas->drawRRect(SkRRect::MakeRectXY(SkRect::MakeXYWH(0, trackY, fillWidth + thumbRadius, trackHeight), 2, 2), fillPaint);

    // 绘制刻度
    if (m_tickPosition != SliderTickPosition::None && m_tickInterval > 0) {
        SkPaint tickPaint;
        tickPaint.setAntiAlias(true);
        tickPaint.setColor(m_trackColor.toARGB());
        tickPaint.setStrokeWidth(1);
        for (float v = m_minimum; v <= m_maximum; v += m_tickInterval) {
            float tickX = (w - m_thumbSize) * valueToPosition(v) + thumbRadius;
            if (m_tickPosition == SliderTickPosition::Above || m_tickPosition == SliderTickPosition::Both) {
                canvas->drawLine(tickX, trackY - 4, tickX, trackY, tickPaint);
            }
            if (m_tickPosition == SliderTickPosition::Below || m_tickPosition == SliderTickPosition::Both) {
                canvas->drawLine(tickX, trackY + trackHeight, tickX, trackY + trackHeight + 4, tickPaint);
            }
        }
    }

    // 绘制滑块
    float thumbX = (w - m_thumbSize) * progress + thumbRadius;
    float thumbY = h / 2;
    SkPaint thumbPaint;
    thumbPaint.setAntiAlias(true);
    thumbPaint.setColor(m_thumbColor.toARGB());
    canvas->drawCircle(thumbX, thumbY, thumbRadius, thumbPaint);
    
    // 滑块边框
    SkPaint borderPaint;
    borderPaint.setAntiAlias(true);
    borderPaint.setStyle(SkPaint::kStroke_Style);
    borderPaint.setColor(m_fillColor.toARGB());
    borderPaint.setStrokeWidth(2);
    canvas->drawCircle(thumbX, thumbY, thumbRadius, borderPaint);
}

void LiteSlider::onMousePressed(const MouseEvent& event) {
    float w = getLayoutWidth();
    float localX = event.x - getLeft();
    float pos = (localX - m_thumbSize / 2) / (w - m_thumbSize);
    pos = std::max(0.0f, std::min(1.0f, pos));
    setValue(positionToValue(pos));
    m_isDragging = true;
}

void LiteSlider::onMouseReleased(const MouseEvent& event) {
    m_isDragging = false;
}

void LiteSlider::onMouseMoved(const MouseEvent& event) {
    if (m_isDragging) {
        float w = getLayoutWidth();
        float localX = event.x - getLeft();
        float pos = (localX - m_thumbSize / 2) / (w - m_thumbSize);
        pos = std::max(0.0f, std::min(1.0f, pos));
        setValue(positionToValue(pos));
    }
}

} // namespace liteDui
