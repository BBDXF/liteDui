/**
 * LiteWindow.cpp - 使用简化Cairo渲染器的窗口实现
 */

#include "lite_window.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <chrono>

// 为了使用glfwGetX11Window函数，需要包含GLFW的头文件
#ifdef __linux__
#include <X11/Xlib.h>
#define GLFW_EXPOSE_NATIVE_X11
#endif
#include <GLFW/glfw3native.h>

// LiteWindow类的实现

namespace liteDui
{

LiteWindow::LiteWindow(int width, int height, const char *title, LiteWindowManager *manager)
    : width_(width), height_(height), title_(title), manager_(manager), window_(nullptr)
{
    // 注意：CairoRenderer需要在窗口创建后才能初始化
}

LiteWindow::~LiteWindow()
{
    if (window_)
    {
        glfwDestroyWindow(window_);
    }
}

bool LiteWindow::Initialize()
{
    // 设置窗口提示，不创建OpenGL上下文
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // 创建窗口
    window_ = glfwCreateWindow(width_, height_, title_, nullptr, nullptr);
    if (!window_)
    {
        std::cerr << "Failed to create GLFW window: " << title_ << std::endl;
        return false;
    }

    // 设置窗口回调
    glfwSetWindowUserPointer(window_, this);
    glfwSetWindowSizeCallback(window_, WindowSizeCallback);
    glfwSetWindowCloseCallback(window_, WindowCloseCallback);

    // 获取窗口ID并创建简化的Cairo渲染器
    // 注意：这里暂时注释掉Cairo渲染器，因为还没有实现
    // void *windowId = getWindowId();
    // if (windowId)
    // {
    //     cairoRenderer_ = std::make_unique<liteDui::LiteCairoRenderer>(windowId, width_, height_);
    //     std::cout << "Created simplified Cairo renderer for window: " << title_ << std::endl;
    // }
    // else
    // {
    //     std::cerr << "Failed to get window ID for: " << title_ << std::endl;
    //     return false;
    // }

    std::cout << "Created window: " << title_ << " (" << width_ << "x" << height_ << ")" << std::endl;
    return true;
}

void LiteWindow::Render()
{
    if (!window_ || glfwWindowShouldClose(window_))
        return;

    // 如果有根容器，使用双缓冲Cairo渲染
    // 暂时注释掉，因为没有实现CairoRenderer
    // if (rootContainer_ && cairoRenderer_)
    // {
    //     cairoRenderer_->begin();
    //     if (cairoRenderer_->getContext())
    //     {
    //         rootContainer_->renderTree(cairoRenderer_->getContext());
    //     }
    //     cairoRenderer_->end();
    // }
}

bool LiteWindow::ShouldClose() const
{
    return window_ ? glfwWindowShouldClose(window_) : true;
}

void *LiteWindow::GetWindow() const
{
    return window_;
}

const char *LiteWindow::GetTitle() const
{
    return title_;
}

void *LiteWindow::getWindowId()
{
    if (!window_)
        return nullptr;

    // 根据平台返回相应的窗口ID
#ifdef _WIN32
    return glfwGetWin32Window(window_);
#elif defined(__linux__)
    return (void *)glfwGetX11Window(window_);
#elif defined(__APPLE__)
    return glfwGetCocoaWindow(window_);
#else
    std::cerr << "Platform not supported for direct Cairo rendering" << std::endl;
    return nullptr;
#endif
}

// 窗口大小回调
void LiteWindow::WindowSizeCallback(GLFWwindow *window, int width, int height)
{
    auto win = static_cast<LiteWindow *>(glfwGetWindowUserPointer(window));
    if (win)
    {
        win->width_ = width;
        win->height_ = height;

        // 调整简化的Cairo渲染器大小
        // if (win->cairoRenderer_)
        // {
        //     win->cairoRenderer_->resize(width, height);
        // }

        std::cout << "Window resized: " << win->title_ << " -> " << width << "x" << height << std::endl;
    }
}

// 窗口关闭回调
void LiteWindow::WindowCloseCallback(GLFWwindow *window)
{
    auto win = static_cast<LiteWindow *>(glfwGetWindowUserPointer(window));
    if (win)
    {
        std::cout << "Window close requested: " << win->title_ << std::endl;
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

// WindowManager类的实现

LiteWindowManager::LiteWindowManager() : glfw_initialized_(false)
{
}

LiteWindowManager::~LiteWindowManager()
{
    // 清理所有窗口
    windows_.clear();
    if (glfw_initialized_)
    {
        glfwTerminate();
    }
}

// 创建新窗口
std::shared_ptr<LiteWindow> LiteWindowManager::CreateWindow(int width, int height, const char *title)
{
    // 初始化GLFW（如果还没有初始化）
    if (!glfw_initialized_)
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return nullptr;
        }
        glfw_initialized_ = true;
    }

    auto window = std::make_shared<LiteWindow>(width, height, title, this);
    if (window->Initialize())
    {
        windows_.push_back(window);
        return window;
    }
    return nullptr;
}

// 移除关闭的窗口
void LiteWindowManager::RemoveClosedWindows()
{
    auto it = windows_.begin();
    while (it != windows_.end())
    {
        if ((*it)->ShouldClose())
        {
            std::cout << "LiteWindowManager: Removing closed window: " << (*it)->GetTitle() << std::endl;
            it = windows_.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

// 运行所有窗口
void LiteWindowManager::Run()
{
    if (windows_.empty())
    {
        std::cout << "No windows to run" << std::endl;
        return;
    }

    std::cout << "Created " << windows_.size() << " windows." << std::endl;
    std::cout << "Each window can be closed independently. Program exits when all windows are closed." << std::endl;

    while (!windows_.empty())
    {
        // 渲染所有打开的窗口
        for (auto &window : windows_)
        {
            window->Render();
        }

        // 移除关闭的窗口
        RemoveClosedWindows();

        // 处理事件
        glfwPollEvents();

        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }

    std::cout << "All windows closed. Exiting program." << std::endl;
}

// 获取窗口数量
size_t LiteWindowManager::GetWindowCount() const
{
    return windows_.size();
}

} // namespace liteDui