# AGENTS.md - liteDui 开发指南

本文档为 AI 代理提供 liteDui 代码库的编码规范和构建说明。

## 项目概述

**liteDui** 是一个用 C++17 编写的轻量级直接用户界面框架，主要面向 Linux 平台。

**技术栈：**
 **语言：** C++17
 **构建系统：** CMake 3.16+
 **窗口管理：** GLFW
 **渲染引擎：** Skia (OpenGL 后端)
 **布局引擎：** Yoga (Flexbox)
 **平台：** Linux (主要)，未来支持 Windows/macOS/Android

**架构：**
- `include/` - 公共头文件
- `src/` - 实现文件，按类别组织：
  - `controls/` - UI 控件（按钮、输入框、列表等）
  - `layout/` - 布局和容器类
  - `render/` - 渲染和字体管理
  - `utils/` - 工具函数
  - `window/` - 窗口管理
- `examples/` - 示例应用
- `third-party/` - 外部依赖（Skia、Yoga）

---

## 构建命令

### 初始设置

```bash
# 克隆并进入项目
cd liteDui
# 创建构建目录
mkdir build && cd build
# 配置 CMake
cmake ..
# 构建项目
make -j$(nproc)
```

### 构建命令

```bash
# 完整构建（从 build/ 目录）
make -j$(nproc)
# 构建特定目标
make litedui              # 仅构建库
make 01_glfw_win          # 构建特定示例
make 04_gui_demo          # 构建 GUI 演示

# 清理构建
make clean
# 从头重新构建
rm -rf build && mkdir build && cd build && cmake .. && make -j$(nproc)
```

### 运行示例

```bash
# 从 build/ 目录运行
./bin/01_glfw_win         # 基础 GLFW 窗口
./bin/02_skia_basic       # Skia 渲染演示
./bin/03_layout_controls  # 布局系统演示
./bin/04_gui_demo         # 完整 GUI 控件演示
```

### 调试构建

项目默认配置为调试构建（`CMAKE_BUILD_TYPE=Debug`，`-g -O0`）。

```bash
# 生成 compile_commands.json 供 clangd 使用
cmake ..  # 已通过 CMAKE_EXPORT_COMPILE_COMMANDS 启用
```

### 无测试框架

目前没有自动化测试套件。验证通过示例应用程序完成。

---

## 代码风格指南

### 文件组织

**头文件 (.h)：**
 以 Doxygen 风格文件注释开头：`/** * filename.h - 简要描述 */`
 使用 `#pragma once` 作为包含保护
 包含顺序：
  1. 项目头文件（引号，如 `"lite_common.h"`）
  2. 第三方头文件（尖括号，如 `<yoga/YGNode.h>`）
  3. 标准库头文件（尖括号，如 `<memory>`、`<vector>`）
 命名空间前的前向声明
 所有代码在 `namespace liteDui { }` 内

**实现文件 (.cpp)：**
 以 Doxygen 风格文件注释开头
 首先包含对应的头文件
 然后是外部依赖（Skia、Yoga）
 谨慎使用 `using namespace`（仅用于第三方命名空间，如 `skia::textlayout`）

**示例：**
```cpp
/**
 * lite_button.h - 按钮控件
 */

#pragma once

#include "lite_container.h"

namespace liteDui {

class LiteButton;
using LiteButtonPtr = std::shared_ptr<LiteButton>;

class LiteButton : public LiteContainer {
    // ...
};

} // namespace liteDui
```

### 命名约定

 **类：** PascalCase，带 `Lite` 前缀（如 `LiteButton`、`LiteContainer`）
 **类型别名：** PascalCase，shared_ptr 带 `Ptr` 后缀（如 `LiteButtonPtr`）
 **成员变量：** camelCase，带 `m_` 前缀（如 `m_backgroundColor`、`m_state`）
 **函数/方法：** camelCase（如 `setBackgroundColor()`、`onMousePressed()`）
 **枚举：** enum class 名称用 PascalCase，值用 PascalCase（如 `enum class ControlState { Normal, Hover, Pressed }`）
 **常量：** 宏使用 UPPER_SNAKE_CASE（如 `LITE_DEFAULT_FONT_FAMILY`）
 **命名空间：** camelCase（如 `namespace liteDui`）

### 类型和内存管理

 **智能指针：** 使用 `std::shared_ptr` 表示所有权，`std::weak_ptr` 表示非所有权引用
 **类型别名：** 为所有主要类定义 `using XxxPtr = std::shared_ptr<Xxx>`
 **引用传递：** 只读参数使用 `const Type&`，可变参数使用 `Type&`
 **返回类型：** 小类型按值返回，大型只读返回 `const Type&`，所有权转移返回智能指针

### 类结构

