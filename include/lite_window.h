#ifndef LITE_WINDOW_H
#define LITE_WINDOW_H

#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <cstddef>
#include "lite_common.h"

// 前向声明
namespace liteDui {
    class LiteSkiaRenderer;
    class LiteContainer;
    class LiteWindowManager;
}

/**
 * @class LiteWindow
 * @brief 使用Skia渲染器的窗口类
 */
namespace liteDui {
class LiteWindow {
public:
    /**
     * @brief 构造函数
     * @param width 窗口宽度（像素）
     * @param height 窗口高度（像素）
     * @param title 窗口标题
     * @param manager 关联的窗口管理器指针
     */
    LiteWindow(int width, int height, const char* title, LiteWindowManager* manager);

    /**
     * @brief 析构函数
     */
    ~LiteWindow();

    /**
     * @brief 初始化窗口
     * @return 成功返回true，失败返回false
     */
    bool Initialize();

    /**
     * @brief 渲染窗口
     */
    void Render();

    /**
     * @brief 检查窗口是否应该关闭
     * @return 窗口应该关闭返回true，否则返回false
     */
    bool ShouldClose() const;

    /**
     * @brief 获取GLFW窗口指针
     * @return GLFWwindow指针
     */
    void* GetWindow() const;

    /**
     * @brief 获取窗口标题
     * @return 窗口标题字符串
     */
    const char* GetTitle() const;

    /**
     * @brief 设置根容器
     * @param root 根容器指针
     */
    void SetRootContainer(std::shared_ptr<liteDui::LiteContainer> root);

    /**
     * @brief 获取根容器
     * @return 根容器指针
     */
    std::shared_ptr<liteDui::LiteContainer> GetRootContainer() const;

    // 禁止复制和赋值
    LiteWindow(const LiteWindow&) = delete;
    LiteWindow& operator=(const LiteWindow&) = delete;

private:
    int width_;
    int height_;
    const char* title_;
    LiteWindowManager* manager_;
    GLFWwindow* window_; // GLFWwindow指针
    std::unique_ptr<liteDui::LiteSkiaRenderer> skiaRenderer_; // Skia渲染器
    std::shared_ptr<liteDui::LiteContainer> rootContainer_; // 根容器

    /**
     * @brief 获取平台特定的窗口ID
     * @return 窗口ID（HWND、X11 Window或NSWindow）
     */
    void* getWindowId();

    // 窗口大小回调函数
    static void WindowSizeCallback(GLFWwindow* window, int width, int height);

    // 窗口关闭回调函数
    static void WindowCloseCallback(GLFWwindow* window);

    // 鼠标位置回调函数
    static void MousePosCallback(GLFWwindow* window, double xpos, double ypos);

    // 鼠标按钮回调函数
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    // 鼠标滚轮回调函数
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    // 键盘按键回调函数
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    // 字符输入回调函数
    static void CharCallback(GLFWwindow* window, unsigned int codepoint);

    friend class LiteWindowManager;
};

class LiteWindowManager {
public:
    /**
     * @brief 构造函数
     */
    LiteWindowManager();

    /**
     * @brief 析构函数
     */
    ~LiteWindowManager();

    /**
     * @brief 创建新窗口
     * @param width 窗口宽度（像素）
     * @param height 窗口高度（像素）
     * @param title 窗口标题
     * @return 成功返回LiteWindow共享指针，失败返回nullptr
     */
    std::shared_ptr<LiteWindow> CreateWindow(int width, int height, const char* title);

    /**
     * @brief 移除已关闭的窗口
     */
    void RemoveClosedWindows();

    /**
     * @brief 运行主循环
     */
    void Run();

    /**
     * @brief 获取当前窗口数量
     * @return 当前管理的窗口数量
     */
    size_t GetWindowCount() const;

private:
    std::vector<std::shared_ptr<LiteWindow>> windows_;
    bool glfw_initialized_;

    // 禁止复制和赋值
    LiteWindowManager(const LiteWindowManager&) = delete;
    LiteWindowManager& operator=(const LiteWindowManager&) = delete;
};

} // namespace liteDui

#endif // LITE_WINDOW_H
