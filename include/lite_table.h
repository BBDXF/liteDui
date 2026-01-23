/**
 * lite_table.h - 表格控件
 */

#pragma once

#include "lite_scroll_view.h"
#include <vector>

namespace liteDui {

class LiteTable;
using LiteTablePtr = std::shared_ptr<LiteTable>;

/**
 * 表格列定义
 */
struct TableColumn {
    std::string title;
    float width = 100.0f;
    TextAlign align = TextAlign::Left;
    bool resizable = true;

    TableColumn() = default;
    TableColumn(const std::string& t, float w = 100.0f, TextAlign a = TextAlign::Left)
        : title(t), width(w), align(a) {}
};

/**
 * 表格单元格
 */
struct TableCell {
    std::string text;
    Color textColor = Color::Black();
    Color backgroundColor = Color::Transparent();
    void* userData = nullptr;

    TableCell() = default;
    TableCell(const std::string& t) : text(t) {}
};

/**
 * 表格行
 */
struct TableRow {
    std::vector<TableCell> cells;
    bool selected = false;
    void* userData = nullptr;

    TableRow() = default;
    TableRow(const std::vector<std::string>& texts) {
        for (const auto& t : texts) {
            cells.emplace_back(t);
        }
    }
};

/**
 * LiteTable - 表格控件
 * 
 * 支持多列显示、行选择、表头固定、可滚动
 */
class LiteTable : public LiteScrollView {
public:
    LiteTable();
    ~LiteTable() override = default;

    // 列管理
    void addColumn(const std::string& title, float width = 100.0f, TextAlign align = TextAlign::Left);
    void insertColumn(size_t index, const std::string& title, float width = 100.0f);
    void removeColumn(size_t index);
    void clearColumns();
    size_t getColumnCount() const { return m_columns.size(); }
    TableColumn* getColumn(size_t index);
    const TableColumn* getColumn(size_t index) const;
    void setColumnWidth(size_t index, float width);

    // 行管理
    void addRow(const std::vector<std::string>& cells);
    void insertRow(size_t index, const std::vector<std::string>& cells);
    void removeRow(size_t index);
    void clearRows();
    size_t getRowCount() const { return m_rows.size(); }
    TableRow* getRow(size_t index);
    const TableRow* getRow(size_t index) const;

    // 单元格操作
    void setCellText(size_t row, size_t col, const std::string& text);
    std::string getCellText(size_t row, size_t col) const;
    void setCellColor(size_t row, size_t col, const Color& textColor, const Color& bgColor = Color::Transparent());

    // 选择
    void setSelectionMode(ListSelectionMode mode);
    ListSelectionMode getSelectionMode() const { return m_selectionMode; }
    void setSelectedRow(int index);
    int getSelectedRow() const;
    std::vector<int> getSelectedRows() const;
    void clearSelection();

    // 样式设置
    void setHeaderHeight(float height) { m_headerHeight = height; markDirty(); }
    float getHeaderHeight() const { return m_headerHeight; }

    void setRowHeight(float height) { m_rowHeight = height; markDirty(); }
    float getRowHeight() const { return m_rowHeight; }

    void setHeaderBackgroundColor(const Color& color) { m_headerBgColor = color; markDirty(); }
    void setHeaderTextColor(const Color& color) { m_headerTextColor = color; markDirty(); }

    void setSelectedRowColor(const Color& color) { m_selectedRowColor = color; markDirty(); }
    void setHoverRowColor(const Color& color) { m_hoverRowColor = color; markDirty(); }
    void setAlternateRowColor(const Color& color) { m_alternateRowColor = color; markDirty(); }

    void setGridColor(const Color& color) { m_gridColor = color; markDirty(); }
    void setShowGrid(bool show) { m_showGrid = show; markDirty(); }
    void setShowHeader(bool show) { m_showHeader = show; markDirty(); }
    void setShowAlternateRows(bool show) { m_showAlternateRows = show; markDirty(); }

    // 回调
    void setOnSelectionChanged(SelectionChangedCallback callback);
    void setOnRowClicked(std::function<void(int)> callback);
    void setOnRowDoubleClicked(std::function<void(int)> callback);
    void setOnCellClicked(std::function<void(int, int)> callback);

    // 重写渲染
    void render(SkCanvas* canvas) override;

    // 重写获取内容高度
    float getContentHeight() const override;

    // 事件处理
    void onMousePressed(const MouseEvent& event) override;
    void onMouseMoved(const MouseEvent& event) override;
    void onMouseExited(const MouseEvent& event) override;

protected:
    void renderContent(SkCanvas* canvas) override;

private:
    int getRowIndexAtY(float y) const;
    int getColumnIndexAtX(float x) const;
    float getTotalColumnWidth() const;
    void drawHeader(SkCanvas* canvas);
    void drawRow(SkCanvas* canvas, size_t index, float y);
    void drawCell(SkCanvas* canvas, const TableCell& cell, float x, float y, float width, float height, TextAlign align);
    void drawGrid(SkCanvas* canvas);

    std::vector<TableColumn> m_columns;
    std::vector<TableRow> m_rows;
    ListSelectionMode m_selectionMode = ListSelectionMode::Single;
    int m_hoverRow = -1;

    // 样式
    float m_headerHeight = 36.0f;
    float m_rowHeight = 32.0f;
    float m_cellPadding = 8.0f;

    Color m_headerBgColor = Color::fromRGB(245, 245, 245);
    Color m_headerTextColor = Color::fromRGB(33, 33, 33);
    Color m_selectedRowColor = Color::fromRGB(66, 133, 244, 80);
    Color m_hoverRowColor = Color::fromRGB(200, 200, 200, 100);
    Color m_alternateRowColor = Color::fromRGB(250, 250, 250);
    Color m_gridColor = Color::fromRGB(220, 220, 220);

    bool m_showGrid = true;
    bool m_showHeader = true;
    bool m_showAlternateRows = true;

    // 回调
    SelectionChangedCallback m_onSelectionChanged;
    std::function<void(int)> m_onRowClicked;
    std::function<void(int)> m_onRowDoubleClicked;
    std::function<void(int, int)> m_onCellClicked;
};

} // namespace liteDui
