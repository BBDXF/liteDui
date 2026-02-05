# Overlay 系统设计文档

## 1. 问题背景

### 1.1 当前问题

liteDui 框架中存在多种需要弹出显示的控件：
- ComboBox 下拉列表
- Menu 弹出菜单
- Dialog 对话框
- MessageBox 消息框

当前实现存在以下问题：

1. **ComboBox/Menu 问题**：下拉内容绘制在控件布局区域外，但事件分发系统 `findDeepestContainerAtPosition` 只检测控件布局边界内的点击，导致无法正确响应下拉项的点击。

2. **实现不统一**：Dialog/MessageBox 采用全屏 overlay 方式，而 ComboBox/Menu 尝试在控件边界外绘制。

3. **渲染层级问题**：弹出内容可能被其他控件遮挡。

### 1.2 现有实现分析

**Dialog/MessageBox 方式：**
```cpp
// 使用时设置为全屏
dialog->setWidth(LayoutValue::Percent(100));
dialog->setHeight(LayoutValue::Percent(100));
root_->addChild(dialog);

// render 中绘制半透明遮罩 + 居中对话框
void LiteDialog::render(SkCanvas* canvas) {
    // 模态背景覆盖整个区域
    canvas->drawRect(SkRect::MakeXYWH(0, 0, w, h), overlayPaint);
    // 对话框居中绘制
    float dlgX = (w - m_dialogWidth) / 2;
    float dlgY = (h - m_dialogHeight) / 2;
    // ...
}
```

**ComboBox 当前问题：**
```cpp
// 控件高度只有 32px
setHeight(32.0f);

// 下拉列表绘制在 y > h 的位置（控件边界外）
canvas->drawRect(SkRect::MakeXYWH(0, h, w, dropdownHeight), bgPaint);

// 事件分发检测不到这个区域的点击
```

## 2. 设计目标

1. 统一所有弹出层的管理机制
2. 正确处理弹出层的事件分发
3. 保证弹出层的渲染层级
4. 支持弹出层嵌套（如 Dialog 中的 ComboBox）
5. 最小化对现有代码的改动

## 3. 架构设计

### 3.1 整体架构

```
┌─────────────────────────────────────────────────────┐
│                    LiteWindow                        │
│  ┌───────────────────────────────────────────────┐  │
│  │              rootContainer_                    │  │
│  │  ┌─────────────────────────────────────────┐  │  │
│  │  │           正常控件树                      │  │  │
│  │  │  Button, Input, Label, ComboBox...      │  │  │
│  │  └─────────────────────────────────────────┘  │  │
│  └───────────────────────────────────────────────┘  │
│  ┌───────────────────────────────────────────────┐  │
│  │              overlays_ 栈                      │  │
│  │  [0] ComboBox Dropdown                        │  │
│  │  [1] Context Menu                             │  │
│  │  [2] Dialog                                   │  │
│  │  [3] MessageBox  ← 最顶层                     │  │
│  └───────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────┘
```

### 3.2 事件分发流程

```
MouseEvent
    │
    ▼
┌─────────────────────┐
│ overlays_ 非空？     │
└─────────────────────┘
    │ Yes              │ No
    ▼                  ▼
┌─────────────────┐  ┌─────────────────┐
│ 分发给顶层       │  │ 分发给           │
│ overlay 处理     │  │ rootContainer_  │
└─────────────────┘  └─────────────────┘
    │
    ▼
┌─────────────────────┐
│ overlay 是否消费？   │
└─────────────────────┘
    │ No（点击外部）
    ▼
┌─────────────────────┐
│ 关闭 overlay        │
│ 可选：传递给下层     │
└─────────────────────┘
```

### 3.3 渲染流程

```
Render()
    │
    ├─► rootContainer_->renderTree(canvas)
    │
    └─► for each overlay in overlays_:
            overlay->renderTree(canvas)  // 后渲染的在上层
```

## 4. 接口设计

### 4.1 LiteWindow 新增接口

```cpp
// include/lite_window.h

class LiteWindow {
public:
    // ... 现有接口 ...
    
    // Overlay 管理
    void pushOverlay(LiteContainerPtr overlay);
    void popOverlay();
    void removeOverlay(LiteContainerPtr overlay);
    void clearOverlays();
    bool hasOverlay() const;
    LiteContainerPtr getTopOverlay() const;
    
private:
    std::vector<LiteContainerPtr> overlays_;
};
```

