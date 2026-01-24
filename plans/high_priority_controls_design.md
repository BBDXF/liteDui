# 高优先级控件技术设计文档

## 项目概述

本文档详细描述 liteDui 项目中 8 个高优先级缺失控件的技术设计方案。

**设计原则：**
- 遵循现有代码风格和命名规范
- 继承自 LiteContainer 基类
- 使用 Skia 进行渲染
- 支持事件回调机制
- 支持样式定制

---

## 1. LiteLabel（文本标签）

### 1.1 控件描述
独立的文本标签控件，用于显示静态文本、图标或超链接。

### 1.2 头文件设计

```cpp
// include/lite_label.h
#pragma once
#include "lite_container.h"

namespace liteDui {

class LiteLabel;
using LiteLabelPtr = std::shared_ptr<LiteLabel>;

enum class LabelDisplayMode {
    SingleLine,
    MultiLine,
    Ellipsis
};

class LiteLabel : public LiteContainer {
public:
    LiteLabel();
    explicit LiteLabel(const std::string& text);
    ~LiteLabel() override = default;

    void setDisplayMode(LabelDisplayMode mode);
    LabelDisplayMode getDisplayMode() const { return m_displayMode; }
    
    void setVerticalAlign(Align align);
    void setWordWrap(bool wrap);
    void setMaxLines(int maxLines);
    void setUrl(const std::string& url);
    void setLinkColor(const Color& color);
    void setOnClick(MouseEventCallback callback);
    
    void render(SkCanvas* canvas) override;
    void onMousePressed(const MouseEvent& event) override;
    void onMouseEntered(const MouseEvent& event) override;
    void onMouseExited(const MouseEvent& event) override;

private:
    LabelDisplayMode m_displayMode = LabelDisplayMode::SingleLine;
    Align m_verticalAlign = Align::Center;
    bool m_wordWrap = false;
    int m_maxLines = 0;
    std::string m_url;
    Color m_linkColor = Color::fromRGB(66, 133, 244);
    bool m_isHovered = false;
    MouseEventCallback m_onClick;
};

} // namespace liteDui
```

### 1.3 文件结构
- include/lite_label.h
- src/controls/lite_label.cpp

---

## 2. LiteProgressBar（进度条）

### 2.1 控件描述
显示任务进度的控件，支持确定和不确定模式。

### 2.2 头文件设计

```cpp
// include/lite_progress_bar.h
#pragma once
#include "lite_container.h"

namespace liteDui {

class LiteProgressBar;
using LiteProgressBarPtr = std::shared_ptr<LiteProgressBar>;

enum class ProgressBarOrientation { Horizontal, Vertical };
enum class ProgressBarMode { Determinate, Indeterminate };

class LiteProgressBar : public LiteContainer {
public:
    LiteProgressBar();
    ~LiteProgressBar() override = default;

    void setValue(float value);
    float getValue() const { return m_value; }
    void setMinimum(float min);
    void setMaximum(float max);
    void setMode(ProgressBarMode mode);
    void setOrientation(ProgressBarOrientation orientation);
    void setProgressColor(const Color& color);
    void setShowText(bool show);
    void setTextFormat(const std::string& format);
    
    void render(SkCanvas* canvas) override;
    void update() override;

private:
    float m_value = 0.0f;
    float m_minimum = 0.0f;
    float m_maximum = 100.0f;
    ProgressBarMode m_mode = ProgressBarMode::Determinate;
    ProgressBarOrientation m_orientation = ProgressBarOrientation::Horizontal;
    Color m_progressColor = Color::fromRGB(66, 133, 244);
    bool m_showText = false;
    std::string m_textFormat = "%p%";
    float m_animationOffset = 0.0f;
};

} // namespace liteDui
```

### 2.3 文件结构
- include/lite_progress_bar.h
- src/controls/lite_progress_bar.cpp

---

## 3. LiteSlider（滑块）

