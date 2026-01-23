/**
 * lite_image.cpp - 图片控件实现
 */

#include "lite_image.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkRRect.h"
#include "include/core/SkData.h"
#include "include/core/SkSamplingOptions.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkPath.h"
#include "include/codec/SkCodec.h"
#include <fstream>
#include <iostream>

namespace liteDui {

LiteImage::LiteImage() {
    setBackgroundColor(Color::Transparent());
}

LiteImage::LiteImage(const std::string& path) : LiteImage() {
    setImagePath(path);
}

void LiteImage::setImagePath(const std::string& path) {
    if (m_imagePath == path) return;
    
    m_imagePath = path;
    m_image = nullptr;
    m_loadFailed = false;
    
    if (!path.empty()) {
        loadImage();
    }
    
    markDirty();
}

void LiteImage::setScaleMode(ImageScaleMode mode) {
    if (m_scaleMode == mode) return;
    m_scaleMode = mode;
    markDirty();
}

void LiteImage::setPlaceholderColor(const Color& color) {
    m_placeholderColor = color;
    markDirty();
}

int LiteImage::getImageWidth() const {
    return m_image ? m_image->width() : 0;
}

int LiteImage::getImageHeight() const {
    return m_image ? m_image->height() : 0;
}

void LiteImage::loadImage() {
    if (m_imagePath.empty()) {
        m_loadFailed = true;
        return;
    }

    // 读取文件内容
    std::ifstream file(m_imagePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "LiteImage: Failed to open file: " << m_imagePath << std::endl;
        m_loadFailed = true;
        return;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        std::cerr << "LiteImage: Failed to read file: " << m_imagePath << std::endl;
        m_loadFailed = true;
        return;
    }

    // 创建 SkData
    sk_sp<SkData> data = SkData::MakeWithCopy(buffer.data(), buffer.size());
    if (!data) {
        std::cerr << "LiteImage: Failed to create SkData" << std::endl;
        m_loadFailed = true;
        return;
    }

    // 使用 SkCodec 解码图片
    std::unique_ptr<SkCodec> codec = SkCodec::MakeFromData(data);
    if (!codec) {
        std::cerr << "LiteImage: Failed to create codec for: " << m_imagePath << std::endl;
        m_loadFailed = true;
        return;
    }

    // 获取图片信息
    SkImageInfo info = codec->getInfo().makeColorType(kN32_SkColorType);
    
    // 创建位图并解码
    SkBitmap bitmap;
    if (!bitmap.tryAllocPixels(info)) {
        std::cerr << "LiteImage: Failed to allocate pixels" << std::endl;
        m_loadFailed = true;
        return;
    }

    SkCodec::Result result = codec->getPixels(info, bitmap.getPixels(), bitmap.rowBytes());
    if (result != SkCodec::kSuccess) {
        std::cerr << "LiteImage: Failed to decode image: " << m_imagePath << std::endl;
        m_loadFailed = true;
        return;
    }

    // 从位图创建图片
    m_image = bitmap.asImage();
    if (!m_image) {
        std::cerr << "LiteImage: Failed to create image from bitmap" << std::endl;
        m_loadFailed = true;
        return;
    }

    m_loadFailed = false;
    std::cout << "LiteImage: Loaded image: " << m_imagePath 
              << " (" << m_image->width() << "x" << m_image->height() << ")" << std::endl;
}

void LiteImage::render(SkCanvas* canvas) {
    if (!canvas) return;

    float w = getLayoutWidth();
    float h = getLayoutHeight();
    if (w <= 0 || h <= 0) return;

    // 绘制背景
    drawBackground(canvas, 0, 0, w, h);

    float borderL = getLayoutBorderLeft();
    float borderT = getLayoutBorderTop();
    float borderR = getLayoutBorderRight();
    float borderB = getLayoutBorderBottom();
    float padL = getLayoutPaddingLeft();
    float padT = getLayoutPaddingTop();
    float padR = getLayoutPaddingRight();
    float padB = getLayoutPaddingBottom();

    float contentX = borderL + padL;
    float contentY = borderT + padT;
    float contentW = w - borderL - borderR - padL - padR;
    float contentH = h - borderT - borderB - padT - padB;

    if (contentW <= 0 || contentH <= 0) return;

    // 绘制图片或占位符
    if (m_image) {
        drawImage(canvas, contentX, contentY, contentW, contentH);
    } else {
        drawPlaceholder(canvas, contentX, contentY, contentW, contentH);
    }

    // 绘制边框
    drawBorder(canvas, 0, 0, w, h);
}

void LiteImage::drawImage(SkCanvas* canvas, float x, float y, float w, float h) {
    if (!m_image) return;

    float imgW = static_cast<float>(m_image->width());
    float imgH = static_cast<float>(m_image->height());

    SkRect srcRect = SkRect::MakeWH(imgW, imgH);
    SkRect dstRect;

    switch (m_scaleMode) {
        case ImageScaleMode::None: {
            // 原始大小，居中显示
            float dstX = x + (w - imgW) / 2;
            float dstY = y + (h - imgH) / 2;
            dstRect = SkRect::MakeXYWH(dstX, dstY, imgW, imgH);
            break;
        }
        case ImageScaleMode::Fit: {
            // 保持比例适应容器
            float scale = std::min(w / imgW, h / imgH);
            float scaledW = imgW * scale;
            float scaledH = imgH * scale;
            float dstX = x + (w - scaledW) / 2;
            float dstY = y + (h - scaledH) / 2;
            dstRect = SkRect::MakeXYWH(dstX, dstY, scaledW, scaledH);
            break;
        }
        case ImageScaleMode::Fill: {
            // 保持比例填充容器（可能裁剪）
            float scale = std::max(w / imgW, h / imgH);
            float scaledW = imgW * scale;
            float scaledH = imgH * scale;
            
            // 计算裁剪区域
            float srcX = 0, srcY = 0, srcW = imgW, srcH = imgH;
            if (scaledW > w) {
                srcW = w / scale;
                srcX = (imgW - srcW) / 2;
            }
            if (scaledH > h) {
                srcH = h / scale;
                srcY = (imgH - srcH) / 2;
            }
            srcRect = SkRect::MakeXYWH(srcX, srcY, srcW, srcH);
            dstRect = SkRect::MakeXYWH(x, y, w, h);
            break;
        }
        case ImageScaleMode::Stretch: {
            // 拉伸填充容器
            dstRect = SkRect::MakeXYWH(x, y, w, h);
            break;
        }
    }

    // 应用圆角裁剪
    float radius = m_borderRadius.left.value;
    if (radius > 0) {
        canvas->save();
        SkRRect clipRRect = SkRRect::MakeRectXY(
            SkRect::MakeXYWH(x, y, w, h), radius, radius);
        canvas->clipRRect(clipRRect, true);
    }

    // 绘制图片
    SkSamplingOptions sampling(SkFilterMode::kLinear, SkMipmapMode::kLinear);
    canvas->drawImageRect(m_image, srcRect, dstRect, sampling, nullptr, 
                          SkCanvas::kStrict_SrcRectConstraint);

    if (radius > 0) {
        canvas->restore();
    }
}

void LiteImage::drawPlaceholder(SkCanvas* canvas, float x, float y, float w, float h) {
    SkPaint paint;
    paint.setColor(m_placeholderColor.toARGB());
    paint.setStyle(SkPaint::kFill_Style);
    paint.setAntiAlias(true);

    float radius = m_borderRadius.left.value;
    if (radius > 0) {
        SkRRect rrect = SkRRect::MakeRectXY(SkRect::MakeXYWH(x, y, w, h), radius, radius);
        canvas->drawRRect(rrect, paint);
    } else {
        canvas->drawRect(SkRect::MakeXYWH(x, y, w, h), paint);
    }

    // 绘制简单的图片图标占位符
    if (w > 40 && h > 40) {
        SkPaint iconPaint;
        iconPaint.setColor(Color::Gray().toARGB());
        iconPaint.setStyle(SkPaint::kStroke_Style);
        iconPaint.setStrokeWidth(2.0f);
        iconPaint.setAntiAlias(true);

        float iconSize = std::min(w, h) * 0.3f;
        float iconX = x + (w - iconSize) / 2;
        float iconY = y + (h - iconSize) / 2;

        // 绘制简单的图片图标（矩形框）
        canvas->drawRect(SkRect::MakeXYWH(iconX, iconY, iconSize, iconSize), iconPaint);
        
        // 绘制对角线表示图片
        canvas->drawLine(iconX, iconY, iconX + iconSize, iconY + iconSize, iconPaint);
        canvas->drawLine(iconX + iconSize, iconY, iconX, iconY + iconSize, iconPaint);

        // 绘制太阳（圆形）
        iconPaint.setStyle(SkPaint::kFill_Style);
        canvas->drawCircle(iconX + iconSize * 0.75f, iconY + iconSize * 0.25f,
                          iconSize * 0.1f, iconPaint);
    }
}

} // namespace liteDui
