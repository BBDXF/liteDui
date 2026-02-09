/**
 * 04_gui_demo - liteDui 控件展示 (模块化重构版)
 * 使用 class 方式组织各个功能模块
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
#include "lite_label.h"
#include "lite_progress_bar.h"
#include "lite_slider.h"
#include "lite_combo_box.h"
#include "lite_tree_view.h"
#include "lite_menu.h"
#include "lite_dialog.h"
#include "lite_message_box.h"
#include <iostream>
#include <memory>

using namespace liteDui;

// ==================== 基础面板类 ====================
class DemoPanel {
public:
    virtual ~DemoPanel() = default;
    virtual LiteContainerPtr create() = 0;
};

// ==================== 登录面板 ====================
class LoginPanel : public DemoPanel {
public:
    LiteContainerPtr create() override {
        auto content = std::make_shared<LiteContainer>();
        content->setBackgroundColor(Color::White());
        content->setFlexDirection(FlexDirection::Column);
        content->setPadding(EdgeInsets::All(15));
        content->setGap(10);
        content->setWidth(LayoutValue::Percent(100));
        content->setFlexGrow(1);

        // 使用 LiteLabel
        auto nameLabel = std::make_shared<LiteLabel>("Username:");
        nameLabel->setHeight(20);
        content->addChild(nameLabel);

        auto nameInput = std::make_shared<LiteInput>("Enter username...");
        nameInput->setWidth(LayoutValue::Percent(100));
        nameInput->setHeight(36);
        content->addChild(nameInput);

        auto pwdLabel = std::make_shared<LiteLabel>("Password:");
        pwdLabel->setHeight(20);
        content->addChild(pwdLabel);

        auto passwordInput = std::make_shared<LiteInput>("Enter password...");
        passwordInput->setWidth(LayoutValue::Percent(100));
        passwordInput->setHeight(36);
        passwordInput->setInputType(InputType::Password);
        content->addChild(passwordInput);

        auto rememberCheckbox = std::make_shared<LiteCheckbox>("Remember me");
        rememberCheckbox->setWidth(LayoutValue::Percent(100));
        rememberCheckbox->setHeight(30);
        content->addChild(rememberCheckbox);

        return content;
    }
};

// ==================== 设置面板 ====================
class SettingsPanel : public DemoPanel {
    LiteRadioGroupPtr themeRadioGroup_;
public:
    LiteContainerPtr create() override {
        auto content = std::make_shared<LiteContainer>();
        content->setBackgroundColor(Color::White());
        content->setFlexDirection(FlexDirection::Column);
        content->setPadding(EdgeInsets::All(15));
        content->setGap(10);
        content->setWidth(LayoutValue::Percent(100));
        content->setFlexGrow(1);

        // 主题设置
        auto themeGroup = std::make_shared<LiteGroupBox>("Theme Settings");
        themeGroup->setWidth(LayoutValue::Percent(100));
        themeGroup->setHeight(110);
        themeGroup->setFlexDirection(FlexDirection::Column);
        themeGroup->setGap(5);

        themeRadioGroup_ = std::make_shared<LiteRadioGroup>();

        auto lightRadio = std::make_shared<LiteRadioButton>("Light Theme");
        lightRadio->setWidth(LayoutValue::Percent(100));
        lightRadio->setHeight(28);
        lightRadio->setGroup(themeRadioGroup_.get());
        lightRadio->setSelected(true);
        themeGroup->addChild(lightRadio);

        auto darkRadio = std::make_shared<LiteRadioButton>("Dark Theme");
        darkRadio->setWidth(LayoutValue::Percent(100));
        darkRadio->setHeight(28);
        darkRadio->setGroup(themeRadioGroup_.get());
        themeGroup->addChild(darkRadio);

        content->addChild(themeGroup);

        return content;
    }
};

// ==================== 新控件展示面板 ====================
class NewControlsPanel : public DemoPanel {
    std::shared_ptr<LiteProgressBar> progressBar_;
    std::shared_ptr<LiteSlider> slider_;
    std::shared_ptr<LiteLabel> valueLabel_;

public:
    LiteContainerPtr create() override {
        auto content = std::make_shared<LiteContainer>();
        content->setBackgroundColor(Color::White());
        content->setFlexDirection(FlexDirection::Column);
        content->setPadding(EdgeInsets::All(15));
        content->setGap(12);
        content->setWidth(LayoutValue::Percent(100));
        content->setFlexGrow(1);

        // LiteLabel 展示
        auto labelTitle = std::make_shared<LiteLabel>("LiteLabel Demo:");
        labelTitle->setHeight(20);
        labelTitle->setTextColor(Color::fromRGB(33, 33, 33));
        content->addChild(labelTitle);

        auto multiLabel = std::make_shared<LiteLabel>("This is a multi-line label that can wrap text automatically when the content is too long.");
        multiLabel->setWidth(LayoutValue::Percent(100));
        multiLabel->setHeight(40);
        multiLabel->setDisplayMode(LabelDisplayMode::MultiLine);
        content->addChild(multiLabel);

        // LiteProgressBar 展示
        auto progressTitle = std::make_shared<LiteLabel>("LiteProgressBar:");
        progressTitle->setHeight(20);
        content->addChild(progressTitle);

        progressBar_ = std::make_shared<LiteProgressBar>();
        progressBar_->setWidth(LayoutValue::Percent(100));
        progressBar_->setHeight(20);
        progressBar_->setValue(65.0f);
        progressBar_->setShowText(true);
        content->addChild(progressBar_);

        // LiteSlider 展示
        auto sliderTitle = std::make_shared<LiteLabel>("LiteSlider:");
        sliderTitle->setHeight(20);
        content->addChild(sliderTitle);

        auto sliderRow = std::make_shared<LiteContainer>();
        sliderRow->setWidth(LayoutValue::Percent(100));
        sliderRow->setHeight(30);
        sliderRow->setFlexDirection(FlexDirection::Row);
        sliderRow->setGap(10);
        sliderRow->setBackgroundColor(Color::Transparent());

        slider_ = std::make_shared<LiteSlider>();
        slider_->setWidth(LayoutValue::Percent(80));
        slider_->setHeight(30);
        slider_->setMinimum(0);
        slider_->setMaximum(100);
        slider_->setValue(65);

        valueLabel_ = std::make_shared<LiteLabel>("65");
        valueLabel_->setWidth(LayoutValue::Percent(20));
        valueLabel_->setHeight(30);

        slider_->setOnValueChanged([this](float val) {
            valueLabel_->setText(std::to_string((int)val));
            progressBar_->setValue(val);
        });

        sliderRow->addChild(slider_);
        sliderRow->addChild(valueLabel_);
        content->addChild(sliderRow);

        // LiteComboBox 展示
        auto comboTitle = std::make_shared<LiteLabel>("LiteComboBox:");
        comboTitle->setHeight(20);
        content->addChild(comboTitle);

        auto combo = std::make_shared<LiteComboBox>();
        combo->setWidth(LayoutValue::Percent(100));
        combo->setHeight(32);
        combo->addItem("Option 1");
        combo->addItem("Option 2");
        combo->addItem("Option 3");
        combo->setSelectedIndex(0);
        content->addChild(combo);

        return content;
    }
};

// ==================== 列表面板 ====================
class ListPanel : public DemoPanel {
public:
    LiteContainerPtr create() override {
        auto content = std::make_shared<LiteContainer>();
        content->setBackgroundColor(Color::White());
        content->setFlexDirection(FlexDirection::Column);
        content->setPadding(EdgeInsets::All(10));
        content->setWidth(LayoutValue::Percent(100));
        content->setFlexGrow(1);

        auto title = std::make_shared<LiteLabel>("LiteList Demo:");
        title->setHeight(20);
        content->addChild(title);

        auto list = std::make_shared<LiteList>();
        list->setWidth(LayoutValue::Percent(100));
        list->setHeight(180);
        list->setShowAlternateRows(true);
        
        list->addItem("Item 1 - First entry");
        list->addItem("Item 2 - Second entry");
        list->addItem("Item 3 - Third entry");
        list->addItem("Item 4 - Fourth entry");
        list->addItem("Item 5 - Fifth entry");
        list->addItem("Item 6 - ABC entry");
        list->addItem("Item 7 - 123 entry");
        
        list->setOnItemClicked([](int idx) {
            std::cout << "List item clicked: " << idx << std::endl;
        });
        
        content->addChild(list);
        return content;
    }
};

// ==================== 滚动视图面板 ====================
class ScrollViewPanel : public DemoPanel {
public:
    LiteContainerPtr create() override {
        auto content = std::make_shared<LiteContainer>();
        content->setBackgroundColor(Color::White());
        content->setFlexDirection(FlexDirection::Column);
        content->setPadding(EdgeInsets::All(10));
        content->setWidth(LayoutValue::Percent(100));
        content->setFlexGrow(1);

        auto title = std::make_shared<LiteLabel>("LiteScrollView Demo:");
        title->setHeight(20);
        content->addChild(title);

        auto scrollView = std::make_shared<LiteScrollView>();
        scrollView->setWidth(LayoutValue::Percent(100));
        scrollView->setHeight(180);
        scrollView->setScrollDirection(ScrollDirection::Vertical);
        scrollView->setBackgroundColor(Color::fromRGB(250, 250, 250));
        
        for (int i = 1; i <= 15; ++i) {
            auto label = std::make_shared<LiteLabel>("Scroll Item " + std::to_string(i));
            label->setWidth(LayoutValue::Percent(100));
            label->setHeight(30);
            label->setBackgroundColor(i % 2 == 0 ? Color::fromRGB(245, 245, 245) : Color::White());
            scrollView->addChild(label);
        }
        
        content->addChild(scrollView);
        return content;
    }
};

// ==================== 图片面板 ====================
class ImagePanel : public DemoPanel {
public:
    LiteContainerPtr create() override {
        auto content = std::make_shared<LiteContainer>();
        content->setBackgroundColor(Color::White());
        content->setFlexDirection(FlexDirection::Column);
        content->setPadding(EdgeInsets::All(10));
        content->setGap(10);
        content->setWidth(LayoutValue::Percent(100));
        content->setFlexGrow(1);

        auto title = std::make_shared<LiteLabel>("LiteImage Demo:");
        title->setHeight(20);
        content->addChild(title);

        auto image = std::make_shared<LiteImage>();
        image->setWidth(LayoutValue::Percent(100));
        image->setHeight(120);
        image->setScaleMode(ImageScaleMode::Fit);
        image->setPlaceholderColor(Color::fromRGB(200, 200, 200));
        // 无图片时显示占位符
        content->addChild(image);

        auto modeLabel = std::make_shared<LiteLabel>("Scale Mode: Fit (placeholder shown)");
        modeLabel->setHeight(20);
        modeLabel->setTextColor(Color::fromRGB(100, 100, 100));
        content->addChild(modeLabel);

        return content;
    }
};

// ==================== 树形控件面板 ====================
class TreeViewPanel : public DemoPanel {
    std::shared_ptr<LiteTreeView> tree_;
public:
    LiteContainerPtr create() override {
        tree_ = std::make_shared<LiteTreeView>();
        tree_->setWidth(LayoutValue::Percent(100));
        tree_->setHeight(LayoutValue::Percent(100));

        // 构建树结构
        auto* rootNode = new TreeNode("Root");
        rootNode->setExpanded(true);
        
        auto* docs = new TreeNode("Documents");
        docs->setExpanded(true);
        docs->addChild(new TreeNode("Work"));
        docs->addChild(new TreeNode("Personal"));
        rootNode->addChild(docs);

        auto* pics = new TreeNode("Pictures");
        pics->addChild(new TreeNode("2024"));
        pics->addChild(new TreeNode("2025"));
        rootNode->addChild(pics);

        rootNode->addChild(new TreeNode("Music"));
        
        tree_->setRootNode(rootNode);
        tree_->setShowRoot(false);

        tree_->setOnNodeSelected([](TreeNode* node) {
            std::cout << "Tree node selected: " << node->getText() << std::endl;
        });

        return tree_;
    }
};

// ==================== 数据表格面板 ====================
class DataTablePanel : public DemoPanel {
public:
    LiteContainerPtr create() override {
        auto content = std::make_shared<LiteContainer>();
        content->setBackgroundColor(Color::White());
        content->setFlexDirection(FlexDirection::Column);
        content->setPadding(EdgeInsets::All(10));
        content->setGap(10);
        content->setWidth(LayoutValue::Percent(100));
        content->setFlexGrow(1);

        auto title = std::make_shared<LiteLabel>("Data Table:");
        title->setHeight(25);
        content->addChild(title);

        auto table = std::make_shared<LiteTable>();
        table->setWidth(LayoutValue::Percent(100));
        table->setFlexGrow(1);

        table->addColumn("ID", 50, TextAlign::Center);
        table->addColumn("Name", 100, TextAlign::Left);
        table->addColumn("Email", 150, TextAlign::Left);
        table->addColumn("Status", 80, TextAlign::Center);

        table->addRow({"1", "Alice", "alice@test.com", "Active"});
        table->addRow({"2", "Bob", "bob@test.com", "Active"});
        table->addRow({"3", "Charlie", "charlie@test.com", "Inactive"});
        table->addRow({"4", "Diana", "diana@test.com", "Pending"});

        table->setOnRowClicked([](int row) {
            std::cout << "Row clicked: " << row << std::endl;
        });

        content->addChild(table);
        return content;
    }
};

// ==================== 主应用类 ====================
using LiteWindowPtr = std::shared_ptr<LiteWindow>;

class GuiDemoApp {
    LiteWindowManager manager_;
    LiteWindowPtr window_;
    LiteContainerPtr root_;
    std::shared_ptr<LiteLabel> statusLabel_;
    std::shared_ptr<LiteMenu> contextMenu_;

    // 面板实例
    LoginPanel loginPanel_;
    SettingsPanel settingsPanel_;
    NewControlsPanel newControlsPanel_;
    TreeViewPanel treeViewPanel_;
    DataTablePanel dataTablePanel_;
    ListPanel listPanel_;
    ScrollViewPanel scrollViewPanel_;
    ImagePanel imagePanel_;

public:
    bool init() {
        window_ = manager_.CreateWindow(1000, 750, "liteDui GUI Demo - Modular");
        if (!window_) {
            std::cerr << "Failed to create window" << std::endl;
            return false;
        }

        createUI();
        window_->SetRootContainer(root_);
        return true;
    }

    void run() {
        std::cout << "GUI Demo started with ALL controls:" << std::endl;
        std::cout << "  - LiteLabel, LiteProgressBar, LiteSlider, LiteComboBox" << std::endl;
        std::cout << "  - LiteTreeView, LiteTable, LiteList, LiteScrollView" << std::endl;
        std::cout << "  - LiteImage, LiteMenu, LiteDialog, LiteMessageBox" << std::endl;
        manager_.Run();
    }

private:
    void createUI() {
        root_ = std::make_shared<LiteContainer>();
        root_->setWidth(LayoutValue::Percent(100));
        root_->setHeight(LayoutValue::Percent(100));
        root_->setBackgroundColor(Color::fromRGB(240, 240, 240));
        root_->setPadding(EdgeInsets::All(15));
        root_->setFlexDirection(FlexDirection::Column);
        root_->setGap(12);

        // 标题
        auto title = std::make_shared<LiteLabel>("liteDui GUI Demo - All Controls");
        title->setWidth(LayoutValue::Percent(100));
        title->setHeight(40);
        title->setFontSize(20);
        title->setTextColor(Color::fromRGB(33, 33, 33));
        title->setTextAlign(TextAlign::Center);
        title->setBackgroundColor(Color::White());
        title->setBorderRadius(EdgeInsets::All(6));
        root_->addChild(title);

        // 主内容区
        auto mainContent = std::make_shared<LiteContainer>();
        mainContent->setWidth(LayoutValue::Percent(100));
        mainContent->setFlexGrow(1);
        mainContent->setFlexDirection(FlexDirection::Row);
        mainContent->setGap(12);
        mainContent->setBackgroundColor(Color::Transparent());

        // 左侧 TabView
        auto leftColumn = std::make_shared<LiteContainer>();
        leftColumn->setWidth(LayoutValue::Percent(50));
        leftColumn->setFlexDirection(FlexDirection::Column);
        leftColumn->setGap(12);
        leftColumn->setBackgroundColor(Color::Transparent());

        auto tabView = std::make_shared<LiteTabView>();
        tabView->setWidth(LayoutValue::Percent(100));
        tabView->setFlexGrow(1);
        tabView->setTabHeight(32);

        tabView->addTab("Login", loginPanel_.create());
        tabView->addTab("Settings", settingsPanel_.create());
        tabView->addTab("Controls", newControlsPanel_.create());
        tabView->addTab("List", listPanel_.create());
        tabView->addTab("Scroll", scrollViewPanel_.create());
        tabView->addTab("Image", imagePanel_.create());

        leftColumn->addChild(tabView);
        mainContent->addChild(leftColumn);

        // 右侧
        auto rightColumn = std::make_shared<LiteContainer>();
        rightColumn->setWidth(LayoutValue::Percent(50));
        rightColumn->setFlexDirection(FlexDirection::Column);
        rightColumn->setGap(12);
        rightColumn->setBackgroundColor(Color::Transparent());

        // TreeView 区域
        auto treeSection = std::make_shared<LiteGroupBox>("Tree View");
        treeSection->setWidth(LayoutValue::Percent(100));
        treeSection->setHeight(180);
        treeSection->setFlexDirection(FlexDirection::Column);
        treeSection->addChild(treeViewPanel_.create());
        rightColumn->addChild(treeSection);

        // Table 区域
        auto tableSection = std::make_shared<LiteGroupBox>("Data Table");
        tableSection->setWidth(LayoutValue::Percent(100));
        tableSection->setFlexGrow(1);
        tableSection->setFlexDirection(FlexDirection::Column);
        tableSection->addChild(dataTablePanel_.create());
        rightColumn->addChild(tableSection);

        mainContent->addChild(rightColumn);
        root_->addChild(mainContent);

        // 按钮区域
        auto btnArea = std::make_shared<LiteContainer>();
        btnArea->setWidth(LayoutValue::Percent(100));
        btnArea->setHeight(45);
        btnArea->setFlexDirection(FlexDirection::Row);
        btnArea->setJustifyContent(Align::Center);
        btnArea->setGap(10);
        btnArea->setBackgroundColor(Color::Transparent());

        // MessageBox 按钮
        auto msgBoxBtn = std::make_shared<LiteButton>("MessageBox");
        msgBoxBtn->setWidth(100);
        msgBoxBtn->setHeight(36);
        msgBoxBtn->setNormalBackgroundColor(Color::fromRGB(66, 133, 244));
        msgBoxBtn->setNormalTextColor(Color::White());
        msgBoxBtn->setOnClick([this](const MouseEvent& e) {
            auto msgBox = LiteMessageBox::information("Info", "This is a LiteMessageBox!");
            msgBox->setWidth(LayoutValue::Percent(100));
            msgBox->setHeight(LayoutValue::Percent(100));
            msgBox->show(window_.get());
            statusLabel_->setText("MessageBox shown!");
        });
        btnArea->addChild(msgBoxBtn);

        // Dialog 按钮
        auto dialogBtn = std::make_shared<LiteButton>("Dialog");
        dialogBtn->setWidth(100);
        dialogBtn->setHeight(36);
        dialogBtn->setNormalBackgroundColor(Color::fromRGB(76, 175, 80));
        dialogBtn->setNormalTextColor(Color::White());
        dialogBtn->setOnClick([this](const MouseEvent& e) {
            auto dialog = std::make_shared<LiteDialog>();
            dialog->setTitle("LiteDialog Demo");
            dialog->setDialogSize(300, 150);
            dialog->setStandardButtons(Ok | Cancel);
            dialog->setOnAccepted([this]() { statusLabel_->setText("Dialog accepted!"); });
            dialog->setOnRejected([this]() { statusLabel_->setText("Dialog rejected!"); });
            dialog->show(window_.get());
            statusLabel_->setText("Dialog shown!");
        });
        btnArea->addChild(dialogBtn);

        // Menu 按钮
        auto menuBtn = std::make_shared<LiteButton>("Menu");
        menuBtn->setWidth(100);
        menuBtn->setHeight(36);
        menuBtn->setNormalBackgroundColor(Color::fromRGB(156, 39, 176));
        menuBtn->setNormalTextColor(Color::White());
        menuBtn->setOnClick([this, menuBtn](const MouseEvent& e) {
            if (!contextMenu_) {
                contextMenu_ = std::make_shared<LiteMenu>();
                contextMenu_->addItem("Menu Item 1", [this]() { statusLabel_->setText("Menu Item 1 clicked!"); });
                contextMenu_->addItem("Menu Item 2", [this]() { statusLabel_->setText("Menu Item 2 clicked!"); });
                contextMenu_->addSeparator();
                contextMenu_->addItem("Menu Item 3", [this]() { statusLabel_->setText("Menu Item 3 clicked!"); });
            }
            // 使用按钮的绝对坐标定位菜单，并传入 window 指针
            float absX = menuBtn->getAbsoluteLeft() + e.x;
            float absY = menuBtn->getAbsoluteTop() + e.y;
            contextMenu_->show(absX, absY, window_.get());
            statusLabel_->setText("Menu shown!");
        });
        btnArea->addChild(menuBtn);

        // Exit 按钮
        auto exitBtn = std::make_shared<LiteButton>("Exit");
        exitBtn->setWidth(80);
        exitBtn->setHeight(36);
        exitBtn->setNormalBackgroundColor(Color::fromRGB(244, 67, 54));
        exitBtn->setNormalTextColor(Color::White());
        exitBtn->setOnClick([](const MouseEvent& e) {
            exit(0);
        });
        btnArea->addChild(exitBtn);

        root_->addChild(btnArea);

        // 状态栏
        statusLabel_ = std::make_shared<LiteLabel>("Ready - Click controls to interact");
        statusLabel_->setWidth(LayoutValue::Percent(100));
        statusLabel_->setHeight(28);
        statusLabel_->setTextAlign(TextAlign::Center);
        statusLabel_->setFontSize(13);
        statusLabel_->setBackgroundColor(Color::White());
        statusLabel_->setBorderRadius(EdgeInsets::All(4));
        statusLabel_->setTextColor(Color::fromRGB(100, 100, 100));
        root_->addChild(statusLabel_);
    }
};

// ==================== 主函数 ====================
int main() {
    GuiDemoApp app;
    if (!app.init()) {
        return 1;
    }
    app.run();
    return 0;
}