### 3.1 控件描述
用于数值范围选择的滑块控件。

### 3.2 头文件设计

```cpp
// include/lite_slider.h
#pragma once
#include "lite_container.h"

namespace liteDui {

class LiteSlider;
using LiteSliderPtr = std::shared_ptr<LiteSlider>;

enum class SliderOrientation { Horizontal, Vertical };
enum class SliderTickPosition { None, Above, Below, Both };

class LiteSlider : public LiteContainer {
public:
    LiteSlider();
    ~LiteSlider() override = default;

    void setValue(float value);
    float getValue() const { return m_value; }
    void setMinimum(float min);
    void setMaximum(float max);
    void setStep(float step);
    void setOrientation(SliderOrientation orientation);
    void setTickPosition(SliderTickPosition position);
    void setTickInterval(float interval);
    void setTrackColor(const Color& color);
    void setFillColor(const Color& color);
    void setThumbColor(const Color& color);
    void setThumbSize(float size);
    
    void setOnValueChanged(std::function<void(float)> callback);
    
    void render(SkCanvas* canvas) override;
    void onMousePressed(const MouseEvent& event) override;
    void onMouseReleased(const MouseEvent& event) override;
    void onMouseMoved(const MouseEvent& event) override;

private:
    float valueToPosition(float value) const;
    float positionToValue(float pos) const;
    
    float m_value = 0.0f;
    float m_minimum = 0.0f;
    float m_maximum = 100.0f;
    float m_step = 1.0f;
    SliderOrientation m_orientation = SliderOrientation::Horizontal;
    SliderTickPosition m_tickPosition = SliderTickPosition::None;
    float m_tickInterval = 10.0f;
    Color m_trackColor = Color::fromRGB(200, 200, 200);
    Color m_fillColor = Color::fromRGB(66, 133, 244);
    Color m_thumbColor = Color::White();
    float m_thumbSize = 16.0f;
    bool m_isDragging = false;
    std::function<void(float)> m_onValueChanged;
};

} // namespace liteDui
```

### 3.3 文件结构
- include/lite_slider.h
- src/controls/lite_slider.cpp

---

## 4. LiteComboBox（下拉组合框）

### 4.1 控件描述
下拉选择框，支持可编辑和只读模式。

### 4.2 头文件设计

```cpp
// include/lite_combo_box.h
#pragma once
#include "lite_container.h"

namespace liteDui {

class LiteComboBox;
using LiteComboBoxPtr = std::shared_ptr<LiteComboBox>;

enum class ComboBoxMode { ReadOnly, Editable };

class LiteComboBox : public LiteContainer {
public:
    LiteComboBox();
    ~LiteComboBox() override = default;

    void addItem(const std::string& text, const std::string& data = "");
    void insertItem(size_t index, const std::string& text, const std::string& data = "");
    void removeItem(size_t index);
    void clearItems();
    size_t getItemCount() const { return m_items.size(); }
    
    void setSelectedIndex(int index);
    int getSelectedIndex() const { return m_selectedIndex; }
    std::string getSelectedText() const;
    
    void setMode(ComboBoxMode mode);
    void setPlaceholder(const std::string& placeholder);
    void setMaxVisibleItems(int max);
    void setDropdownColor(const Color& color);
    void setHoverColor(const Color& color);
    
    void setOnSelectionChanged(std::function<void(int)> callback);
    
    void render(SkCanvas* canvas) override;
    void onMousePressed(const MouseEvent& event) override;
    void onMouseMoved(const MouseEvent& event) override;
    void onFocusLost() override;

private:
    struct ComboItem { std::string text; std::string data; };
    std::vector<ComboItem> m_items;
    int m_selectedIndex = -1;
    ComboBoxMode m_mode = ComboBoxMode::ReadOnly;
    std::string m_placeholder;
    int m_maxVisibleItems = 8;
    bool m_isPopupShown = false;
    int m_hoverIndex = -1;
    Color m_dropdownColor = Color::White();
    Color m_hoverColor = Color::fromRGB(230, 230, 230);
    std::function<void(int)> m_onSelectionChanged;
};

} // namespace liteDui
```

