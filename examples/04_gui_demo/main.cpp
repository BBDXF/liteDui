/**
 * 04_gui_demo - 完整的窗口+GUI控件+交互演示
 */

#include "lite_window.h"
#include "lite_layout.h"
#include "lite_container.h"
#include "lite_button.h"
#include "lite_input.h"

#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>

using namespace liteDui;

// 应用程序状态管理类
class GUIDemoApp {
public:
    GUIDemoApp(int w = 800, int h = 600) : width(w), height(h) {
        bitmap.allocPixels(SkImageInfo::MakeN32Premul(width, height));
    }

    // 初始化 GUI
    void initializeGUI() {
        createGUI();
    }

    // 渲染到屏幕
    void renderToScreen(GLFWwindow* window) {
        SkCanvas canvas(bitmap);
        canvas.clear(SK_ColorWHITE);

        if (root) {
            root->renderTree(&canvas);
        }

        // 使用 OpenGL 绘制位图
        glClear(GL_COLOR_BUFFER_BIT);
        glRasterPos2f(-1, 1);
        glPixelZoom(1, -1);
        glDrawPixels(width, height, GL_BGRA, GL_UNSIGNED_BYTE, bitmap.getPixels());

        glfwSwapBuffers(window);
    }

    // 鼠标事件处理
    void handleMouseEvent(GLFWwindow* window, int button, int action, int mods) {
        if (button != GLFW_MOUSE_BUTTON_LEFT) return;

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        MouseEvent event;
        event.x = static_cast<float>(xpos);
        event.y = static_cast<float>(ypos);
        event.button = MouseButton::Left;
        event.pressed = (action == GLFW_PRESS);
        event.released = (action == GLFW_RELEASE);

        LiteContainer* hit = hitTestContainer(root.get(), event.x, event.y);

        if (action == GLFW_PRESS) {
            // 处理输入框焦点
            if (auto input = dynamic_cast<LiteInput*>(hit)) {
                focusedInput = input;
                input->onMousePressed(event);
            } else if (auto btn = dynamic_cast<LiteButton*>(hit)) {
                btn->onMousePressed(event);
            } else {
                focusedInput = nullptr;
            }
        } else if (action == GLFW_RELEASE) {
            if (auto btn = dynamic_cast<LiteButton*>(hit)) {
                btn->onMouseReleased(event);
            }
        }
    }

    // 键盘事件处理
    void handleKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (action != GLFW_PRESS && action != GLFW_REPEAT) return;

        if (auto input = dynamic_cast<LiteInput*>(focusedInput)) {
            KeyEvent event;
            event.keyCode = key;
            event.pressed = true;
            event.mods = mods;
            event.codepoint = 0;
            input->onKeyPressed(event);
        }
    }

    // 字符输入事件处理
    void handleCharEvent(GLFWwindow* window, unsigned int codepoint) {
        if (auto input = dynamic_cast<LiteInput*>(focusedInput)) {
            KeyEvent event;
            event.keyCode = 0;
            event.pressed = true;
            event.mods = 0;
            event.codepoint = codepoint;
            input->onKeyPressed(event);
        }
    }

    // 窗口大小改变处理
    void handleResize(int newWidth, int newHeight) {
        width = newWidth;
        height = newHeight;
        bitmap.allocPixels(SkImageInfo::MakeN32Premul(width, height));

        if (root) {
            root->setWidth(width);
            root->setHeight(height);
            root->calculateLayout(width, height);
        }

        glViewport(0, 0, width, height);
    }

