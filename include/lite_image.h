/**
 * lite_image.h - 图片控件
 */

#pragma once

#include "lite_container.h"
#include "include/core/SkImage.h"

namespace liteDui {

class LiteImage;
using LiteImagePtr = std::shared_ptr<LiteImage>;

/**
 * 图片缩放模式枚举
 */
enum class ImageScaleMode {
    None,       // 原始大小，不缩放
    Fit,        // 保持比例适应容器（可能有空白）
    Fill,       // 保持比例填充容器（可能裁剪）
    Stretch     // 拉伸填充容器（可能变形）
};

/**
 * LiteImage - 图片控件
 * 
 * 支持从文件加载图片，支持多种缩放模式
 */
class LiteImage : public LiteContainer {
public:
    LiteImage();
    explicit LiteImage(const std::string& path);
    ~LiteImage() override = default;

    // 图片路径
    void setImagePath(const std::string& path);
    const std::string& getImagePath() const { return m_imagePath; }

    // 缩放模式
    void setScaleMode(ImageScaleMode mode);
    ImageScaleMode getScaleMode() const { return m_scaleMode; }

    // 占位符颜色（图片加载失败时显示）
    void setPlaceholderColor(const Color& color);
    Color getPlaceholderColor() const { return m_placeholderColor; }

    // 图片是否加载成功
    bool isLoaded() const { return m_image != nullptr; }
    bool isLoadFailed() const { return m_loadFailed; }

    // 获取原始图片尺寸
    int getImageWidth() const;
    int getImageHeight() const;

    // 重写渲染
    void render(SkCanvas* canvas) override;

private:
    void loadImage();
    void drawImage(SkCanvas* canvas, float x, float y, float w, float h);
    void drawPlaceholder(SkCanvas* canvas, float x, float y, float w, float h);

    std::string m_imagePath;
    sk_sp<SkImage> m_image;
    ImageScaleMode m_scaleMode = ImageScaleMode::Fit;
    Color m_placeholderColor = Color::LightGray();
    bool m_loadFailed = false;
};

} // namespace liteDui