### 4.3 文件结构
- include/lite_combo_box.h
- src/controls/lite_combo_box.cpp

---

## 5. LiteTreeView（树形控件）

### 5.1 控件描述
用于显示层级数据的树形控件。

### 5.2 头文件设计

```cpp
// include/lite_tree_view.h
#pragma once
#include "lite_scroll_view.h"

namespace liteDui {

class LiteTreeView;
class TreeNode;
using LiteTreeViewPtr = std::shared_ptr<LiteTreeView>;

class TreeNode {
public:
    TreeNode();
    explicit TreeNode(const std::string& text);
    ~TreeNode();
    
    void setText(const std::string& text);
    const std::string& getText() const { return m_text; }
    void setData(void* data);
    void* getData() const { return m_data; }
    void setExpanded(bool expanded);
    bool isExpanded() const { return m_expanded; }
    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }
    
    void addChild(TreeNode* child);
    void removeChild(TreeNode* child);
    void clearChildren();
    size_t getChildCount() const { return m_children.size(); }
    TreeNode* getChildAt(size_t index) const;
    TreeNode* getParent() const { return m_parent; }
    bool hasChildren() const { return !m_children.empty(); }
    int getDepth() const;

private:
    std::string m_text;
    void* m_data = nullptr;
    bool m_expanded = false;
    bool m_selected = false;
    TreeNode* m_parent = nullptr;
    std::vector<TreeNode*> m_children;
};

enum class TreeSelectionMode { None, Single, Multiple };

class LiteTreeView : public LiteScrollView {
public:
    LiteTreeView();
    ~LiteTreeView() override;
    
    void setRootNode(TreeNode* root);
    TreeNode* getRootNode() const { return m_rootNode; }
    void setSelectionMode(TreeSelectionMode mode);
    void setShowRoot(bool show);
    void setShowLines(bool show);
    void setIndentation(float indent);
    void setItemHeight(float height);
    
    void expandAll();
    void collapseAll();
    void setSelectedNode(TreeNode* node);
    TreeNode* getSelectedNode() const;
    
    void setOnNodeSelected(std::function<void(TreeNode*)> callback);
    void setOnNodeExpanded(std::function<void(TreeNode*)> callback);
    void setOnNodeCollapsed(std::function<void(TreeNode*)> callback);
    
    void render(SkCanvas* canvas) override;
    float getContentHeight() const override;
    void onMousePressed(const MouseEvent& event) override;
    void onMouseMoved(const MouseEvent& event) override;

protected:
    void renderContent(SkCanvas* canvas) override;

private:
    TreeNode* m_rootNode = nullptr;
    TreeSelectionMode m_selectionMode = TreeSelectionMode::Single;
    bool m_showRoot = true;
    bool m_showLines = true;
    float m_indentation = 20.0f;
    float m_itemHeight = 28.0f;
    TreeNode* m_hoverNode = nullptr;
    std::function<void(TreeNode*)> m_onNodeSelected;
    std::function<void(TreeNode*)> m_onNodeExpanded;
    std::function<void(TreeNode*)> m_onNodeCollapsed;
};

} // namespace liteDui
```

### 5.3 文件结构
- include/lite_tree_view.h
- src/controls/lite_tree_view.cpp

---

## 6. LiteMenu / LiteMenuBar（菜单系统）

### 6.1 控件描述
应用程序菜单系统，包括菜单栏、弹出菜单和上下文菜单。

### 6.2 头文件设计