**公共接口优先：**
```cpp
class LiteButton : public LiteContainer {
public:
    // Constructors/Destructor
    LiteButton();
    explicit LiteButton(const std::string& text);
    ~LiteButton() override = default;

    // Public methods (grouped by functionality)
    void setOnClick(MouseEventCallback callback);
    void setDisabled(bool disabled);
    bool isDisabled() const { return m_state == ControlState::Disabled; }

    // Event handlers (virtual overrides)
    void onMousePressed(const MouseEvent& event) override;

private:
    // Private methods
    void setState(ControlState state);
    void updateAppearance();

    // Member variables
    ControlState m_state = ControlState::Normal;
    MouseEventCallback m_onClick;
    Color m_normalBgColor = Color::LightGray();
};
```

### 注释

 **文件头：** Doxygen 风格，包含文件名和简要描述
 **类文档：** 类声明上方的 Doxygen 风格注释
 **内联注释：** 实现细节可使用中文（项目使用中英文混合）
 **函数注释：** 复杂逻辑使用内联注释，用节注释分组相关代码

**示例：**
```cpp
// 背景属性
void LiteContainer::setBackgroundColor(const Color& color) {
    m_backgroundColor = color;
    markDirty();
}

// 尝试加载指定字体
const std::string& family = fontFamily.empty() ? m_defaultFontFamily : fontFamily;
```

### 格式化

 **缩进：** 4 个空格（不使用制表符）
 **大括号：** 函数/类的左大括号与声明同行，控制结构同样
 **行长度：** 无严格限制，但保持合理（约 100-120 字符）
 **空格：** 关键字后加空格（`if (`、`for (`），函数调用不加空格（`func()`）
 **初始化：** 使用成员初始化列表，简单析构函数优先使用 `= default`

### 虚函数和重写

 虚函数重写始终使用 `override` 关键字
 默认实现使用 `= default`
 为事件处理器提供空的默认实现：`virtual void onMousePressed(const MouseEvent& event) {}`

### 错误处理

 **不使用异常：** 项目不使用异常
 **空指针检查：** 解引用前始终检查指针：`if (!canvas) return;`
 **提前返回：** 对保护条件使用提前返回
 **断言：** 目前不使用；优先使用防御性检查

### 常量和枚举

 使用 `enum class` 实现类型安全的枚举
 使用预处理器定义平台特定常量：
```cpp
#ifdef __linux__
#define LITE_DEFAULT_FONT_FAMILY "Noto Sans CJK SC"
#elif _WIN32
#define LITE_DEFAULT_FONT_FAMILY "Microsoft YaHei"
#endif
```

### 回调和函数类型

 在 `lite_common.h` 中定义回调类型：
```cpp
using MouseEventCallback = std::function<void(const MouseEvent&)>;
using TextChangedCallback = std::function<void(const std::string&)>;
```

---

## 开发工作流

### 添加新控件

1. 在 `include/lite_xxx.h` 中创建头文件
2. 在 `src/controls/lite_xxx.cpp` 中创建实现
3. 继承自 `LiteContainer` 或 `LiteLayout`
4. 遵循命名约定和文件结构
5. 如需要，添加到 CMakeLists.txt（当前使用 `GLOB_RECURSE`）
6. 使用示例应用测试

### 修改现有代码

1. 保持现有代码风格和模式
2. 保持成员变量命名（`m_` 前缀）
3. 保留虚函数签名
4. 如 API 更改，更新相关示例
5. 使用 `./bin/04_gui_demo` 测试

### 依赖项

 **Skia：** 位于 `third-party/skia/out/Release-x64/`
 **Yoga：** 从 `third-party/yoga/` 构建
 **GLFW：** 通过 pkg-config 的系统包

---

## 常见模式

### 创建控件

```cpp
auto button = std::make_shared<LiteButton>("Click Me");
button->setWidth(LayoutValue::Percent(100));
button->setHeight(36);
button->setOnClick([](const MouseEvent& e) {
    std::cout << "Button clicked!" << std::endl;
});
container->addChild(button);
```

### Flexbox 布局

```cpp
auto container = std::make_shared<LiteContainer>();
container->setFlexDirection(FlexDirection::Column);
container->setPadding(EdgeInsets::All(15));
container->setGap(10);
container->setAlignItems(Align::Center);
```

### 状态管理

```cpp
void LiteButton::setState(ControlState state) {
    if (m_state == state) return;  // Avoid redundant updates
    m_state = state;
    updateAppearance();
}
```

---

## 重要说明

 **无 Linter/Formatter：** 项目不使用 clang-format 或 clang-tidy
 **中文注释：** 实现文件中可使用中文注释
 **调试符号：** 始终启用（`-g -O0`）
 **位置无关代码：** 全局启用（`CMAKE_POSITION_INDEPENDENT_CODE ON`）
 **C++ 标准：** 严格 C++17，无扩展（`CMAKE_CXX_EXTENSIONS OFF`）

---

## 未来考虑

 添加单元测试框架（目前没有）
 添加 clang-format 配置以保持一致性
 文档化 CSS 样式系统（计划功能）
 文档化 XML 表单构建器（计划功能）
