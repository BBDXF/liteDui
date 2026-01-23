/**
 * lite_tab_view.h - 标签页控件
 */

#pragma once

#include "lite_container.h"
#include <vector>

namespace liteDui {

class LiteTabView;
using LiteTabViewPtr = std::shared_ptr<LiteTabView>;

/**
 * 标签页项结构体
 */
struct TabItem {
    std::string title;
    LiteContainerPtr content;
    bool enabled = true;

    TabItem() = default;
    TabItem(const std::string& t, LiteContainerPtr c = nullptr)
        : title(t), content(c) {}
};

/**
 * 标签位置枚举
 */
enum class TabPosition {
    Top,
    Bottom
};

/**
 * LiteTabView - 标签页控件
 * 
 * 支持多个标签页切换显示不同内容
 */
class LiteTabView : public LiteContainer {
public:
    LiteTabView();
    ~LiteTabView() override = default;

    // 标签页管理
    void addTab(const std::string& title, LiteContainerPtr content = nullptr);
    void insertTab(size_t index, const std::string& title, LiteContainerPtr content = nullptr);
    void removeTab(size_t index);
    void clearTabs();
    size_t getTabCount() const { return m_tabs.size(); }
    TabItem* getTab(size_t index);
    const TabItem* getTab(size_t index) const;

    // 设置标签页内容
    void setTabContent(size_t index, LiteContainerPtr content);
    LiteContainerPtr getTabContent(size_t index) const;

    // 当前选中的标签
    void setSelectedIndex(int index);
    int getSelectedIndex() const { return m_selectedIndex; }

    // 标签位置
    void setTabPosition(TabPosition position);
    TabPosition getTabPosition() const { return m_tabPosition; }

    // 样式设置
    void setTabHeight(float height);
    float getTabHeight() const { return m_tabHeight; }

    void setTabPadding(float padding) { m_tabPadding = padding; markDirty(); }
    float getTabPadding() const { return m_tabPadding; }

    void setTabGap(float gap) { m_tabGap = gap; markDirty(); }
    float getTabGap() const { return m_tabGap; }

    void setSelectedTabColor(const Color& color) { m_selectedTabColor = color; markDirty(); }
    Color getSelectedTabColor() const { return m_selectedTabColor; }

    void setNormalTabColor(const Color& color) { m_normalTabColor = color; markDirty(); }
    Color getNormalTabColor() const { return m_normalTabColor; }

    void setHoverTabColor(const Color& color) { m_hoverTabColor = color; markDirty(); }
    Color getHoverTabColor() const { return m_hoverTabColor; }

    void setSelectedTextColor(const Color& color) { m_selectedTextColor = color; markDirty(); }
    void setNormalTextColor(const Color& color) { m_normalTextColor = color; markDirty(); }

    void setTabBorderColor(const Color& color) { m_tabBorderColor = color; markDirty(); }
    void setContentBorderColor(const Color& color) { m_contentBorderColor = color; markDirty(); }

    // 回调
    void setOnTabChanged(SelectionChangedCallback callback);

    // 重写渲染
    void render(SkCanvas* canvas) override;

    // 事件处理
    void onMousePressed(const MouseEvent& event) override;
    void onMouseMoved(const MouseEvent& event) override;
    void onMouseExited(const MouseEvent& event) override;

private:
    int getTabIndexAtX(float x) const;
    void drawTabs(SkCanvas* canvas);
    void drawTab(SkCanvas* canvas, size_t index, float x, float y, float width, float height);
    void drawContent(SkCanvas* canvas);
    float measureTabWidth(const std::string& title) const;
    float getContentTop() const;
    float getContentHeight() const;

    std::vector<TabItem> m_tabs;
    int m_selectedIndex = 0;
    int m_hoverIndex = -1;
    TabPosition m_tabPosition = TabPosition::Top;

    // 样式
    float m_tabHeight = 36.0f;
    float m_tabPadding = 16.0f;
    float m_tabGap = 2.0f;
    float m_tabRadius = 4.0f;

    Color m_selectedTabColor = Color::White();
    Color m_normalTabColor = Color::fromRGB(240, 240, 240);
    Color m_hoverTabColor = Color::fromRGB(230, 230, 230);
    Color m_selectedTextColor = Color::fromRGB(66, 133, 244);
    Color m_normalTextColor = Color::fromRGB(100, 100, 100);
    Color m_tabBorderColor = Color::fromRGB(200, 200, 200);
    Color m_contentBorderColor = Color::fromRGB(200, 200, 200);

    SelectionChangedCallback m_onTabChanged;
};

} // namespace liteDui
