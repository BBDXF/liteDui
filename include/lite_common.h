/**
 * lite_common.h - liteDui库的公共结构体和类型定义
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

#ifdef __linux__
#define LITE_DEFAULT_FONT_FAMILY "Noto Sans CJK SC"
#elif _WIN32
#define LITE_DEFAULT_FONT_FAMILY "Microsoft YaHei"
#elif __APPLE__
#define LITE_DEFAULT_FONT_FAMILY "PingFang SC"
#endif

namespace liteDui {

// 颜色结构体
struct Color {
    float r, g, b, a;

    Color() : r(0), g(0), b(0), a(1) {}
    Color(float red, float green, float blue, float alpha = 1.0f)
        : r(red), g(green), b(blue), a(alpha) {}

    static Color fromRGB(int r, int g, int b, int a = 255) {
        return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }

    // 转换为ARGB格式的uint32_t (Skia兼容)
    uint32_t toARGB() const {
        return (static_cast<uint32_t>(a * 255) << 24) |
               (static_cast<uint32_t>(r * 255) << 16) |
               (static_cast<uint32_t>(g * 255) << 8) |
               static_cast<uint32_t>(b * 255);
    }

    static Color Transparent() { return Color(0, 0, 0, 0); }
    static Color Black() { return Color(0, 0, 0, 1); }
    static Color White() { return Color(1, 1, 1, 1); }
    static Color Red() { return Color(1, 0, 0, 1); }
    static Color Green() { return Color(0, 1, 0, 1); }
    static Color Blue() { return Color(0, 0, 1, 1); }
    static Color Gray() { return Color(0.5f, 0.5f, 0.5f, 1); }
    static Color LightGray() { return Color(0.75f, 0.75f, 0.75f, 1); }
    static Color DarkGray() { return Color(0.25f, 0.25f, 0.25f, 1); }
};

// 布局值类型
struct LayoutValue {
    float value = 0.0f;
    bool isPercent = false;
    bool isAuto = false;

    LayoutValue() = default;
    LayoutValue(float v) : value(v) {}
    LayoutValue(float v, bool percent) : value(v), isPercent(percent) {}

    static LayoutValue Auto() {
        LayoutValue v;
        v.isAuto = true;
        return v;
    }
    static LayoutValue Percent(float v) { return LayoutValue(v, true); }
    static LayoutValue Point(float v) { return LayoutValue(v, false); }
};

// 边距设置
struct EdgeInsets {
    LayoutValue left, top, right, bottom;

    EdgeInsets() = default;
    EdgeInsets(float all) : left(all), top(all), right(all), bottom(all) {}
    EdgeInsets(float h, float v) : left(h), top(v), right(h), bottom(v) {}
    EdgeInsets(float l, float t, float r, float b) : left(l), top(t), right(r), bottom(b) {}

    static EdgeInsets All(float v) { return EdgeInsets(v); }
    static EdgeInsets Symmetric(float h, float v) { return EdgeInsets(h, v); }
};

// Flex方向枚举
enum class FlexDirection {
    Column = 0,
    ColumnReverse = 1,
    Row = 2,
    RowReverse = 3
};

// 对齐方式枚举
enum class Align {
    Auto = 0,
    FlexStart = 1,
    Center = 2,
    FlexEnd = 3,
    Stretch = 4,
    Baseline = 5,
    SpaceBetween = 6,
    SpaceAround = 7,
    SpaceEvenly = 8
};

// 位置类型枚举
enum class PositionType {
    Static = 0,
    Relative = 1,
    Absolute = 2
};

// 换行方式枚举
enum class FlexWrap {
    NoWrap = 0,
    Wrap = 1,
    WrapReverse = 2
};

// 溢出处理枚举
enum class Overflow {
    Visible = 0,
    Hidden = 1,
    Scroll = 2
};

// 显示类型枚举
enum class Display {
    Flex = 0,
    None = 1
};

// 边框样式枚举
enum class BorderStyle {
    Solid,
    Dashed,
    Dotted
};

// 文本对齐枚举
enum class TextAlign {
    Left,
    Center,
    Right
};

// 字体粗细枚举
enum class FontWeight {
    Normal = 400,
    Bold = 700
};

// 字体样式枚举
enum class FontStyle {
    Normal,
    Italic
};

// 鼠标按钮枚举
enum class MouseButton {
    Left = 0,
    Right = 1,
    Middle = 2
};

// 鼠标事件结构体
struct MouseEvent {
    float x, y;
    MouseButton button;
    bool pressed;
    bool released;

    MouseEvent() : x(0), y(0), button(MouseButton::Left), pressed(false), released(false) {}
    MouseEvent(float xpos, float ypos, MouseButton btn = MouseButton::Left)
        : x(xpos), y(ypos), button(btn), pressed(false), released(false) {}
};

// 键盘事件结构体
struct KeyEvent {
    int keyCode;
    bool pressed;
    bool released;
    int mods;
    unsigned int codepoint;

    KeyEvent() : keyCode(0), pressed(false), released(false), mods(0), codepoint(0) {}
};

// 控件状态枚举
enum class ControlState {
    Normal,
    Hover,
    Pressed,
    Focused,
    Disabled
};

// 回调函数类型
using MouseEventCallback = std::function<void(const MouseEvent&)>;
using TextChangedCallback = std::function<void(const std::string&)>;
using FocusChangedCallback = std::function<void(bool)>;

} // namespace liteDui