### 4.2 LiteContainer 新增接口

```cpp
// include/lite_container.h

class LiteContainer {
public:
    // ... 现有接口 ...
    
    // 获取所属窗口（用于 overlay 操作）
    LiteWindow* getWindow() const;
    void setWindow(LiteWindow* window);
    
    // 获取绝对坐标（相对于窗口）
    float getAbsoluteLeft() const;
    float getAbsoluteTop() const;
    
protected:
    LiteWindow* m_window = nullptr;
};
```

### 4.3 Overlay 配置结构

```cpp
// include/lite_overlay.h

struct OverlayConfig {
    bool modal = false;              // 是否模态（显示遮罩）
    bool closeOnClickOutside = true; // 点击外部是否关闭
    Color modalColor = Color::fromRGB(0, 0, 0, 128);  // 遮罩颜色
};

// 可选：Overlay 基类
class LiteOverlay : public LiteContainer {
public:
    void show(LiteWindow* window);
    void close();
    
    void setConfig(const OverlayConfig& config);
    const OverlayConfig& getConfig() const;
    
    // 点击 overlay 内容区域外时调用
    virtual void onClickOutside();
    
protected:
    OverlayConfig m_config;
    SkRect m_contentRect;  // 内容区域，用于判断点击位置
};
```

## 5. 实现细节

### 5.1 LiteWindow 实现

```cpp
// src/window/lite_window.cpp

void LiteWindow::pushOverlay(LiteContainerPtr overlay) {
    if (!overlay) return;
    overlay->setWindow(this);
    overlay->setWidth(LayoutValue::Point(static_cast<float>(width_)));
    overlay->setHeight(LayoutValue::Point(static_cast<float>(height_)));
    overlay->calculateLayout(static_cast<float>(width_), static_cast<float>(height_));
    overlays_.push_back(overlay);
    overlay->markDirty();
}

void LiteWindow::popOverlay() {
    if (!overlays_.empty()) {
        overlays_.back()->setWindow(nullptr);
        overlays_.pop_back();
        if (rootContainer_) rootContainer_->markDirty();
    }
}

void LiteWindow::removeOverlay(LiteContainerPtr overlay) {
    auto it = std::find(overlays_.begin(), overlays_.end(), overlay);
    if (it != overlays_.end()) {
        (*it)->setWindow(nullptr);
        overlays_.erase(it);
        if (rootContainer_) rootContainer_->markDirty();
    }
}

// 修改 Render 方法
void LiteWindow::Render() {
    // ... 现有逻辑 ...
    
    if (rootContainer_ && skiaRenderer_) {
        // 检查是否需要重绘
        bool needsRender = rootContainer_->isDirty();
        for (auto& overlay : overlays_) {
            if (overlay->isDirty()) needsRender = true;
        }
        
        if (needsRender) {
            skiaRenderer_->begin();
            SkCanvas* canvas = skiaRenderer_->getCanvas();
            if (canvas) {
                // 1. 渲染主控件树
                rootContainer_->renderTree(canvas);
                
                // 2. 渲染 overlay 层（按顺序，后面的在上层）
                for (auto& overlay : overlays_) {
                    overlay->renderTree(canvas);
                }
            }
            skiaRenderer_->end();
            
            rootContainer_->clearDirtyTree();
            for (auto& overlay : overlays_) {
                overlay->clearDirtyTree();
            }
        }
    }
}
```

### 5.2 事件分发修改

```cpp
// src/window/lite_window.cpp

static void dispatchMouseEvent(LiteWindow* win, ...) {
    // 1. 优先处理 overlay 层
    if (win->hasOverlay()) {
        auto topOverlay = win->getTopOverlay();
        
        // 检查是否点击在 overlay 内容区域
        float subx, suby;
        auto target = findDeepestContainerAtPosition(
            topOverlay.get(), event.x, event.y, subx, suby);
        
        if (target) {
            // 点击在 overlay 内，正常分发
            event.x = subx;
            event.y = suby;
            if (event.pressed) target->onMousePressed(event);
            // ...
        } else if (event.pressed) {
            // 点击在 overlay 外
            // 检查 overlay 配置决定是否关闭
            topOverlay->onClickOutside();
        }
        return;  // overlay 消费事件，不传递给下层
    }
    
    // 2. 正常的控件树分发
    // ... 现有逻辑 ...
}
```

