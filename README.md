# liteDui
litter Direct User Interface Framework. 

借鉴windows平台下的DirectUI和Flutter的方式，使用窗口直接绘制技术构建GUI程序的框架。


## 架构说明
- 使用CMake C++17 开发，首先支持Linux 平台
- 窗口管理使用 GLFW
- 通过OpenGL提供给Skia进行绘制
    - 富文本和文本排版
    - 图像
    - 路径
    - 渐变
    - 矢量/svg
    - 创建控件：按钮，文本框，图片，组合框，GroupBox, 可滚动容器，Tab，树控件，浏览器，表格，List
- 使用Yoga提供Flex Layout管理
- 支持CSS属性美化样式
- 支持XML定制Form
- 导出为C ABI
- 资源管理(image/font/css)


后续辅助项目：
- QuickJS 嵌入，支持全程通过JS实现GUI和功能
- 添加Golang/Python/Rust 语言支持
- 支持更多平台(Android, Windows, Linux, MacOS)
- Skia瘦身，自定义构建
- 从网络进行资源加载


## 参考
- [DuiLib_Ultimate](https://github.com/qdtroy/DuiLib_Ultimate)
- [Flutter](https://github.com/flutter/flutter)
- [Yoga](https://github.com/facebook/yoga)
- [Skia](https://github.com/google/skia)
- [QuickJS](https://github.com/bellard/mquickjs)

预编译的Skia:
- [SkiaBuild](https://github.com/HumbleUI/SkiaBuild)
- [Skia Builder](https://github.com/olilarkin/skia-builder)

## Build
TOOD

## Dev Log
TODO
