/**
 * 04_gui_demo - 使用 liteDui 库提供的窗口和控件功能
 * 展示所有控件：Button, Input, Image, Checkbox, RadioButton, List, ScrollView
 */

#include "lite_window.h"
#include "lite_layout.h"
#include "lite_container.h"
#include "lite_button.h"
#include "lite_input.h"
#include "lite_image.h"
#include "lite_checkbox.h"
#include "lite_radio_button.h"
#include "lite_list.h"
#include "lite_scroll_view.h"
#include <iostream>
#include <memory>

using namespace liteDui;

int main() {
    std::cout << "Starting liteDui GUI Demo - All Controls" << std::endl;

    // 1. 创建窗口管理器
    LiteWindowManager manager;

    // 2. 创建窗口
    auto window = manager.CreateWindow(900, 700, "liteDui GUI Demo - All Controls");
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        return 1;
    }

    // 3. 创建根容器并设置样式
    auto root = std::make_shared<LiteContainer>();
    root->setWidth(LayoutValue::Percent(100));
    root->setHeight(LayoutValue::Percent(100));
    root->setBackgroundColor(Color::fromRGB(245, 245, 245));
    root->setPadding(EdgeInsets::All(20));
    root->setFlexDirection(FlexDirection::Column);
    root->setGap(15);

    // 4. 标题
    auto title = std::make_shared<LiteContainer>();
    title->setWidth(LayoutValue::Percent(100));
    title->setHeight(40);
    title->setText("liteDui GUI Demo - All Controls");
    title->setFontSize(22);
    title->setTextColor(Color::fromRGB(33, 33, 33));
    title->setTextAlign(TextAlign::Center);
    title->setBackgroundColor(Color::White());
    title->setBorderRadius(EdgeInsets::All(8));
    root->addChild(title);

    // 5. 主内容区域 - 左右两栏布局
    auto mainContent = std::make_shared<LiteContainer>();
    mainContent->setWidth(LayoutValue::Percent(100));
    mainContent->setFlexGrow(1);
    mainContent->setFlexDirection(FlexDirection::Row);
    mainContent->setGap(15);
    mainContent->setBackgroundColor(Color::Transparent());

    // ==================== 左侧栏 ====================
    auto leftColumn = std::make_shared<LiteContainer>();
    leftColumn->setWidth(LayoutValue::Percent(50));
    leftColumn->setFlexDirection(FlexDirection::Column);
    leftColumn->setGap(15);
    leftColumn->setBackgroundColor(Color::Transparent());

    // --- 图片控件区域 ---
    auto imageSection = std::make_shared<LiteContainer>();
    imageSection->setWidth(LayoutValue::Percent(100));
    imageSection->setBackgroundColor(Color::White());
    imageSection->setBorderRadius(EdgeInsets::All(8));
    imageSection->setPadding(EdgeInsets::All(15));
    imageSection->setFlexDirection(FlexDirection::Column);
    imageSection->setGap(10);

    auto imageSectionTitle = std::make_shared<LiteContainer>();
    imageSectionTitle->setText("Image Control");
    imageSectionTitle->setFontSize(16);
    imageSectionTitle->setTextColor(Color::fromRGB(66, 133, 244));
    imageSectionTitle->setHeight(25);
    imageSectionTitle->setBackgroundColor(Color::Transparent());
    imageSection->addChild(imageSectionTitle);

    auto imageDemo = std::make_shared<LiteImage>();
    imageDemo->setWidth(LayoutValue::Percent(100));
    imageDemo->setHeight(120);
    imageDemo->setScaleMode(ImageScaleMode::Fit);
    imageDemo->setPlaceholderColor(Color::fromRGB(230, 230, 230));
    imageDemo->setBorderRadius(EdgeInsets::All(4));
    // 如果有图片文件可以设置路径: imageDemo->setImagePath("path/to/image.png");
    imageSection->addChild(imageDemo);

    leftColumn->addChild(imageSection);

    // --- 输入框区域 ---
    auto inputSection = std::make_shared<LiteContainer>();
    inputSection->setWidth(LayoutValue::Percent(100));
    inputSection->setBackgroundColor(Color::White());
    inputSection->setBorderRadius(EdgeInsets::All(8));
    inputSection->setPadding(EdgeInsets::All(15));
    inputSection->setFlexDirection(FlexDirection::Column);
    inputSection->setGap(8);

    auto inputSectionTitle = std::make_shared<LiteContainer>();
    inputSectionTitle->setText("Input Controls");
    inputSectionTitle->setFontSize(16);
    inputSectionTitle->setTextColor(Color::fromRGB(66, 133, 244));
    inputSectionTitle->setHeight(25);
    inputSectionTitle->setBackgroundColor(Color::Transparent());
    inputSection->addChild(inputSectionTitle);

    auto nameLabel = std::make_shared<LiteContainer>();
    nameLabel->setText("Username:");
    nameLabel->setHeight(20);
    nameLabel->setBackgroundColor(Color::Transparent());
    inputSection->addChild(nameLabel);

    auto nameInput = std::make_shared<LiteInput>("Enter username...");
    nameInput->setWidth(LayoutValue::Percent(100));
    nameInput->setHeight(36);
    inputSection->addChild(nameInput);

    auto pwdLabel = std::make_shared<LiteContainer>();
    pwdLabel->setText("Password:");
    pwdLabel->setHeight(20);
    pwdLabel->setBackgroundColor(Color::Transparent());
    inputSection->addChild(pwdLabel);

    auto passwordInput = std::make_shared<LiteInput>("Enter password...");
    passwordInput->setWidth(LayoutValue::Percent(100));
    passwordInput->setHeight(36);
    passwordInput->setInputType(InputType::Password);
    inputSection->addChild(passwordInput);

    leftColumn->addChild(inputSection);

    // --- Checkbox 区域 ---
    auto checkboxSection = std::make_shared<LiteContainer>();
    checkboxSection->setWidth(LayoutValue::Percent(100));
    checkboxSection->setBackgroundColor(Color::White());
    checkboxSection->setBorderRadius(EdgeInsets::All(8));
    checkboxSection->setPadding(EdgeInsets::All(15));
    checkboxSection->setFlexDirection(FlexDirection::Column);
    checkboxSection->setGap(8);

    auto checkboxSectionTitle = std::make_shared<LiteContainer>();
    checkboxSectionTitle->setText("Checkbox Controls");
    checkboxSectionTitle->setFontSize(16);
    checkboxSectionTitle->setTextColor(Color::fromRGB(66, 133, 244));
    checkboxSectionTitle->setHeight(25);
    checkboxSectionTitle->setBackgroundColor(Color::Transparent());
    checkboxSection->addChild(checkboxSectionTitle);

    auto rememberCheckbox = std::make_shared<LiteCheckbox>("Remember me");
    rememberCheckbox->setWidth(LayoutValue::Percent(100));
    rememberCheckbox->setHeight(30);
    rememberCheckbox->setOnChanged([](bool checked) {
        std::cout << "Remember me: " << (checked ? "checked" : "unchecked") << std::endl;
    });
    checkboxSection->addChild(rememberCheckbox);

    auto autoLoginCheckbox = std::make_shared<LiteCheckbox>("Auto login");
    autoLoginCheckbox->setWidth(LayoutValue::Percent(100));
    autoLoginCheckbox->setHeight(30);
    autoLoginCheckbox->setOnChanged([](bool checked) {
        std::cout << "Auto login: " << (checked ? "checked" : "unchecked") << std::endl;
    });
    checkboxSection->addChild(autoLoginCheckbox);

    auto notificationsCheckbox = std::make_shared<LiteCheckbox>("Enable notifications");
    notificationsCheckbox->setWidth(LayoutValue::Percent(100));
    notificationsCheckbox->setHeight(30);
    notificationsCheckbox->setChecked(true);  // 默认选中
    checkboxSection->addChild(notificationsCheckbox);

    leftColumn->addChild(checkboxSection);

    mainContent->addChild(leftColumn);

    // ==================== 右侧栏 ====================
    auto rightColumn = std::make_shared<LiteContainer>();
    rightColumn->setWidth(LayoutValue::Percent(50));
    rightColumn->setFlexDirection(FlexDirection::Column);
    rightColumn->setGap(15);
    rightColumn->setBackgroundColor(Color::Transparent());

    // --- RadioButton 区域 ---
    auto radioSection = std::make_shared<LiteContainer>();
    radioSection->setWidth(LayoutValue::Percent(100));
    radioSection->setBackgroundColor(Color::White());
    radioSection->setBorderRadius(EdgeInsets::All(8));
    radioSection->setPadding(EdgeInsets::All(15));
    radioSection->setFlexDirection(FlexDirection::Column);
    radioSection->setGap(8);

    auto radioSectionTitle = std::make_shared<LiteContainer>();
    radioSectionTitle->setText("RadioButton Controls");
    radioSectionTitle->setFontSize(16);
    radioSectionTitle->setTextColor(Color::fromRGB(66, 133, 244));
    radioSectionTitle->setHeight(25);
    radioSectionTitle->setBackgroundColor(Color::Transparent());
    radioSection->addChild(radioSectionTitle);

    // 创建单选组
    auto themeGroup = std::make_shared<LiteRadioGroup>();

    auto lightThemeRadio = std::make_shared<LiteRadioButton>("Light Theme");
    lightThemeRadio->setWidth(LayoutValue::Percent(100));
    lightThemeRadio->setHeight(30);
    lightThemeRadio->setGroup(themeGroup.get());
    lightThemeRadio->setSelected(true);  // 默认选中
    radioSection->addChild(lightThemeRadio);

    auto darkThemeRadio = std::make_shared<LiteRadioButton>("Dark Theme");
    darkThemeRadio->setWidth(LayoutValue::Percent(100));
    darkThemeRadio->setHeight(30);
    darkThemeRadio->setGroup(themeGroup.get());
    radioSection->addChild(darkThemeRadio);

    auto systemThemeRadio = std::make_shared<LiteRadioButton>("System Default");
    systemThemeRadio->setWidth(LayoutValue::Percent(100));
    systemThemeRadio->setHeight(30);
    systemThemeRadio->setGroup(themeGroup.get());
    radioSection->addChild(systemThemeRadio);

    themeGroup->setOnSelectionChanged([](int index) {
        const char* themes[] = {"Light", "Dark", "System"};
        std::cout << "Theme changed to: " << themes[index] << std::endl;
    });

    rightColumn->addChild(radioSection);

    // --- List 区域 ---
    auto listSection = std::make_shared<LiteContainer>();
    listSection->setWidth(LayoutValue::Percent(100));
    listSection->setFlexGrow(1);
    listSection->setBackgroundColor(Color::White());
    listSection->setBorderRadius(EdgeInsets::All(8));
    listSection->setPadding(EdgeInsets::All(15));
    listSection->setFlexDirection(FlexDirection::Column);
    listSection->setGap(8);

    auto listSectionTitle = std::make_shared<LiteContainer>();
    listSectionTitle->setText("List Control (Scrollable)");
    listSectionTitle->setFontSize(16);
    listSectionTitle->setTextColor(Color::fromRGB(66, 133, 244));
    listSectionTitle->setHeight(25);
    listSectionTitle->setBackgroundColor(Color::Transparent());
    listSection->addChild(listSectionTitle);

    auto listDemo = std::make_shared<LiteList>();
    listDemo->setWidth(LayoutValue::Percent(100));
    listDemo->setFlexGrow(1);
    listDemo->setSelectionMode(ListSelectionMode::Single);
    listDemo->setShowAlternateRows(true);
    
    // 添加列表项
    listDemo->addItem("Item 1 - First item");
    listDemo->addItem("Item 2 - Second item");
    listDemo->addItem("Item 3 - Third item");
    listDemo->addItem("Item 4 - Fourth item");
    listDemo->addItem("Item 5 - Fifth item");
    listDemo->addItem("Item 6 - Sixth item");
    listDemo->addItem("Item 7 - Seventh item");
    listDemo->addItem("Item 8 - Eighth item");
    listDemo->addItem("Item 9 - Ninth item");
    listDemo->addItem("Item 10 - Tenth item");
    
    listDemo->setOnSelectionChanged([](int index) {
        std::cout << "List selection changed to index: " << index << std::endl;
    });
    
    listDemo->setOnItemClicked([](int index) {
        std::cout << "List item clicked: " << index << std::endl;
    });

    listSection->addChild(listDemo);
    rightColumn->addChild(listSection);

    mainContent->addChild(rightColumn);
    root->addChild(mainContent);

    // 6. 状态标签 (提前创建以便在按钮回调中使用)
    auto statusLabel = std::make_shared<LiteContainer>();
    statusLabel->setWidth(LayoutValue::Percent(100));
    statusLabel->setHeight(30);
    statusLabel->setTextAlign(TextAlign::Center);
    statusLabel->setFontSize(14);
    statusLabel->setBackgroundColor(Color::White());
    statusLabel->setBorderRadius(EdgeInsets::All(4));

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
    loginBtn->setOnClick([nameInput, passwordInput, statusLabel, rememberCheckbox](const MouseEvent& e) {
        std::string name = nameInput->getValue();
        std::string pwd = passwordInput->getValue();
        if (!name.empty() && !pwd.empty()) {
            std::string msg = "Welcome, " + name + "!";
            if (rememberCheckbox->isChecked()) {
                msg += " (Remembered)";
            }
            statusLabel->setText(msg);
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
    clearBtn->setOnClick([nameInput, passwordInput, statusLabel, rememberCheckbox, autoLoginCheckbox](const MouseEvent& e) {
        nameInput->clear();
        passwordInput->clear();
        statusLabel->setText("");
        rememberCheckbox->setChecked(false);
        autoLoginCheckbox->setChecked(false);
    });
    btnArea->addChild(clearBtn);

    auto exitBtn = std::make_shared<LiteButton>("Exit");
    exitBtn->setWidth(120);
    exitBtn->setHeight(40);
    exitBtn->setNormalBackgroundColor(Color::fromRGB(244, 67, 54));
    exitBtn->setHoverBackgroundColor(Color::fromRGB(229, 57, 53));
    exitBtn->setPressedBackgroundColor(Color::fromRGB(198, 40, 40));
    exitBtn->setNormalTextColor(Color::White());
    exitBtn->setHoverTextColor(Color::White());
    exitBtn->setPressedTextColor(Color::White());
    exitBtn->setOnClick([](const MouseEvent& e) {
        std::cout << "Exit button clicked" << std::endl;
        exit(0);
    });
    btnArea->addChild(exitBtn);

    root->addChild(btnArea);
    root->addChild(statusLabel);

    // 8. 设置窗口根容器
    window->SetRootContainer(root);

    std::cout << "GUI created with all controls:" << std::endl;
    std::cout << "  - Image (placeholder shown)" << std::endl;
    std::cout << "  - Input (text and password)" << std::endl;
    std::cout << "  - Checkbox (3 options)" << std::endl;
    std::cout << "  - RadioButton (theme selection)" << std::endl;
    std::cout << "  - List (scrollable, 10 items)" << std::endl;
    std::cout << "  - Buttons (Login, Clear, Exit)" << std::endl;
    std::cout << std::endl;
    std::cout << "Try scrolling the list with mouse wheel!" << std::endl;

    // 9. 运行主循环
    manager.Run();

    std::cout << "Program finished" << std::endl;
    return 0;
}
