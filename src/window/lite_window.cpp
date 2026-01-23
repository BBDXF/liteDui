/**
 * LiteWindow.cpp - 使用Skia渲染器的窗口实现
 */

#include "lite_window.h"
#include "lite_skia_renderer.h"
#include "lite_container.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <chrono>

// 为了使用native相关函数，需要包含GLFW的头文件
#ifdef __linux__
#include <X11/Xlib.h>
#define GLFW_EXPOSE_NATIVE_X11
#endif
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>

namespace liteDui
{

LiteWindow::LiteWindow(int width, int height, const char *title, LiteWindowManager *manager)
    : width_(width), height_(height), title_(title), manager_(manager), window_(nullptr)
{
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
    // 设置窗口提示：启用 OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 创建窗口
    window_ = glfwCreateWindow(width_, height_, title_, nullptr, nullptr);
    if (!window_)
    {
        std::cerr << "Failed to create GLFW window: " << title_ << std::endl;
        return false;
    }

    // 设置窗口回调
    glfwSetWindowUserPointer(window_, this);
    glfwMakeContextCurrent(window_);
    glfwSetWindowSizeCallback(window_, WindowSizeCallback);
    glfwSetWindowCloseCallback(window_, WindowCloseCallback);

    // 设置鼠标和键盘回调
    glfwSetCursorPosCallback(window_, MousePosCallback);
    glfwSetMouseButtonCallback(window_, MouseButtonCallback);
    glfwSetScrollCallback(window_, ScrollCallback);
    glfwSetKeyCallback(window_, KeyCallback);
    glfwSetCharCallback(window_, CharCallback);

    // 创建Skia渲染器，直接传递 GLFWwindow*
    if (window_)
    {
        skiaRenderer_ = std::make_unique<liteDui::LiteSkiaRenderer>(window_, width_, height_);
        std::cout << "Created Skia renderer for window: " << title_ << std::endl;
    }
    else
    {
        std::cerr << "Failed to get window ID for: " << title_ << std::endl;
        return false;
    }

    std::cout << "Created window: " << title_ << " (" << width_ << "x" << height_ << ")" << std::endl;
    return true;
}

void LiteWindow::Render()
{
    if (!window_ || glfwWindowShouldClose(window_))
        return;

    if (rootContainer_ && skiaRenderer_)
    {
        // 先执行更新逻辑（如光标闪烁），这可能会触发 markDirty
        rootContainer_->updateTree();
        
        // 只有在脏状态时才重新渲染
        if (rootContainer_->isDirty())
        {
            // 重新计算布局
            rootContainer_->setWidth(liteDui::LayoutValue::Point(static_cast<float>(width_)));
            rootContainer_->setHeight(liteDui::LayoutValue::Point(static_cast<float>(height_)));
            rootContainer_->calculateLayout(static_cast<float>(width_), static_cast<float>(height_));
            
            // 执行渲染
            skiaRenderer_->begin();
            SkCanvas *canvas = skiaRenderer_->getCanvas();
            if (canvas)
            {
                rootContainer_->renderTree(canvas);
            }
            skiaRenderer_->end();
            
            // 清除整棵树的脏标记
            rootContainer_->clearDirtyTree();
        }
    }
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

void LiteWindow::SetRootContainer(std::shared_ptr<liteDui::LiteContainer> root)
{
    rootContainer_ = root;
    focusedContainer_ = nullptr; // 重置焦点
    if (rootContainer_) {
        rootContainer_->markDirty();
    }
}

std::shared_ptr<liteDui::LiteContainer> LiteWindow::GetRootContainer() const
{
    return rootContainer_;
}

void LiteWindow::SetFocusedContainer(liteDui::LiteContainer* container)
{
    if (focusedContainer_ == container) return;
    
    // 通知旧的焦点控件失去焦点
    if (focusedContainer_) {
        liteDui::MouseEvent event;
        focusedContainer_->onFocusLost();
    }
    
    focusedContainer_ = container;
    
    // 通知新的焦点控件获得焦点
    if (focusedContainer_) {
        focusedContainer_->onFocusGained();
    }
}

void *LiteWindow::getWindowId()
{
    if (!window_)
        return nullptr;

#ifdef _WIN32
    return glfwGetWin32Window(window_);
#elif defined(__linux__)
    return (void *)glfwGetX11Window(window_);
#elif defined(__APPLE__)
    return glfwGetCocoaWindow(window_);
#else
    return nullptr;
#endif
}

void LiteWindow::WindowSizeCallback(GLFWwindow *window, int width, int height)
{
    auto win = static_cast<LiteWindow *>(glfwGetWindowUserPointer(window));
    if (win)
    {
        win->width_ = width;
        win->height_ = height;

        if (win->skiaRenderer_)
        {
            win->skiaRenderer_->resize(width, height);
        }
        if (win->rootContainer_)
        {
            win->rootContainer_->markDirty();
        }
    }
}

void LiteWindow::WindowCloseCallback(GLFWwindow *window)
{
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static liteDui::LiteContainer *g_lastMouseInsideContainer = nullptr;

static liteDui::LiteContainer *findDeepestContainerAtPosition(liteDui::LiteContainer *container, float x, float y, float &subx, float &suby)
{
    if (!container) return nullptr;

    float cx = container->getLeft();
    float cy = container->getTop();
    float cw = container->getLayoutWidth();
    float ch = container->getLayoutHeight();

    if (x < cx || x >= cx + cw || y < cy || y >= cy + ch) return nullptr;

    subx = x - cx;
    suby = y - cy;

    for (int i = static_cast<int>(container->getChildCount()) - 1; i >= 0; --i)
    {
        auto child = std::dynamic_pointer_cast<liteDui::LiteContainer>(container->getChildAt(i));
        if (child)
        {
            liteDui::LiteContainer *deepest = findDeepestContainerAtPosition(child.get(), subx, suby, subx, suby);
            if (deepest) return deepest;
        }
    }

    return container;
}

static void dispatchMouseEvent(LiteWindow *win, liteDui::LiteContainer *rootContainer, liteDui::MouseEvent &event, bool isMoving)
{
    if (!rootContainer) return;

    float subx, suby;
    liteDui::LiteContainer *target = findDeepestContainerAtPosition(rootContainer, event.x, event.y, subx, suby);

    if (target != g_lastMouseInsideContainer)
    {
        if (g_lastMouseInsideContainer) g_lastMouseInsideContainer->onMouseExited(event);
        g_lastMouseInsideContainer = target;
        if (target) target->onMouseEntered(event);
    }

    if (target)
    {
        event.x = subx;
        event.y = suby;
        if (isMoving) {
            target->onMouseMoved(event);
        } else if (event.pressed) {
            // 鼠标按下时，设置焦点到被点击的控件
            if (win) {
                win->SetFocusedContainer(target);
            }
            target->onMousePressed(event);
        } else if (event.released) {
            target->onMouseReleased(event);
        }
    }
    else if (event.pressed && win)
    {
        // 点击空白区域时清除焦点
        win->SetFocusedContainer(nullptr);
    }
}

void LiteWindow::MousePosCallback(GLFWwindow *window, double xpos, double ypos)
{
    auto win = static_cast<LiteWindow *>(glfwGetWindowUserPointer(window));
    if (win && win->rootContainer_)
    {
        liteDui::MouseEvent event(static_cast<float>(xpos), static_cast<float>(ypos));
        dispatchMouseEvent(win, win->rootContainer_.get(), event, true);
    }
}

void LiteWindow::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    auto win = static_cast<LiteWindow *>(glfwGetWindowUserPointer(window));
    if (win && win->rootContainer_)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        liteDui::MouseEvent event(static_cast<float>(xpos), static_cast<float>(ypos), static_cast<liteDui::MouseButton>(button));
        event.pressed = (action == GLFW_PRESS);
        event.released = (action == GLFW_RELEASE);
        dispatchMouseEvent(win, win->rootContainer_.get(), event, false);
    }
}

void LiteWindow::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
}

