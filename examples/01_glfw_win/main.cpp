#include "lite_window.h"
#include <iostream>

int main() {
    std::cout << "Starting liteDui GLFW window example" << std::endl;

    // 创建窗口管理器
    liteDui::LiteWindowManager manager;

    // 创建一个窗口
    auto window = manager.CreateWindow(800, 600, "liteDui GLFW Window");
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        return 1;
    }

    // 运行主循环
    manager.Run();

    std::cout << "Program finished" << std::endl;
    return 0;
}