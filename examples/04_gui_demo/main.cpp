/**
 * 04_gui_demo - 使用 liteDui 库提供的窗口和控件功能
 * 展示所有控件：Button, Input, Image, Checkbox, RadioButton, List, ScrollView,
 *              GroupBox, TabView, Table
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
#include "lite_group_box.h"
#include "lite_tab_view.h"
#include "lite_table.h"
#include <iostream>
#include <memory>

using namespace liteDui;

// 创建登录表单标签页内容
LiteContainerPtr createLoginTab() {
    auto content = std::make_shared<LiteContainer>();
    content->setBackgroundColor(Color::White());
    content->setFlexDirection(FlexDirection::Column);
    content->setPadding(EdgeInsets::All(15));
    content->setGap(10);
    content->setWidth(LayoutValue::Percent(100));
    content->setFlexGrow(1);

    // 用户名输入
    auto nameLabel = std::make_shared<LiteContainer>();
    nameLabel->setText("Username:");
    nameLabel->setHeight(20);
    nameLabel->setBackgroundColor(Color::Transparent());
    content->addChild(nameLabel);

    auto nameInput = std::make_shared<LiteInput>("Enter username...");
    nameInput->setWidth(LayoutValue::Percent(100));
    nameInput->setHeight(36);
    content->addChild(nameInput);

    // 密码输入
    auto pwdLabel = std::make_shared<LiteContainer>();
    pwdLabel->setText("Password:");
    pwdLabel->setHeight(20);
    pwdLabel->setBackgroundColor(Color::Transparent());
    content->addChild(pwdLabel);

    auto passwordInput = std::make_shared<LiteInput>("Enter password...");
    passwordInput->setWidth(LayoutValue::Percent(100));
    passwordInput->setHeight(36);
    passwordInput->setInputType(InputType::Password);
    content->addChild(passwordInput);

    // 选项
    auto rememberCheckbox = std::make_shared<LiteCheckbox>("Remember me");
    rememberCheckbox->setWidth(LayoutValue::Percent(100));
    rememberCheckbox->setHeight(30);
    content->addChild(rememberCheckbox);

    auto autoLoginCheckbox = std::make_shared<LiteCheckbox>("Auto login");
    autoLoginCheckbox->setWidth(LayoutValue::Percent(100));
    autoLoginCheckbox->setHeight(30);
    content->addChild(autoLoginCheckbox);

    return content;
}

// 静态变量保持 RadioGroup 的生命周期
static LiteRadioGroupPtr s_themeRadioGroup;

// 创建设置标签页内容
LiteContainerPtr createSettingsTab() {
    auto content = std::make_shared<LiteContainer>();
    content->setBackgroundColor(Color::White());
    content->setFlexDirection(FlexDirection::Column);
    content->setPadding(EdgeInsets::All(15));
    content->setGap(10);
    content->setWidth(LayoutValue::Percent(100));
    content->setFlexGrow(1);

    // 主题设置 GroupBox
    auto themeGroup = std::make_shared<LiteGroupBox>("Theme Settings");
    themeGroup->setWidth(LayoutValue::Percent(100));
    themeGroup->setHeight(130);
    themeGroup->setFlexDirection(FlexDirection::Column);
    themeGroup->setGap(5);

    // 创建单选组（使用静态变量保持生命周期）
    s_themeRadioGroup = std::make_shared<LiteRadioGroup>();

    auto lightThemeRadio = std::make_shared<LiteRadioButton>("Light Theme");
    lightThemeRadio->setWidth(LayoutValue::Percent(100));
    lightThemeRadio->setHeight(28);
    lightThemeRadio->setGroup(s_themeRadioGroup.get());
    lightThemeRadio->setSelected(true);
    themeGroup->addChild(lightThemeRadio);

    auto darkThemeRadio = std::make_shared<LiteRadioButton>("Dark Theme");
    darkThemeRadio->setWidth(LayoutValue::Percent(100));
    darkThemeRadio->setHeight(28);
    darkThemeRadio->setGroup(s_themeRadioGroup.get());
    themeGroup->addChild(darkThemeRadio);

    auto systemThemeRadio = std::make_shared<LiteRadioButton>("System Default");
    systemThemeRadio->setWidth(LayoutValue::Percent(100));
    systemThemeRadio->setHeight(28);
    systemThemeRadio->setGroup(s_themeRadioGroup.get());
    themeGroup->addChild(systemThemeRadio);

    content->addChild(themeGroup);

    // 通知设置 GroupBox
    auto notifyGroup = std::make_shared<LiteGroupBox>("Notifications");
    notifyGroup->setWidth(LayoutValue::Percent(100));
    notifyGroup->setHeight(100);
    notifyGroup->setFlexDirection(FlexDirection::Column);
    notifyGroup->setGap(5);

    auto emailNotify = std::make_shared<LiteCheckbox>("Email notifications");
    emailNotify->setWidth(LayoutValue::Percent(100));
    emailNotify->setHeight(28);
    emailNotify->setChecked(true);
    notifyGroup->addChild(emailNotify);

    auto pushNotify = std::make_shared<LiteCheckbox>("Push notifications");
    pushNotify->setWidth(LayoutValue::Percent(100));
    pushNotify->setHeight(28);
    notifyGroup->addChild(pushNotify);

    content->addChild(notifyGroup);

    return content;
}

// 创建数据标签页内容（包含表格）
LiteContainerPtr createDataTab() {
    auto content = std::make_shared<LiteContainer>();
    content->setBackgroundColor(Color::White());
    content->setFlexDirection(FlexDirection::Column);
    content->setPadding(EdgeInsets::All(10));
    content->setGap(10);
    content->setWidth(LayoutValue::Percent(100));
    content->setFlexGrow(1);

    auto tableLabel = std::make_shared<LiteContainer>();
    tableLabel->setText("User Data Table:");
    tableLabel->setHeight(25);
    tableLabel->setBackgroundColor(Color::Transparent());
    tableLabel->setFontSize(14);
    content->addChild(tableLabel);

    // 创建表格
    auto table = std::make_shared<LiteTable>();
    table->setWidth(LayoutValue::Percent(100));
    table->setFlexGrow(1);
    
    // 添加列
    table->addColumn("ID", 60, TextAlign::Center);
    table->addColumn("Name", 120, TextAlign::Left);
    table->addColumn("Email", 180, TextAlign::Left);
    table->addColumn("Status", 80, TextAlign::Center);

    // 添加数据行
    table->addRow({"1", "Alice", "alice@example.com", "Active"});
    table->addRow({"2", "Bob", "bob@example.com", "Active"});
    table->addRow({"3", "Charlie", "charlie@example.com", "Inactive"});
    table->addRow({"4", "Diana", "diana@example.com", "Active"});
    table->addRow({"5", "Eve", "eve@example.com", "Pending"});
    table->addRow({"6", "Frank", "frank@example.com", "Active"});
    table->addRow({"7", "Grace", "grace@example.com", "Inactive"});
    table->addRow({"8", "Henry", "henry@example.com", "Active"});

    table->setOnRowClicked([](int row) {
        std::cout << "Table row clicked: " << row << std::endl;
    });

    table->setOnCellClicked([](int row, int col) {
        std::cout << "Table cell clicked: row=" << row << ", col=" << col << std::endl;
    });

    content->addChild(table);

    return content;
}

int main() {
    std::cout << "Starting liteDui GUI Demo - All Controls" << std::endl;

    // 1. 创建窗口管理器
    LiteWindowManager manager;

    // 2. 创建窗口
    auto window = manager.CreateWindow(950, 700, "liteDui GUI Demo - All Controls");
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        return 1;
    }

    // 3. 创建根容器并设置样式
    auto root = std::make_shared<LiteContainer>();
    root->setWidth(LayoutValue::Percent(100));
    root->setHeight(LayoutValue::Percent(100));
    root->setBackgroundColor(Color::fromRGB(240, 240, 240));
    root->setPadding(EdgeInsets::All(15));
    root->setFlexDirection(FlexDirection::Column);
    root->setGap(12);

    // 4. 标题
    auto title = std::make_shared<LiteContainer>();
    title->setWidth(LayoutValue::Percent(100));
    title->setHeight(40);
    title->setText("liteDui GUI Demo - All Controls");
    title->setFontSize(20);
    title->setTextColor(Color::fromRGB(33, 33, 33));
    title->setTextAlign(TextAlign::Center);
    title->setBackgroundColor(Color::White());
    title->setBorderRadius(EdgeInsets::All(6));
    root->addChild(title);

    // 5. 主内容区域 - 左右两栏布局
    auto mainContent = std::make_shared<LiteContainer>();
    mainContent->setWidth(LayoutValue::Percent(100));
    mainContent->setFlexGrow(1);
    mainContent->setFlexDirection(FlexDirection::Row);
    mainContent->setGap(12);
    mainContent->setBackgroundColor(Color::Transparent());

    // ==================== 左侧栏 ====================
    auto leftColumn = std::make_shared<LiteContainer>();
    leftColumn->setWidth(LayoutValue::Percent(45));
    leftColumn->setFlexDirection(FlexDirection::Column);
    leftColumn->setGap(12);
    leftColumn->setBackgroundColor(Color::Transparent());

    // --- 图片控件区域 ---
    auto imageSection = std::make_shared<LiteGroupBox>("Image Control");
    imageSection->setWidth(LayoutValue::Percent(100));
    imageSection->setHeight(140);
    imageSection->setFlexDirection(FlexDirection::Column);

    auto imageDemo = std::make_shared<LiteImage>();
    imageDemo->setWidth(LayoutValue::Percent(100));
    imageDemo->setFlexGrow(1);
    imageDemo->setScaleMode(ImageScaleMode::Fit);
    imageDemo->setPlaceholderColor(Color::fromRGB(220, 220, 220));
    imageDemo->setBorderRadius(EdgeInsets::All(4));
    imageSection->addChild(imageDemo);

    leftColumn->addChild(imageSection);

    // --- TabView 区域 ---
    auto tabView = std::make_shared<LiteTabView>();
    tabView->setWidth(LayoutValue::Percent(100));
    tabView->setFlexGrow(1);
    tabView->setTabHeight(32);

    // 添加标签页
    tabView->addTab("Login", createLoginTab());
    tabView->addTab("Settings", createSettingsTab());
    tabView->addTab("Data", createDataTab());

    tabView->setOnTabChanged([](int index) {
        const char* tabs[] = {"Login", "Settings", "Data"};
        std::cout << "Tab changed to: " << tabs[index] << std::endl;
    });

    leftColumn->addChild(tabView);

    mainContent->addChild(leftColumn);

    // ==================== 右侧栏 ====================
    auto rightColumn = std::make_shared<LiteContainer>();
    rightColumn->setWidth(LayoutValue::Percent(55));
    rightColumn->setFlexDirection(FlexDirection::Column);
    rightColumn->setGap(12);
    rightColumn->setBackgroundColor(Color::Transparent());

    // --- List 区域 ---
    auto listSection = std::make_shared<LiteGroupBox>("Scrollable List");
    listSection->setWidth(LayoutValue::Percent(100));
    listSection->setFlexGrow(1);
    listSection->setFlexDirection(FlexDirection::Column);

    auto listDemo = std::make_shared<LiteList>();
    listDemo->setWidth(LayoutValue::Percent(100));
    listDemo->setFlexGrow(1);
    listDemo->setSelectionMode(ListSelectionMode::Single);
    listDemo->setShowAlternateRows(true);
    
    // 添加列表项
    for (int i = 1; i <= 20; ++i) {
        listDemo->addItem("List Item " + std::to_string(i) + " - Click or scroll to test");
    }
    
    listDemo->setOnSelectionChanged([](int index) {
        std::cout << "List selection changed to index: " << index << std::endl;
    });

    listSection->addChild(listDemo);
    rightColumn->addChild(listSection);

    // --- 独立表格区域 ---
    auto tableSection = std::make_shared<LiteGroupBox>("Data Table");
    tableSection->setWidth(LayoutValue::Percent(100));
    tableSection->setHeight(200);
    tableSection->setFlexDirection(FlexDirection::Column);

    auto table2 = std::make_shared<LiteTable>();
    table2->setWidth(LayoutValue::Percent(100));
    table2->setFlexGrow(1);
    
    table2->addColumn("Product", 150, TextAlign::Left);
    table2->addColumn("Price", 80, TextAlign::Right);
    table2->addColumn("Qty", 60, TextAlign::Center);
    table2->addColumn("Total", 100, TextAlign::Right);

    table2->addRow({"Laptop", "$999", "2", "$1998"});
    table2->addRow({"Mouse", "$29", "5", "$145"});
    table2->addRow({"Keyboard", "$79", "3", "$237"});
    table2->addRow({"Monitor", "$299", "2", "$598"});
    table2->addRow({"Headphones", "$149", "4", "$596"});

    tableSection->addChild(table2);
    rightColumn->addChild(tableSection);

    mainContent->addChild(rightColumn);
    root->addChild(mainContent);

    // 6. 状态标签
    auto statusLabel = std::make_shared<LiteContainer>();
    statusLabel->setWidth(LayoutValue::Percent(100));
    statusLabel->setHeight(28);
    statusLabel->setTextAlign(TextAlign::Center);
    statusLabel->setFontSize(13);
    statusLabel->setBackgroundColor(Color::White());
    statusLabel->setBorderRadius(EdgeInsets::All(4));
    statusLabel->setText("Ready - Click controls to interact");
    statusLabel->setTextColor(Color::fromRGB(100, 100, 100));

    // 7. 按钮区域
    auto btnArea = std::make_shared<LiteContainer>();
    btnArea->setWidth(LayoutValue::Percent(100));
    btnArea->setHeight(45);
    btnArea->setFlexDirection(FlexDirection::Row);
    btnArea->setJustifyContent(Align::Center);
    btnArea->setGap(15);
    btnArea->setBackgroundColor(Color::Transparent());

    auto actionBtn = std::make_shared<LiteButton>("Action");
    actionBtn->setWidth(100);
    actionBtn->setHeight(36);
    actionBtn->setNormalBackgroundColor(Color::fromRGB(66, 133, 244));
    actionBtn->setHoverBackgroundColor(Color::fromRGB(53, 122, 232));
    actionBtn->setPressedBackgroundColor(Color::fromRGB(40, 100, 200));
    actionBtn->setNormalTextColor(Color::White());
    actionBtn->setHoverTextColor(Color::White());
    actionBtn->setPressedTextColor(Color::White());
    actionBtn->setOnClick([statusLabel](const MouseEvent& e) {
        statusLabel->setText("Action button clicked!");
        statusLabel->setTextColor(Color::fromRGB(66, 133, 244));
    });
    btnArea->addChild(actionBtn);

    auto resetBtn = std::make_shared<LiteButton>("Reset");
    resetBtn->setWidth(100);
    resetBtn->setHeight(36);
    resetBtn->setOnClick([statusLabel](const MouseEvent& e) {
        statusLabel->setText("Ready - Click controls to interact");
        statusLabel->setTextColor(Color::fromRGB(100, 100, 100));
    });
    btnArea->addChild(resetBtn);

    auto exitBtn = std::make_shared<LiteButton>("Exit");
    exitBtn->setWidth(100);
    exitBtn->setHeight(36);
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
    std::cout << "  - Checkbox" << std::endl;
    std::cout << "  - RadioButton (with RadioGroup)" << std::endl;
    std::cout << "  - List (scrollable)" << std::endl;
    std::cout << "  - ScrollView" << std::endl;
    std::cout << "  - GroupBox" << std::endl;
    std::cout << "  - TabView (3 tabs)" << std::endl;
    std::cout << "  - Table (with headers and data)" << std::endl;
    std::cout << "  - Buttons" << std::endl;
    std::cout << std::endl;
    std::cout << "Try scrolling lists/tables with mouse wheel!" << std::endl;

    // 9. 运行主循环
    manager.Run();

    std::cout << "Program finished" << std::endl;
    return 0;
}