// 处理用户按键输入 - 发送给焦点控件
void LiteWindow::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    auto win = static_cast<LiteWindow *>(glfwGetWindowUserPointer(window));
    if (win && win->focusedContainer_)
    {
        liteDui::KeyEvent event;
        event.keyCode = key;
        event.mods = mods;
        event.pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
        event.released = (action == GLFW_RELEASE);
        win->focusedContainer_->onKeyPressed(event);
    }
}

// 处理用户输入法等Unicode输入 - 发送给焦点控件
void LiteWindow::CharCallback(GLFWwindow *window, unsigned int codepoint)
{
    auto win = static_cast<LiteWindow *>(glfwGetWindowUserPointer(window));
    if (win && win->focusedContainer_)
    {
        win->focusedContainer_->onCharInput(codepoint);
    }
}

LiteWindowManager::LiteWindowManager() : glfw_initialized_(false) {}
LiteWindowManager::~LiteWindowManager()
{
    windows_.clear();
    if (glfw_initialized_) glfwTerminate();
}

std::shared_ptr<LiteWindow> LiteWindowManager::CreateWindow(int width, int height, const char *title)
{
    if (!glfw_initialized_)
    {
        if (!glfwInit()) return nullptr;
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

void LiteWindowManager::RemoveClosedWindows()
{
    auto it = windows_.begin();
    while (it != windows_.end())
    {
        if ((*it)->ShouldClose()) it = windows_.erase(it);
        else ++it;
    }
}

void LiteWindowManager::Run()
{
    if (windows_.empty()) return;
    while (!windows_.empty())
    {
        for (auto &window : windows_) window->Render();
        RemoveClosedWindows();
        glfwPollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

size_t LiteWindowManager::GetWindowCount() const { return windows_.size(); }

} // namespace liteDui
