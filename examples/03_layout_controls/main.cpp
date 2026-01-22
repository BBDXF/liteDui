/**
 * Layout and Controls Demo
 * 演示 liteDui 的布局系统和GUI控件
 */

#include "lite_layout.h"
#include "lite_container.h"
#include "lite_button.h"
#include "lite_input.h"

#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/encode/SkPngEncoder.h"
#include "include/core/SkStream.h"

#include <iostream>
#include <memory>

using namespace liteDui;

int main() {
    // 创建位图和画布 (模拟窗口)
    SkBitmap bitmap;
    bitmap.allocPixels(SkImageInfo::MakeN32Premul(800, 600));
    SkCanvas canvas(bitmap);
    canvas.clear(SK_ColorWHITE);

    // 创建根容器
    auto root = std::make_shared<LiteContainer>();
    root->setWidth(800);
    root->setHeight(600);
    root->setBackgroundColor(Color::fromRGB(240, 240, 240));
    root->setPadding(EdgeInsets::All(20));
    root->setFlexDirection(FlexDirection::Column);
    root->setGap(15);

    // 创建标题
    auto title = std::make_shared<LiteContainer>();
    title->setWidth(LayoutValue::Percent(100));
    title->setHeight(50);
    title->setText("liteDui Layout & Controls Demo");
    title->setFontSize(24);
    title->setTextColor(Color::fromRGB(51, 51, 51));
    title->setTextAlign(TextAlign::Center);
    title->setBackgroundColor(Color::White());
    title->setBorderRadius(EdgeInsets::All(8));
    title->setPadding(EdgeInsets::All(10));
    root->addChild(title);

    // 创建水平布局容器
    auto row1 = std::make_shared<LiteContainer>();
    row1->setWidth(LayoutValue::Percent(100));
    row1->setHeight(60);
    row1->setFlexDirection(FlexDirection::Row);
    row1->setJustifyContent(Align::SpaceBetween);
    row1->setAlignItems(Align::Center);
    row1->setGap(10);
    row1->setBackgroundColor(Color::Transparent());

    // 创建按钮
    auto btn1 = std::make_shared<LiteButton>("Primary Button");
    btn1->setWidth(150);
    btn1->setHeight(40);
    btn1->setNormalBackgroundColor(Color::fromRGB(66, 133, 244));
    btn1->setHoverBackgroundColor(Color::fromRGB(53, 122, 232));
    btn1->setPressedBackgroundColor(Color::fromRGB(40, 100, 200));
    btn1->setNormalTextColor(Color::White());
    btn1->setHoverTextColor(Color::White());
    btn1->setPressedTextColor(Color::White());
    btn1->setOnClick([](const MouseEvent& e) {
        std::cout << "Primary Button clicked!" << std::endl;
    });
    row1->addChild(btn1);

    auto btn2 = std::make_shared<LiteButton>("Secondary");
    btn2->setWidth(120);
    btn2->setHeight(40);
    row1->addChild(btn2);

    auto btn3 = std::make_shared<LiteButton>("Disabled");
    btn3->setWidth(120);
    btn3->setHeight(40);
    btn3->setDisabled(true);
    row1->addChild(btn3);

    root->addChild(row1);

    // 创建输入框区域
    auto inputSection = std::make_shared<LiteContainer>();
    inputSection->setWidth(LayoutValue::Percent(100));
    inputSection->setFlexDirection(FlexDirection::Column);
    inputSection->setGap(10);
    inputSection->setBackgroundColor(Color::White());
    inputSection->setBorderRadius(EdgeInsets::All(8));
    inputSection->setPadding(EdgeInsets::All(15));

    auto inputLabel = std::make_shared<LiteContainer>();
    inputLabel->setText("Input Controls:");
    inputLabel->setFontSize(16);
    inputLabel->setTextColor(Color::fromRGB(51, 51, 51));
    inputLabel->setHeight(25);
    inputLabel->setBackgroundColor(Color::Transparent());
    inputSection->addChild(inputLabel);

    auto input1 = std::make_shared<LiteInput>("Enter your name...");
    input1->setWidth(LayoutValue::Percent(100));
    input1->setHeight(40);
    input1->setOnTextChanged([](const std::string& text) {
        std::cout << "Text changed: " << text << std::endl;
    });
    inputSection->addChild(input1);

    auto input2 = std::make_shared<LiteInput>("Password");
    input2->setWidth(LayoutValue::Percent(100));
    input2->setHeight(40);
    input2->setInputType(InputType::Password);
    inputSection->addChild(input2);

    root->addChild(inputSection);

    // 创建Flex布局演示区域
    auto flexDemo = std::make_shared<LiteContainer>();
    flexDemo->setWidth(LayoutValue::Percent(100));
    flexDemo->setFlex(1);
    flexDemo->setFlexDirection(FlexDirection::Row);
    flexDemo->setGap(10);
    flexDemo->setBackgroundColor(Color::Transparent());

    // 左侧面板
    auto leftPanel = std::make_shared<LiteContainer>();
    leftPanel->setFlex(1);
    leftPanel->setBackgroundColor(Color::fromRGB(232, 245, 233));
    leftPanel->setBorderRadius(EdgeInsets::All(8));
    leftPanel->setPadding(EdgeInsets::All(15));
    leftPanel->setText("Flex: 1");
    leftPanel->setTextAlign(TextAlign::Center);
    flexDemo->addChild(leftPanel);

    // 中间面板
    auto centerPanel = std::make_shared<LiteContainer>();
    centerPanel->setFlex(2);
    centerPanel->setBackgroundColor(Color::fromRGB(227, 242, 253));
    centerPanel->setBorderRadius(EdgeInsets::All(8));
    centerPanel->setPadding(EdgeInsets::All(15));
    centerPanel->setText("Flex: 2");
    centerPanel->setTextAlign(TextAlign::Center);
    flexDemo->addChild(centerPanel);

    // 右侧面板
    auto rightPanel = std::make_shared<LiteContainer>();
    rightPanel->setFlex(1);
    rightPanel->setBackgroundColor(Color::fromRGB(255, 243, 224));
    rightPanel->setBorderRadius(EdgeInsets::All(8));
    rightPanel->setPadding(EdgeInsets::All(15));
    rightPanel->setText("Flex: 1");
    rightPanel->setTextAlign(TextAlign::Center);
    flexDemo->addChild(rightPanel);

    root->addChild(flexDemo);

    // 计算布局
    root->calculateLayout(800, 600);

    // 渲染
    root->renderTree(&canvas);

    // 保存为PNG
    SkFILEWStream file("layout_controls_demo.png");
    SkPngEncoder::Encode(&file, bitmap.pixmap(), {});

    std::cout << "Demo rendered to layout_controls_demo.png" << std::endl;
    std::cout << "\nLayout tree:" << std::endl;
    std::cout << "Root: " << root->getLayoutWidth() << "x" << root->getLayoutHeight() << std::endl;
    std::cout << "  Title: " << title->getLayoutWidth() << "x" << title->getLayoutHeight() << std::endl;
    std::cout << "  Row1: " << row1->getLayoutWidth() << "x" << row1->getLayoutHeight() << std::endl;
    std::cout << "  InputSection: " << inputSection->getLayoutWidth() << "x" << inputSection->getLayoutHeight() << std::endl;
    std::cout << "  FlexDemo: " << flexDemo->getLayoutWidth() << "x" << flexDemo->getLayoutHeight() << std::endl;

    return 0;
}
