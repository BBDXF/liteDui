# liteDui
Simple Direct User Interface Framework. 虽然叫lite GUI, 但是一点也不简单。

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
