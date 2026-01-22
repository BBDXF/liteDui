/**
 * LiteSkiaRenderer.cpp - Skia渲染器实现 (OpenGL 后端)
 */

#include "lite_skia_renderer.h"
#include <GLFW/glfw3.h>
#include "include/gpu/ganesh/GrBackendSurface.h"
#include "include/gpu/ganesh/gl/GrGLInterface.h"
#include "include/gpu/ganesh/gl/GrGLDirectContext.h"
#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/core/SkColorSpace.h"
#include <iostream>

namespace liteDui {

LiteSkiaRenderer::LiteSkiaRenderer(void* windowId, int width, int height)
    : m_windowId(windowId), m_width(width), m_height(height) {
    initSkia();
}

LiteSkiaRenderer::~LiteSkiaRenderer() {
    cleanupSkia();
}

void LiteSkiaRenderer::resize(int width, int height) {
    if (m_width == width && m_height == height) return;
    m_width = width;
    m_height = height;
    
    if (!m_context) return;

    m_surface.reset();
    
    GrGLFramebufferInfo fbInfo;
    fbInfo.fFBOID = 0;
    fbInfo.fFormat = 0x8058; // GL_RGBA8

    GrBackendRenderTarget backendRT = GrBackendRenderTargets::MakeGL(m_width, m_height, 0, 8, fbInfo);

    m_surface = SkSurfaces::WrapBackendRenderTarget(
        m_context.get(), backendRT, kBottomLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType, nullptr, nullptr);
}

void LiteSkiaRenderer::initSkia() {
    auto interface = GrGLMakeNativeInterface();
    if (!interface) {
        std::cerr << "Failed to create native GL interface" << std::endl;
        return;
    }
    
    m_context = GrDirectContexts::MakeGL(interface);
    if (!m_context) {
        std::cerr << "Failed to create GrDirectContext" << std::endl;
        return;
    }

    GrGLFramebufferInfo fbInfo;
    fbInfo.fFBOID = 0;
    fbInfo.fFormat = 0x8058; // GL_RGBA8

    GrBackendRenderTarget backendRT = GrBackendRenderTargets::MakeGL(m_width, m_height, 0, 8, fbInfo);

    m_surface = SkSurfaces::WrapBackendRenderTarget(
        m_context.get(), backendRT, kBottomLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType, nullptr, nullptr);

    if (!m_surface) {
        std::cerr << "Failed to create Skia GPU surface" << std::endl;
    }
}

void LiteSkiaRenderer::cleanupSkia() {
    m_surface.reset();
    m_context.reset();
}

SkCanvas* LiteSkiaRenderer::getCanvas() const {
    return m_surface ? m_surface->getCanvas() : nullptr;
}

void LiteSkiaRenderer::begin() {
    if (m_surface) {
        m_surface->getCanvas()->clear(SK_ColorWHITE);
    }
}

void LiteSkiaRenderer::end() {
    if (m_context) {
        m_context->flush();
    }
    if (m_windowId) {
        glfwSwapBuffers(static_cast<GLFWwindow*>(m_windowId));
    }
}

} // namespace liteDui
