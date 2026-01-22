# liteDui
Simple Direct User Interface Framework. 虽然叫lite GUI, 但是一点也不简单。

目标:
- 基于 GLFW 和 Skia 创建的 DirectUI GUI 框架。
- 完全基于 Skia 框架和相关扩展，实现 GUI 控件的自绘制。
- 支持 Flex Layout.
- 支持 CSS 美化 Controls.
- 支持 window management 和 builder.
- 实现基本的 UI 控件，比如 `LiteContainer, LiteElement, LiteInput, LiteImage, LiteScrollView`
- 使用 C++ Proxy 技术抽象 class 与实现。

## 规范
- 所有对外 ABI 的头文件放在 include 文件夹, 如果内部使用，放在 `src/internal`。
- 所有源代码分类放在 src 文件夹下。
- 对于每个主要的模块，都在 examples 下创建对应的核心测试 demo, 方便学习和完善。

## TODO
- [x] 基于 GLFW 的窗口管理 class 和 Demo
- [x] 基于 Yoga 实现的控件树和对应 demo.
- [x] 基于 Skia 实现的基础 Layout 和 Container, Element.
- [ ] Layout 和控件支持 CSS
- [ ] 完善的 Layout flex/fixed，更多的控件。

## Note
```bash
## 实施时间线

### 第一阶段 (4-6周)

- [x] Skia 渲染引擎集成
- [x] Yoga Flexbox 布局引擎
- [x] 基础组件系统 (Container, Button, Input)
- [x] 简单示例应用

### 第二阶段 (4-6周)

- [ ] CSS 样式系统
- [ ] 事件处理系统完善
- [ ] 高级组件实现 (ScrollView, Image 等)
- [ ] 性能优化

### 第三阶段 (3-4周)

- [ ] QuickJS 集成 (可选)
- [ ] 资源管理 (image/font/css)
- [ ] 多平台支持扩展
- [ ] 文档和示例

## 技术风险和解决方案

### 主要风险

1. __Skia 集成复杂性__: Skia 库庞大，编译和集成困难
2. __布局精度__: Flexbox 在 Skia 上的精确实现
3. __性能问题__: 实时渲染的性能开销
4. __跨平台一致性__: GLFW 和 Skia 在不同平台的行为

### 解决方案

1. __预编译 Skia__: 使用预编译的 Skia 库简化集成
2. __测试驱动__: 大量布局测试用例
3. __优化渲染__: 实现高效的渲染管道
4. __抽象层__: 平台抽象层确保一致性

## 成功指标

### 功能指标

- [x] 支持基础控件 (Container, Button, Input)
- [x] 完整的 Flexbox 布局支持
- [ ] CSS 兼容性达到 80%+
- [ ] 应用启动时间 < 3秒

### 性能指标

- [ ] 60fps 流畅渲染
- [ ] 内存使用 < 150MB
- [ ] 组件渲染延迟 < 16ms

这个方案将 liteDui 转变为一个现代化的 DirectUI 框架，既保持了原有的 Skia/GLFW 基础，又引入了现代化的开发体验。

```

## 架构说明
- 使用 CMake C++17 开发，首先支持 Linux 平台
- 窗口管理使用 GLFW
- 通过 OpenGL 提供给 Skia 进行绘制
    - 富文本和文本排版
    - 图像
    - 路径
    - 渐变
    - 矢量/svg
    - 创建控件：按钮，文本框，图片，组合框，GroupBox, 可滚动容器，Tab，树控件，浏览器，表格，List
- 使用 Yoga 提供 Flex Layout 管理
- 支持 CSS 属性美化样式
- 支持 XML 定制 Form
- 导出为 C ABI
- 资源管理 (image/font/css)

后续辅助项目：
- QuickJS 嵌入，支持全程通过 JS 实现 GUI 和功能
- 添加 Golang/Python/Rust 语言支持
- 支持更多平台 (Android, Windows, Linux, MacOS)
- Skia 瘦身，自定义构建
- 从网络进行资源加载

## 参考
- [DuiLib_Ultimate](https://github.com/qdtroy/DuiLib_Ultimate)
- [Flutter](https://github.com/flutter/flutter)
- [Yoga](https://github.com/facebook/yoga)
- [Skia](https://github.com/google/skia)
- [QuickJS](https://github.com/bellard/quickjs)

预编译的 Skia:
- [SkiaBuild](https://github.com/HumbleUI/SkiaBuild)
- [Skia Builder](https://github.com/olilarkin/skia-builder)

## Build
```bash
# 克隆项目
git clone <repo>
cd liteDui

# 创建构建目录
mkdir build && cd build

# 配置和构建
cmake ..
make -j$(nproc)

# 运行示例
./bin/01_glfw_win
./bin/04_gui_demo
```

## Dev Log
- [x] 初始化项目结构
- [x] 集成 GLFW 和 Skia
- [x] 实现基础布局和控件
- [x] 创建 GUI demo
- [ ] 添加 CSS 支持
- [ ] 完善事件系统