```cpp
// include/lite_menu.h
#pragma once
#include "lite_container.h"

namespace liteDui {

class LiteMenu;
class LiteMenuBar;
class LiteMenuItem;
using LiteMenuPtr = std::shared_ptr<LiteMenu>;
using LiteMenuBarPtr = std::shared_ptr<LiteMenuBar>;

enum class MenuItemType { Normal, Separator, Submenu, Checkable };

class LiteMenuItem {
public:
    LiteMenuItem();
    explicit LiteMenuItem(const std::string& text);
    ~LiteMenuItem();
    
    void setText(const std::string& text);
    const std::string& getText() const { return m_text; }
    void setType(MenuItemType type);
    void setChecked(bool checked);
    bool isChecked() const { return m_checked; }
    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }
    void setShortcut(const std::string& shortcut);
    void setSubmenu(LiteMenu* submenu);
    LiteMenu* getSubmenu() const { return m_submenu; }
    void setOnClick(std::function<void()> callback);

private:
    std::string m_text;
    MenuItemType m_type = MenuItemType::Normal;
    bool m_checked = false;
    bool m_enabled = true;
    std::string m_shortcut;
    LiteMenu* m_submenu = nullptr;
    std::function<void()> m_onClick;
    friend class LiteMenu;
};

class LiteMenu : public LiteContainer {
public:
    LiteMenu();
    ~LiteMenu() override;
    
    void addItem(const std::string& text, std::function<void()> callback = nullptr);
    void addSeparator();
    void addSubmenu(const std::string& text, LiteMenu* submenu);
    void removeItem(size_t index);
    void clearItems();
    size_t getItemCount() const { return m_items.size(); }
    
    void show(float x, float y);
    void hide();
    bool isShown() const { return m_isShown; }
    
    void setItemHeight(float height);
    void setBackgroundColor(const Color& color);
    void setSelectionColor(const Color& color);
    
    void render(SkCanvas* canvas) override;
    void onMousePressed(const MouseEvent& event) override;
    void onMouseMoved(const MouseEvent& event) override;

private:
    std::vector<LiteMenuItem*> m_items;
    bool m_isShown = false;
    int m_hoverIndex = -1;
    float m_itemHeight = 28.0f;
    Color m_bgColor = Color::White();
    Color m_selectionColor = Color::fromRGB(66, 133, 244, 80);
};

class LiteMenuBar : public LiteContainer {
public:
    LiteMenuBar();
    ~LiteMenuBar() override;
    
    void addMenu(const std::string& title, LiteMenu* menu);
    void removeMenu(size_t index);
    void clearMenus();
    
    void render(SkCanvas* canvas) override;
    void onMousePressed(const MouseEvent& event) override;
    void onMouseMoved(const MouseEvent& event) override;

private:
    struct MenuInfo { std::string title; LiteMenu* menu; };
    std::vector<MenuInfo> m_menus;
    int m_activeIndex = -1;
    int m_hoverIndex = -1;
};

} // namespace liteDui
```

### 6.3 文件结构
- include/lite_menu.h
- src/controls/lite_menu.cpp

---

## 7. LiteDialog（对话框基类）

### 7.1 控件描述
对话框基类，支持模态和非模态对话框。

### 7.2 头文件设计

```cpp
// include/lite_dialog.h
#pragma once
#include "lite_container.h"
#include "lite_button.h"

namespace liteDui {

class LiteDialog;
using LiteDialogPtr = std::shared_ptr<LiteDialog>;

enum class DialogCode { None, Accepted, Rejected };

enum StandardButton {
    NoButton = 0,
    Ok = 0x0001,
    Cancel = 0x0002,
    Yes = 0x0004,
    No = 0x0008,
    Close = 0x0010,
    Apply = 0x0020
};

class LiteDialog : public LiteContainer {
public:
    LiteDialog();
    ~LiteDialog() override;
    
    void setTitle(const std::string& title);
    const std::string& getTitle() const { return m_title; }
    void setModal(bool modal);
    bool isModal() const { return m_isModal; }
    void setDialogSize(float width, float height);
    void setContent(LiteContainerPtr content);
    void setStandardButtons(int buttons);
    
    DialogCode exec();
    DialogCode getResult() const { return m_result; }
    void accept();
    void reject();
    
    void setOnAccepted(std::function<void()> callback);
    void setOnRejected(std::function<void()> callback);
    
    void render(SkCanvas* canvas) override;

protected:
    void drawTitleBar(SkCanvas* canvas, float x, float y, float w, float h);
    void drawButtons(SkCanvas* canvas, float x, float y, float w, float h);
    
    std::string m_title;
    bool m_isModal = true;
    float m_dialogWidth = 400.0f;
    float m_dialogHeight = 300.0f;
    LiteContainerPtr m_content;
    int m_standardButtons = Ok | Cancel;
    DialogCode m_result = DialogCode::None;
    std::function<void()> m_onAccepted;
    std::function<void()> m_onRejected;
};

} // namespace liteDui
```

