/**
 * 04_gui_demo - 使用 liteDui 库提供的窗口和控件功能
 */

#include "lite_window.h"
#include "lite_layout.h"
#include "lite_container.h"
#include "lite_button.h"
#include "lite_input.h"
#include <iostream>
#include <memory>

using namespace liteDui;

int main() {
    std::cout << "Starting liteDui GUI Demo" << std::endl;

    // 1. 创建窗口管理器
    LiteWindowManager manager;

    // 2. 创建窗口
    auto window = manager.CreateWindow(800, 600, "liteDui GUI Demo");
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        return 1;
    }

    // 3. 创建根容器并设置样式
    auto root = std::make_shared<LiteContainer>();
    root->setWidth(LayoutValue::Percent(100));
    root->setHeight(LayoutValue::Percent(100));
    root->setBackgroundColor(Color::fromRGB(245, 245, 245));
    root->setPadding(EdgeInsets::All(30));
    root->setFlexDirection(FlexDirection::Column);
    root->setGap(20);

    // 4. 标题
    auto title = std::make_shared<LiteContainer>();
    title->setWidth(LayoutValue::Percent(100));
    title->setHeight(50);
    title->setText("liteDui GUI Demo");
    title->setFontSize(24);
    title->setTextColor(Color::fromRGB(33, 33, 33));
    title->setTextAlign(TextAlign::Center);
    title->setBackgroundColor(Color::White());
    title->setBorderRadius(EdgeInsets::All(8));
    root->addChild(title);

    // 5. 输入区域
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

    auto nameInput = std::make_shared<LiteInput>("Enter username...");
    nameInput->setWidth(LayoutValue::Percent(100));
    nameInput->setHeight(40);
    inputArea->addChild(nameInput);

    auto pwdLabel = std::make_shared<LiteContainer>();
    pwdLabel->setText("Password:");
    pwdLabel->setHeight(20);
    pwdLabel->setBackgroundColor(Color::Transparent());
    inputArea->addChild(pwdLabel);

    auto passwordInput = std::make_shared<LiteInput>("Enter password...");
    passwordInput->setWidth(LayoutValue::Percent(100));
    passwordInput->setHeight(40);
    passwordInput->setInputType(InputType::Password);
    inputArea->addChild(passwordInput);

    root->addChild(inputArea);

    // 6. 状态标签 (提前创建以便在按钮回调中使用)
    auto statusLabel = std::make_shared<LiteContainer>();
    statusLabel->setWidth(LayoutValue::Percent(100));
    statusLabel->setHeight(30);
    statusLabel->setTextAlign(TextAlign::Center);
    statusLabel->setFontSize(16);
    statusLabel->setBackgroundColor(Color::Transparent());

    // 7. 按钮区域
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
    loginBtn->setOnClick([nameInput, passwordInput, statusLabel](const MouseEvent& e) {
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
    clearBtn->setOnClick([nameInput, passwordInput, statusLabel](const MouseEvent& e) {
        nameInput->clear();
        passwordInput->clear();
        statusLabel->setText("");
    });
    btnArea->addChild(clearBtn);

    root->addChild(btnArea);
    root->addChild(statusLabel);

    // 8. 设置窗口根容器
    window->SetRootContainer(root);

    std::cout << "GUI created. Click inputs to focus, type to enter text." << std::endl;

    // 9. 运行主循环
    manager.Run();

    std::cout << "Program finished" << std::endl;
    return 0;
}
