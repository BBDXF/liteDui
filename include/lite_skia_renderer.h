/**
 * LiteSkiaRenderer.h - Skia渲染器 (OpenGL 后端)
 */

#ifndef LITE_SKIA_RENDERER_H
#define LITE_SKIA_RENDERER_H

#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include <memory>
#include <string>

namespace liteDui {

/**
 * LiteSkiaRenderer类
 * 
 * 使用 GPU 加速的 Skia 渲染器，通过 OpenGL 与 GLFW 融合
 */
class LiteSkiaRenderer {
public:
    /**
     * 构造函数
     * @param windowId 窗口指针 (GLFWwindow*)
     * @param width 渲染器宽度
     * @param height 渲染器高度
     */
    LiteSkiaRenderer(void* windowId, int width, int height);
    
    /**
     * 析构函数
     */
    ~LiteSkiaRenderer();
    
    /**
     * 禁用拷贝构造和赋值
     */
    LiteSkiaRenderer(const LiteSkiaRenderer&) = delete;
    LiteSkiaRenderer& operator=(const LiteSkiaRenderer&) = delete;
    
    /**
     * 调整渲染器大小
     * @param width 新宽度
     * @param height 新高度
     */
    void resize(int width, int height);
    
    /**
     * 获取Skia画布
     * @return SkCanvas指针
     */
    SkCanvas* getCanvas() const;
    
    /**
     * 开始绘制
     */
    void begin();
    
    /**
     * 结束绘制
     */
    void end();

private:
    void* m_windowId;  // 存储 GLFWwindow* 指针
    int m_width;
    int m_height;
    
    sk_sp<GrDirectContext> m_context;
    sk_sp<SkSurface> m_surface;
    
    /**
     * 初始化Skia GPU上下文和表面
     */
    void initSkia();
    
    /**
     * 清理Skia资源
     */
    void cleanupSkia();
};

} // namespace liteDui

#endif // LITE_SKIA_RENDERER_H
