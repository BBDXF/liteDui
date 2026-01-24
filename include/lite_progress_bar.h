/**
 * lite_progress_bar.h - 进度条控件
 */

#pragma once

#include "lite_container.h"

namespace liteDui {

class LiteProgressBar;
using LiteProgressBarPtr = std::shared_ptr<LiteProgressBar>;

enum class ProgressBarOrientation { Horizontal, Vertical };
enum class ProgressBarMode { Determinate, Indeterminate };

class LiteProgressBar : public LiteContainer {
public:
    LiteProgressBar();
    ~LiteProgressBar() override = default;

    void setValue(float value);
    float getValue() const { return m_value; }
    void setMinimum(float min);
    void setMaximum(float max);
    void setMode(ProgressBarMode mode);
    void setOrientation(ProgressBarOrientation orientation);
    void setProgressColor(const Color& color);
    void setShowText(bool show);
    void setTextFormat(const std::string& format);
    
    void render(SkCanvas* canvas) override;
    void update() override;

private:
    float m_value = 0.0f;
    float m_minimum = 0.0f;
    float m_maximum = 100.0f;
    ProgressBarMode m_mode = ProgressBarMode::Determinate;
    ProgressBarOrientation m_orientation = ProgressBarOrientation::Horizontal;
    Color m_progressColor = Color::fromRGB(66, 133, 244);
    bool m_showText = false;
    std::string m_textFormat = "%p%";
    float m_animationOffset = 0.0f;
};

} // namespace liteDui