### 5.3 ComboBox 改造

```cpp
// include/lite_combo_box.h

class LiteComboBox : public LiteContainer {
    // 下拉列表作为独立的 overlay 容器
    class DropdownOverlay : public LiteContainer {
    public:
        DropdownOverlay(LiteComboBox* comboBox);
        void render(SkCanvas* canvas) override;
        void onMousePressed(const MouseEvent& event) override;
        void onMouseMoved(const MouseEvent& event) override;
        void onClickOutside();
        
    private:
        LiteComboBox* m_comboBox;
        SkRect m_dropdownRect;  // 下拉列表的实际区域
    };
    
    std::shared_ptr<DropdownOverlay> m_dropdownOverlay;
    
public:
    void showDropdown();
    void hideDropdown();
};

// src/controls/lite_combo_box.cpp

void LiteComboBox::showDropdown() {
    if (!m_dropdownOverlay) {
        m_dropdownOverlay = std::make_shared<DropdownOverlay>(this);
    }
    
    auto window = getWindow();
    if (window) {
        window->pushOverlay(m_dropdownOverlay);
        m_isPopupShown = true;
        markDirty();
    }
}

void LiteComboBox::hideDropdown() {
    auto window = getWindow();
    if (window && m_dropdownOverlay) {
        window->removeOverlay(m_dropdownOverlay);
        m_isPopupShown = false;
        markDirty();
    }
}

// DropdownOverlay 实现
void LiteComboBox::DropdownOverlay::render(SkCanvas* canvas) {
    // 计算下拉列表位置（基于 ComboBox 的绝对位置）
    float comboX = m_comboBox->getAbsoluteLeft();
    float comboY = m_comboBox->getAbsoluteTop();
    float comboW = m_comboBox->getLayoutWidth();
    float comboH = m_comboBox->getLayoutHeight();
    
    float dropdownX = comboX;
    float dropdownY = comboY + comboH;
    float dropdownW = comboW;
    float dropdownH = m_comboBox->getDropdownHeight();
    
    m_dropdownRect = SkRect::MakeXYWH(dropdownX, dropdownY, dropdownW, dropdownH);
    
    // 绘制下拉列表背景
    SkPaint bgPaint;
    bgPaint.setColor(m_comboBox->getDropdownColor().toARGB());
    canvas->drawRect(m_dropdownRect, bgPaint);
    
    // 绘制选项...
}

void LiteComboBox::DropdownOverlay::onMousePressed(const MouseEvent& event) {
    if (m_dropdownRect.contains(event.x, event.y)) {
        // 计算点击的选项索引
        int index = static_cast<int>((event.y - m_dropdownRect.top()) / ITEM_HEIGHT);
        m_comboBox->setSelectedIndex(index);
    }
    m_comboBox->hideDropdown();
}

void LiteComboBox::DropdownOverlay::onClickOutside() {
    m_comboBox->hideDropdown();
}
```

## 6. 迁移计划

### 阶段 1：基础设施（必需）

1. LiteContainer 添加 `getWindow()/setWindow()` 和 `getAbsoluteLeft()/getAbsoluteTop()`
2. LiteWindow 添加 overlay 管理接口
3. 修改 LiteWindow::Render() 支持 overlay 渲染
4. 修改事件分发逻辑支持 overlay

### 阶段 2：ComboBox 改造（必需）

1. 创建 DropdownOverlay 内部类
2. 修改 showDropdown/hideDropdown 使用 overlay 机制
3. 移除原有的边界外绘制逻辑

### 阶段 3：Menu 改造（必需）

1. 类似 ComboBox 的改造方式
2. 支持子菜单的嵌套 overlay

### 阶段 4：Dialog/MessageBox 改造（可选）

1. 现有实现可以继续工作
2. 可选择迁移到 overlay 机制以统一管理

## 7. 兼容性考虑

1. **向后兼容**：现有的 Dialog/MessageBox 使用方式不变
2. **渐进迁移**：可以逐步迁移各个控件
3. **API 稳定**：对外接口保持不变，内部实现改造

## 8. 测试要点

1. ComboBox 下拉列表点击选择
2. Menu 弹出和选择
3. Dialog 中嵌套 ComboBox
4. 多个 overlay 同时存在的层级
5. 点击 overlay 外部关闭
6. 窗口大小变化时 overlay 位置更新
