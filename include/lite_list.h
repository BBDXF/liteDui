/**
 * lite_list.h - 列表控件
 * 
 * 基于 LiteScrollView 实现的可滚动列表控件
 */

#pragma once

#include "lite_scroll_view.h"
#include <vector>

namespace liteDui {

class LiteList;
using LiteListPtr = std::shared_ptr<LiteList>;

/**
 * 列表项结构体
 */
struct ListItem {
    std::string text;
    std::string id;
    bool selected = false;
    void* userData = nullptr;

    ListItem() = default;
    ListItem(const std::string& t, const std::string& i = "")
        : text(t), id(i) {}
};

/**
 * LiteList - 列表控件
 * 
 * 基于 LiteScrollView 实现的可滚动列表，支持单选/多选。
 * 继承 LiteScrollView 的 canvas 裁剪和滚动功能，专注于列表项的渲染和交互。
 */
class LiteList : public LiteScrollView {
public:
    LiteList();
    ~LiteList() override = default;

    // 列表项管理
    void addItem(const std::string& text, const std::string& id = "");
    void insertItem(size_t index, const std::string& text, const std::string& id = "");
    void removeItem(size_t index);
    void clearItems();
    size_t getItemCount() const { return m_items.size(); }
    ListItem* getItem(size_t index);
    const ListItem* getItem(size_t index) const;

    // 选择模式
    void setSelectionMode(ListSelectionMode mode);
    ListSelectionMode getSelectionMode() const { return m_selectionMode; }

    // 选择操作
    void setSelectedIndex(int index);
    int getSelectedIndex() const;
    std::vector<int> getSelectedIndices() const;
    void clearSelection();
    void selectAll();

    // 样式设置
    void setItemHeight(float height);
    float getItemHeight() const { return m_itemHeight; }
    
    void setItemPadding(float padding) { m_itemPadding = padding; markDirty(); }
    float getItemPadding() const { return m_itemPadding; }

    void setSelectedColor(const Color& color) { m_selectedColor = color; markDirty(); }
    Color getSelectedColor() const { return m_selectedColor; }

    void setHoverColor(const Color& color) { m_hoverColor = color; markDirty(); }
    Color getHoverColor() const { return m_hoverColor; }

    void setAlternateColor(const Color& color) { m_alternateColor = color; markDirty(); }
    Color getAlternateColor() const { return m_alternateColor; }

    void setShowAlternateRows(bool show) { m_showAlternateRows = show; markDirty(); }
    bool isShowAlternateRows() const { return m_showAlternateRows; }

    // 回调
    void setOnSelectionChanged(SelectionChangedCallback callback);
    void setOnItemClicked(std::function<void(int)> callback);
    void setOnItemDoubleClicked(std::function<void(int)> callback);

    // 事件处理
    void onMousePressed(const MouseEvent& event) override;
    void onMouseMoved(const MouseEvent& event) override;
    void onMouseExited(const MouseEvent& event) override;

    // 重写获取内容高度（列表的内容高度 = 项目数 × 项目高度）
    float getContentHeight() const override;

protected:
    /**
     * 重写内容渲染方法
     * 在 LiteScrollView 已经设置好裁剪和滚动偏移的 canvas 上渲染列表项
     */
    void renderContent(SkCanvas* canvas) override;

private:
    /**
     * 根据 Y 坐标获取对应的列表项索引
     * @param y 相对于内容区域的 Y 坐标（已考虑滚动偏移）
     */
    int getItemIndexAtY(float y) const;
    
    /**
     * 绘制单个列表项
     * @param canvas 画布
     * @param index 项目索引
     * @param y 项目的 Y 坐标
     * @param width 项目宽度
     */
    void drawItem(SkCanvas* canvas, size_t index, float y, float width);

    std::vector<ListItem> m_items;
    ListSelectionMode m_selectionMode = ListSelectionMode::Single;
    int m_hoverIndex = -1;
    
    // 样式
    float m_itemHeight = 36.0f;
    float m_itemPadding = 12.0f;
    Color m_selectedColor = Color::fromRGB(66, 133, 244, 80);
    Color m_hoverColor = Color::fromRGB(200, 200, 200, 100);
    Color m_alternateColor = Color::fromRGB(245, 245, 245);
    bool m_showAlternateRows = false;

    // 回调
    SelectionChangedCallback m_onSelectionChanged;
    std::function<void(int)> m_onItemClicked;
    std::function<void(int)> m_onItemDoubleClicked;
};

} // namespace liteDui