### 7.3 文件结构
- include/lite_dialog.h
- src/controls/lite_dialog.cpp

---

## 8. LiteMessageBox（消息框）

### 8.1 控件描述
显示提示、警告、错误信息的标准消息框。

### 8.2 头文件设计

```cpp
// include/lite_message_box.h
#pragma once
#include "lite_dialog.h"

namespace liteDui {

class LiteMessageBox;
using LiteMessageBoxPtr = std::shared_ptr<LiteMessageBox>;

enum class MessageBoxIcon {
    None,
    Information,
    Warning,
    Error,
    Question
};

class LiteMessageBox : public LiteDialog {
public:
    LiteMessageBox();
    ~LiteMessageBox() override = default;
    
    void setIcon(MessageBoxIcon icon);
    MessageBoxIcon getIcon() const { return m_icon; }
    void setMessage(const std::string& message);
    const std::string& getMessage() const { return m_message; }
    void setDetailedText(const std::string& text);
    
    // 静态便捷方法
    static DialogCode information(const std::string& title, const std::string& message, int buttons = Ok);
    static DialogCode warning(const std::string& title, const std::string& message, int buttons = Ok);
    static DialogCode error(const std::string& title, const std::string& message, int buttons = Ok);
    static DialogCode question(const std::string& title, const std::string& message, int buttons = Yes | No);
    
    void render(SkCanvas* canvas) override;

private:
    void drawIcon(SkCanvas* canvas, float x, float y, float size);
    
    MessageBoxIcon m_icon = MessageBoxIcon::None;
    std::string m_message;
    std::string m_detailedText;
};

} // namespace liteDui
```

### 8.3 文件结构
- include/lite_message_box.h
- src/controls/lite_message_box.cpp

---

## 实施计划

### 阶段一：基础控件（建议顺序）

1. **LiteLabel** - 最简单，作为热身
2. **LiteProgressBar** - 简单的绘制逻辑
3. **LiteSlider** - 需要拖拽交互

### 阶段二：复杂控件

4. **LiteComboBox** - 需要弹出窗口逻辑
5. **LiteTreeView** - 递归结构，较复杂

### 阶段三：对话框系统

6. **LiteDialog** - 对话框基类
7. **LiteMessageBox** - 继承自 Dialog
8. **LiteMenu/LiteMenuBar** - 菜单系统

### 文件清单

需要创建的文件：
```
include/lite_label.h
include/lite_progress_bar.h
include/lite_slider.h
include/lite_combo_box.h
include/lite_tree_view.h
include/lite_menu.h
include/lite_dialog.h
include/lite_message_box.h

src/controls/lite_label.cpp
src/controls/lite_progress_bar.cpp
src/controls/lite_slider.cpp
src/controls/lite_combo_box.cpp
src/controls/lite_tree_view.cpp
src/controls/lite_menu.cpp
src/controls/lite_dialog.cpp
src/controls/lite_message_box.cpp
```

### CMakeLists.txt 更新

需要在 CMakeLists.txt 中添加新的源文件（使用 GLOB_RECURSE 已自动包含）。

---

*文档生成时间：2026-01-24*