private:
    // 查找点击位置的控件
    LiteContainer* hitTestContainer(LiteLayout* node, float x, float y) {
        if (!node) return nullptr;

        float nx = node->getLeft();
        float ny = node->getTop();
        float nw = node->getLayoutWidth();
        float nh = node->getLayoutHeight();

        if (x >= nx && x < nx + nw && y >= ny && y < ny + nh) {
            for (size_t i = 0; i < node->getChildCount(); ++i) {
                auto child = node->getChildAt(i);
                LiteContainer* hit = hitTestContainer(child.get(), x, y);
                if (hit) return hit;
            }
            return dynamic_cast<LiteContainer*>(node);
        }
        return nullptr;
    }

    // 创建 GUI 布局
    void createGUI() {
        root = std::make_shared<LiteContainer>();
        root->setWidth(width);
        root->setHeight(height);
        root->setBackgroundColor(Color::fromRGB(245, 245, 245));
        root->setPadding(EdgeInsets::All(30));
        root->setFlexDirection(FlexDirection::Column);
        root->setGap(20);

        // 标题
        auto title = createTitle();
        root->addChild(title);

        // 输入区域
        auto inputArea = createInputArea();
        root->addChild(inputArea);

        // 按钮区域
        auto buttonArea = createButtonArea();
        root->addChild(buttonArea);

        // 状态标签
        statusLabel = createStatusLabel();
        root->addChild(statusLabel);

        root->calculateLayout(width, height);
    }

    std::shared_ptr<LiteContainer> createTitle() {
        auto title = std::make_shared<LiteContainer>();
        title->setWidth(LayoutValue::Percent(100));
        title->setHeight(50);
        title->setText("liteDui GUI Demo");
        title->setFontSize(24);
        title->setTextColor(Color::fromRGB(33, 33, 33));
        title->setTextAlign(TextAlign::Center);
        title->setBackgroundColor(Color::White());
        title->setBorderRadius(EdgeInsets::All(8));
        return title;
    }

    std::shared_ptr<LiteContainer> createInputArea() {
        auto inputArea = std::make_shared<LiteContainer>();
        inputArea->setWidth(LayoutValue::Percent(100));
        inputArea->setFlexDirection(FlexDirection::Column);
        inputArea->setGap(10);
        inputArea->setBackgroundColor(Color::White());
        inputArea->setBorderRadius(EdgeInsets::All(8));
        inputArea->setPadding(EdgeInsets::All(20));

        auto nameLabel = std::make_shared<LiteContainer>();
        nameLabel->setText("Username:");
        nameLabel->setHeight(20);
        nameLabel->setBackgroundColor(Color::Transparent());
        inputArea->addChild(nameLabel);

        nameInput = std::make_shared<LiteInput>("Enter username...");
        nameInput->setWidth(LayoutValue::Percent(100));
        nameInput->setHeight(40);
        inputArea->addChild(nameInput);

        auto pwdLabel = std::make_shared<LiteContainer>();
        pwdLabel->setText("Password:");
        pwdLabel->setHeight(20);
        pwdLabel->setBackgroundColor(Color::Transparent());
        inputArea->addChild(pwdLabel);

        passwordInput = std::make_shared<LiteInput>("Enter password...");
        passwordInput->setWidth(LayoutValue::Percent(100));
        passwordInput->setHeight(40);
        passwordInput->setInputType(InputType::Password);
        inputArea->addChild(passwordInput);

        return inputArea;
    }

    std::shared_ptr<LiteContainer> createButtonArea() {
        auto btnArea = std::make_shared<LiteContainer>();
        btnArea->setWidth(LayoutValue::Percent(100));
        btnArea->setHeight(50);
        btnArea->setFlexDirection(FlexDirection::Row);
        btnArea->setJustifyContent(Align::Center);
        btnArea->setGap(20);
        btnArea->setBackgroundColor(Color::Transparent());

        auto loginBtn = std::make_shared<LiteButton>("Login");
        loginBtn->setWidth(120);
        loginBtn->setHeight(40);
        loginBtn->setNormalBackgroundColor(Color::fromRGB(66, 133, 244));
        loginBtn->setHoverBackgroundColor(Color::fromRGB(53, 122, 232));
        loginBtn->setPressedBackgroundColor(Color::fromRGB(40, 100, 200));
        loginBtn->setNormalTextColor(Color::White());
        loginBtn->setHoverTextColor(Color::White());
        loginBtn->setPressedTextColor(Color::White());
        loginBtn->setOnClick([this](const MouseEvent& e) {
            std::string name = nameInput->getValue();
            std::string pwd = passwordInput->getValue();
            if (!name.empty() && !pwd.empty()) {
                statusLabel->setText("Welcome, " + name + "!");
                statusLabel->setTextColor(Color::fromRGB(76, 175, 80));
            } else {
                statusLabel->setText("Please fill all fields");
                statusLabel->setTextColor(Color::fromRGB(244, 67, 54));
            }
        });
        btnArea->addChild(loginBtn);

        auto clearBtn = std::make_shared<LiteButton>("Clear");
        clearBtn->setWidth(120);
        clearBtn->setHeight(40);
        clearBtn->setOnClick([this](const MouseEvent& e) {
            nameInput->clear();
            passwordInput->clear();
            statusLabel->setText("");
        });
        btnArea->addChild(clearBtn);

        return btnArea;
    }

    std::shared_ptr<LiteContainer> createStatusLabel() {
        auto label = std::make_shared<LiteContainer>();
        label->setWidth(LayoutValue::Percent(100));
        label->setHeight(30);
        label->setTextAlign(TextAlign::Center);
        label->setFontSize(16);
        label->setBackgroundColor(Color::Transparent());
        return label;
    }

public:
    std::shared_ptr<LiteContainer> root;
    std::shared_ptr<LiteInput> nameInput;
    std::shared_ptr<LiteInput> passwordInput;
    std::shared_ptr<LiteContainer> statusLabel;
    LiteContainer* focusedInput = nullptr;
    int width, height;
    SkBitmap bitmap;
};

// 全局应用实例
static GUIDemoApp* g_app = nullptr;

// 回调函数包装器
void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
    if (g_app) g_app->handleMouseEvent(window, button, action, mods);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (g_app) g_app->handleKeyEvent(window, key, scancode, action, mods);
}

void charCallback(GLFWwindow* window, unsigned int codepoint) {
    if (g_app) g_app->handleCharEvent(window, codepoint);
}

void resizeCallback(GLFWwindow* window, int width, int height) {
    if (g_app) g_app->handleResize(width, height);
}

void renderCallback(GLFWwindow* window) {
    if (g_app) g_app->renderToScreen(window);
}

int main() {
    std::cout << "Starting liteDui GUI Demo" << std::endl;

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }

    // 创建应用实例
    g_app = new GUIDemoApp();

    GLFWwindow* window = glfwCreateWindow(g_app->width, g_app->height, "liteDui GUI Demo", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        delete g_app;
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // 设置回调
    glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCharCallback(window, charCallback);
    glfwSetFramebufferSizeCallback(window, resizeCallback);

    // 初始化 GUI
    g_app->initializeGUI();

    std::cout << "GUI created. Click inputs to focus, type to enter text." << std::endl;

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        renderCallback(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    delete g_app;

    std::cout << "Program finished" << std::endl;
    return 0;
}
