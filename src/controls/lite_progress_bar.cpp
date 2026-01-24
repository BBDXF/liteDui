/**
 * lite_progress_bar.cpp - 进度条控件实现
 */

#include "lite_progress_bar.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRRect.h"

namespace liteDui {

LiteProgressBar::LiteProgressBar() {
    setBackgroundColor(Color::fromRGB(230, 230, 230));
    setBorderRadius(EdgeInsets::All(4.0f));
    setHeight(8.0f);
}

void LiteProgressBar::setValue(float value) {
    m_value = std::max(m_minimum, std::min(m_maximum, value));
    markDirty();
}

void LiteProgressBar::setMinimum(float min) {
    m_minimum = min;
    if (m_value < m_minimum) m_value = m_minimum;
    markDirty();
}

void LiteProgressBar::setMaximum(float max) {
    m_maximum = max;
    if (m_value > m_maximum) m_value = m_maximum;
    markDirty();
}

void LiteProgressBar::setMode(ProgressBarMode mode) {
    m_mode = mode;
    markDirty();
}

void LiteProgressBar::setOrientation(ProgressBarOrientation orientation) {
    m_orientation = orientation;
    markDirty();
}

void LiteProgressBar::setProgressColor(const Color& color) {
    m_progressColor = color;
    markDirty();
}

void LiteProgressBar::setShowText(bool show) {
    m_showText = show;
    markDirty();
}

void LiteProgressBar::setTextFormat(const std::string& format) {
    m_textFormat = format;
    markDirty();
}

void LiteProgressBar::render(SkCanvas* canvas) {
    float w = getLayoutWidth();
    float h = getLayoutHeight();

    drawBackground(canvas, 0, 0, w, h);

    float progress = (m_value - m_minimum) / (m_maximum - m_minimum);
    float radius = m_borderRadius.left.value;

    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(m_progressColor.toARGB());

    if (m_mode == ProgressBarMode::Determinate) {
        if (m_orientation == ProgressBarOrientation::Horizontal) {
            float progressWidth = w * progress;
            if (progressWidth > 0) {
                SkRRect rrect = SkRRect::MakeRectXY(SkRect::MakeXYWH(0, 0, progressWidth, h), radius, radius);
                canvas->drawRRect(rrect, paint);
            }
        } else {
            float progressHeight = h * progress;
            if (progressHeight > 0) {
                SkRRect rrect = SkRRect::MakeRectXY(SkRect::MakeXYWH(0, h - progressHeight, w, progressHeight), radius, radius);
                canvas->drawRRect(rrect, paint);
            }
        }
    } else {
        // 不确定模式：绘制移动的条
        float barWidth = w * 0.3f;
        float barX = m_animationOffset * (w + barWidth) - barWidth;
        SkRRect rrect = SkRRect::MakeRectXY(SkRect::MakeXYWH(barX, 0, barWidth, h), radius, radius);
        canvas->save();
        canvas->clipRect(SkRect::MakeXYWH(0, 0, w, h));
        canvas->drawRRect(rrect, paint);
        canvas->restore();
    }

    if (m_showText && m_mode == ProgressBarMode::Determinate) {
        int percent = static_cast<int>(progress * 100);
        std::string text = std::to_string(percent) + "%";
        setText(text);
        drawText(canvas, 0, 0, w, h);
    }

    drawBorder(canvas, 0, 0, w, h);
}

void LiteProgressBar::update() {
    if (m_mode == ProgressBarMode::Indeterminate) {
        m_animationOffset += 0.02f;
        if (m_animationOffset > 1.0f) m_animationOffset = 0.0f;
        markDirty();
    }
    LiteContainer::update();
}

} // namespace liteDui
