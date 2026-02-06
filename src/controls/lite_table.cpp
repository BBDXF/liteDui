/**
 * lite_table.cpp - 表格控件实现
 * 
 * 基于 LiteScrollView 实现，支持固定表头和双向滚动
 */

#include "lite_table.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "modules/skparagraph/include/ParagraphBuilder.h"
#include "modules/skparagraph/include/Paragraph.h"
#include <algorithm>

using namespace skia::textlayout;

namespace liteDui {

LiteTable::LiteTable() {
    setScrollDirection(ScrollDirection::Both);
    setBackgroundColor(Color::White());
    setBorderColor(Color::LightGray());
    setBorder(EdgeInsets::All(1.0f));
    setPadding(EdgeInsets::All(0));
}

// 列管理
void LiteTable::addColumn(const std::string& title, float width, TextAlign align) {
    m_columns.emplace_back(title, width, align);
    markDirty();
}

void LiteTable::insertColumn(size_t index, const std::string& title, float width) {
    if (index > m_columns.size()) {
        index = m_columns.size();
    }
    m_columns.insert(m_columns.begin() + index, TableColumn(title, width));
    markDirty();
}

void LiteTable::removeColumn(size_t index) {
    if (index >= m_columns.size()) return;
    m_columns.erase(m_columns.begin() + index);
    
    // 同时移除所有行中对应的单元格
    for (auto& row : m_rows) {
        if (index < row.cells.size()) {
            row.cells.erase(row.cells.begin() + index);
        }
    }
    markDirty();
}

void LiteTable::clearColumns() {
    m_columns.clear();
    m_rows.clear();
    markDirty();
}

TableColumn* LiteTable::getColumn(size_t index) {
    if (index >= m_columns.size()) return nullptr;
    return &m_columns[index];
}

const TableColumn* LiteTable::getColumn(size_t index) const {
    if (index >= m_columns.size()) return nullptr;
    return &m_columns[index];
}

void LiteTable::setColumnWidth(size_t index, float width) {
    if (index >= m_columns.size()) return;
    m_columns[index].width = width;
    markDirty();
}

// 行管理
void LiteTable::addRow(const std::vector<std::string>& cells) {
    TableRow row;
    for (size_t i = 0; i < m_columns.size(); ++i) {
        if (i < cells.size()) {
            row.cells.emplace_back(cells[i]);
        } else {
            row.cells.emplace_back("");
        }
    }
    m_rows.push_back(row);
    markDirty();
}

void LiteTable::insertRow(size_t index, const std::vector<std::string>& cells) {
    if (index > m_rows.size()) {
        index = m_rows.size();
    }
    TableRow row;
    for (size_t i = 0; i < m_columns.size(); ++i) {
        if (i < cells.size()) {
            row.cells.emplace_back(cells[i]);
        } else {
            row.cells.emplace_back("");
        }
    }
    m_rows.insert(m_rows.begin() + index, row);
    markDirty();
}

void LiteTable::removeRow(size_t index) {
    if (index >= m_rows.size()) return;
    m_rows.erase(m_rows.begin() + index);
    markDirty();
}

void LiteTable::clearRows() {
    m_rows.clear();
    m_hoverRow = -1;
    markDirty();
}

TableRow* LiteTable::getRow(size_t index) {
    if (index >= m_rows.size()) return nullptr;
    return &m_rows[index];
}

const TableRow* LiteTable::getRow(size_t index) const {
    if (index >= m_rows.size()) return nullptr;
    return &m_rows[index];
}

// 单元格操作
void LiteTable::setCellText(size_t row, size_t col, const std::string& text) {
    if (row >= m_rows.size() || col >= m_columns.size()) return;
    if (col >= m_rows[row].cells.size()) {
        m_rows[row].cells.resize(m_columns.size());
    }
    m_rows[row].cells[col].text = text;
    markDirty();
}

std::string LiteTable::getCellText(size_t row, size_t col) const {
    if (row >= m_rows.size() || col >= m_rows[row].cells.size()) return "";
    return m_rows[row].cells[col].text;
}

void LiteTable::setCellColor(size_t row, size_t col, const Color& textColor, const Color& bgColor) {
    if (row >= m_rows.size() || col >= m_columns.size()) return;
    if (col >= m_rows[row].cells.size()) {
        m_rows[row].cells.resize(m_columns.size());
    }
    m_rows[row].cells[col].textColor = textColor;
    m_rows[row].cells[col].backgroundColor = bgColor;
    markDirty();
}

// 选择
void LiteTable::setSelectionMode(ListSelectionMode mode) {
    if (m_selectionMode == mode) return;
    m_selectionMode = mode;
    
    if (mode == ListSelectionMode::Single) {
        bool foundSelected = false;
        for (auto& row : m_rows) {
            if (row.selected) {
                if (foundSelected) {
                    row.selected = false;
                } else {
                    foundSelected = true;
                }
            }
        }
    } else if (mode == ListSelectionMode::None) {
        clearSelection();
    }
    markDirty();
}

void LiteTable::setSelectedRow(int index) {
    if (m_selectionMode == ListSelectionMode::None) return;
    
    if (m_selectionMode == ListSelectionMode::Single) {
        for (auto& row : m_rows) {
            row.selected = false;
        }
    }
    
    if (index >= 0 && index < static_cast<int>(m_rows.size())) {
        m_rows[index].selected = true;
    }
    
    markDirty();
    
    if (m_onSelectionChanged) {
        m_onSelectionChanged(index);
    }
}

int LiteTable::getSelectedRow() const {
    for (size_t i = 0; i < m_rows.size(); ++i) {
        if (m_rows[i].selected) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

std::vector<int> LiteTable::getSelectedRows() const {
    std::vector<int> indices;
    for (size_t i = 0; i < m_rows.size(); ++i) {
        if (m_rows[i].selected) {
            indices.push_back(static_cast<int>(i));
        }
    }
    return indices;
}

void LiteTable::clearSelection() {
    for (auto& row : m_rows) {
        row.selected = false;
    }
    markDirty();
}

// 回调
void LiteTable::setOnSelectionChanged(SelectionChangedCallback callback) {
    m_onSelectionChanged = callback;
}

void LiteTable::setOnRowClicked(std::function<void(int)> callback) {
    m_onRowClicked = callback;
}

void LiteTable::setOnRowDoubleClicked(std::function<void(int)> callback) {
    m_onRowDoubleClicked = callback;
}

void LiteTable::setOnCellClicked(std::function<void(int, int)> callback) {
    m_onCellClicked = callback;
}

// 辅助方法
int LiteTable::getRowIndexAtY(float y) const {
    if (m_rows.empty() || m_rowHeight <= 0) return -1;
    
    // y 是相对于内容区域的坐标（已经考虑了滚动偏移）
    int index = static_cast<int>(y / m_rowHeight);
    
    if (index < 0 || index >= static_cast<int>(m_rows.size())) {
        return -1;
    }
    return index;
}

int LiteTable::getColumnIndexAtX(float x) const {
    if (m_columns.empty()) return -1;
    
    // x 是相对于内容区域的坐标（已经考虑了滚动偏移）
    float currentX = 0;
    
    for (size_t i = 0; i < m_columns.size(); ++i) {
        if (x >= currentX && x < currentX + m_columns[i].width) {
            return static_cast<int>(i);
        }
        currentX += m_columns[i].width;
    }
    return -1;
}

float LiteTable::getTotalColumnWidth() const {
    float total = 0;
    for (const auto& col : m_columns) {
        total += col.width;
    }
    return total;
}

float LiteTable::getContentWidth() const {
    return getTotalColumnWidth();
}

float LiteTable::getContentHeight() const {
    // 内容高度 = 行数 × 行高（不包括表头，表头是固定的）
    return m_rows.size() * m_rowHeight;
}

void LiteTable::render(SkCanvas* canvas) {
    // 只绘制背景和边框
    LiteScrollView::render(canvas);
}

void LiteTable::renderTree(SkCanvas* canvas) {
    if (!canvas) return;

    // 保存 canvas 状态并平移到控件位置
    canvas->save();
    canvas->translate(getLeft(), getTop());

    // 渲染背景和边框
    render(canvas);

    // 计算内容区域的位置和尺寸
    float borderL = getLayoutBorderLeft();
    float borderT = getLayoutBorderTop();
    float padL = getLayoutPaddingLeft();
    float padT = getLayoutPaddingTop();

    float contentX = borderL + padL;
    float contentY = borderT + padT;
    float viewportW = getViewportWidth();
    float viewportH = getViewportHeight();

    // 如果显示表头，绘制固定表头
    if (m_showHeader) {
        canvas->save();
        
        // 裁剪表头区域
        SkRect headerClip = SkRect::MakeXYWH(contentX, contentY, viewportW, m_headerHeight);
        canvas->clipRect(headerClip, SkClipOp::kIntersect, true);
        
        // 平移 canvas（只水平滚动，垂直不滚动）
        canvas->translate(contentX - m_scrollX, contentY);
        
        // 绘制表头
        drawHeader(canvas);
        
        canvas->restore();
        
        // 调整内容区域的起始位置
        contentY += m_headerHeight;
        viewportH -= m_headerHeight;
    }

    // 绘制表格内容（可滚动区域）
    canvas->save();
    
    // 裁剪内容区域
    SkRect contentClip = SkRect::MakeXYWH(contentX, contentY, viewportW, viewportH);
    canvas->clipRect(contentClip, SkClipOp::kIntersect, true);
    
    // 平移 canvas 以实现滚动效果
    canvas->translate(contentX - m_scrollX, contentY - m_scrollY);
    
    // 渲染内容
    renderContent(canvas);
    
    canvas->restore();

    // 绘制滚动条（在裁剪区域之外）
    if (m_showScrollbar) {
        drawScrollbar(canvas);
    }

    // 恢复最外层 canvas 状态
    canvas->restore();
}

void LiteTable::renderContent(SkCanvas* canvas) {
    if (m_rows.empty()) return;

    float viewportH = getViewportHeight() - (m_showHeader ? m_headerHeight : 0);
    
    // 计算可见范围
    int firstVisible = static_cast<int>(m_scrollY / m_rowHeight);
    int lastVisible = static_cast<int>((m_scrollY + viewportH) / m_rowHeight);
    
    firstVisible = std::max(0, firstVisible);
    lastVisible = std::min(static_cast<int>(m_rows.size()) - 1, lastVisible);

    // 绘制可见行
    for (int i = firstVisible; i <= lastVisible; ++i) {
        float rowY = i * m_rowHeight;
        drawRow(canvas, i, rowY);
    }

    // 绘制网格线
    if (m_showGrid) {
        drawGrid(canvas);
    }
}

void LiteTable::drawHeader(SkCanvas* canvas) {
    float totalWidth = getTotalColumnWidth();

    // 绘制表头背景
    SkPaint bgPaint;
    bgPaint.setColor(m_headerBgColor.toARGB());
    bgPaint.setStyle(SkPaint::kFill_Style);
    canvas->drawRect(SkRect::MakeXYWH(0, 0, totalWidth, m_headerHeight), bgPaint);

    // 绘制表头单元格
    float currentX = 0;
    for (size_t i = 0; i < m_columns.size(); ++i) {
        const auto& col = m_columns[i];
        
        // 绘制表头文本
        if (!col.title.empty()) {
            auto& fontMgr = getFontManager();
            auto fontCollection = fontMgr.getFontCollection();

            skia::textlayout::TextAlign skAlign;
            switch (col.align) {
                case TextAlign::Left: skAlign = skia::textlayout::TextAlign::kLeft; break;
                case TextAlign::Center: skAlign = skia::textlayout::TextAlign::kCenter; break;
                case TextAlign::Right: skAlign = skia::textlayout::TextAlign::kRight; break;
            }

            ParagraphStyle paraStyle;
            paraStyle.setTextAlign(skAlign);
            paraStyle.setMaxLines(1);

            auto textStyle = fontMgr.createTextStyle(m_headerTextColor, getFontSize(), getFontFamily());

            auto builder = ParagraphBuilder::make(paraStyle, fontCollection);
            builder->pushStyle(textStyle);
            builder->addText(col.title.c_str());

            auto paragraph = builder->Build();
            paragraph->layout(col.width - m_cellPadding * 2);

            float textHeight = paragraph->getHeight();
            float textY = (m_headerHeight - textHeight) / 2;

            paragraph->paint(canvas, currentX + m_cellPadding, textY);
        }

        currentX += col.width;
    }

    // 绘制表头底部边框
    SkPaint borderPaint;
    borderPaint.setColor(m_gridColor.toARGB());
    borderPaint.setStyle(SkPaint::kStroke_Style);
    borderPaint.setStrokeWidth(1.0f);
    canvas->drawLine(0, m_headerHeight, totalWidth, m_headerHeight, borderPaint);
}

void LiteTable::drawRow(SkCanvas* canvas, size_t index, float y) {
    if (index >= m_rows.size()) return;

    const TableRow& row = m_rows[index];
    float totalWidth = getTotalColumnWidth();

    // 绘制行背景
    SkPaint bgPaint;
    bgPaint.setStyle(SkPaint::kFill_Style);

    if (row.selected) {
        bgPaint.setColor(m_selectedRowColor.toARGB());
        canvas->drawRect(SkRect::MakeXYWH(0, y, totalWidth, m_rowHeight), bgPaint);
    } else if (static_cast<int>(index) == m_hoverRow) {
        bgPaint.setColor(m_hoverRowColor.toARGB());
        canvas->drawRect(SkRect::MakeXYWH(0, y, totalWidth, m_rowHeight), bgPaint);
    } else if (m_showAlternateRows && index % 2 == 1) {
        bgPaint.setColor(m_alternateRowColor.toARGB());
        canvas->drawRect(SkRect::MakeXYWH(0, y, totalWidth, m_rowHeight), bgPaint);
    }

    // 绘制单元格
    float currentX = 0;
    for (size_t i = 0; i < m_columns.size(); ++i) {
        const auto& col = m_columns[i];
        
        if (i < row.cells.size()) {
            drawCell(canvas, row.cells[i], currentX, y, col.width, m_rowHeight, col.align);
        }
        
        currentX += col.width;
    }
}

void LiteTable::drawCell(SkCanvas* canvas, const TableCell& cell, float x, float y, 
                         float width, float height, TextAlign align) {
    // 绘制单元格背景
    if (cell.backgroundColor.a > 0) {
        SkPaint bgPaint;
        bgPaint.setColor(cell.backgroundColor.toARGB());
        bgPaint.setStyle(SkPaint::kFill_Style);
        canvas->drawRect(SkRect::MakeXYWH(x, y, width, height), bgPaint);
    }

    // 绘制单元格文本
    if (!cell.text.empty()) {
        auto& fontMgr = getFontManager();
        auto fontCollection = fontMgr.getFontCollection();

        skia::textlayout::TextAlign skAlign;
        switch (align) {
            case TextAlign::Left: skAlign = skia::textlayout::TextAlign::kLeft; break;
            case TextAlign::Center: skAlign = skia::textlayout::TextAlign::kCenter; break;
            case TextAlign::Right: skAlign = skia::textlayout::TextAlign::kRight; break;
        }

        ParagraphStyle paraStyle;
        paraStyle.setTextAlign(skAlign);
        paraStyle.setMaxLines(1);

        Color textColor = (cell.textColor.a > 0) ? cell.textColor : getTextColor();
        auto textStyle = fontMgr.createTextStyle(textColor, getFontSize(), getFontFamily());

        auto builder = ParagraphBuilder::make(paraStyle, fontCollection);
        builder->pushStyle(textStyle);
        builder->addText(cell.text.c_str());

        auto paragraph = builder->Build();
        paragraph->layout(width - m_cellPadding * 2);

        float textHeight = paragraph->getHeight();
        float textY = y + (height - textHeight) / 2;

        paragraph->paint(canvas, x + m_cellPadding, textY);
    }
}

void LiteTable::drawGrid(SkCanvas* canvas) {
    SkPaint gridPaint;
    gridPaint.setColor(m_gridColor.toARGB());
    gridPaint.setStyle(SkPaint::kStroke_Style);
    gridPaint.setStrokeWidth(1.0f);

    float totalWidth = getTotalColumnWidth();
    float totalHeight = m_rows.size() * m_rowHeight;

    // 绘制水平线
    for (size_t i = 0; i <= m_rows.size(); ++i) {
        float y = i * m_rowHeight;
        canvas->drawLine(0, y, totalWidth, y, gridPaint);
    }

    // 绘制垂直线
    float currentX = 0;
    for (size_t i = 0; i <= m_columns.size(); ++i) {
        canvas->drawLine(currentX, 0, currentX, totalHeight, gridPaint);
        if (i < m_columns.size()) {
            currentX += m_columns[i].width;
        }
    }
}

void LiteTable::onMousePressed(const MouseEvent& event) {
    // 先检查是否点击了滚动条
    if (isPointInVerticalScrollbar(event.x, event.y) || 
        isPointInHorizontalScrollbar(event.x, event.y)) {
        LiteScrollView::onMousePressed(event);
        return;
    }

    if (event.button != MouseButton::Left) return;

    // 计算点击位置相对于内容区域的坐标
    float borderL = getLayoutBorderLeft();
    float borderT = getLayoutBorderTop();
    float padL = getLayoutPaddingLeft();
    float padT = getLayoutPaddingTop();

    float contentX = event.x - borderL - padL;
    float contentY = event.y - borderT - padT;

    // 检查是否点击了表头
    if (m_showHeader && contentY >= 0 && contentY < m_headerHeight) {
        // 表头点击，可以用于排序等功能
        return;
    }

    // 检查是否在视口范围内
    if (contentX < 0 || contentX >= getViewportWidth()) {
        return;
    }

    // 调整 contentY 以排除表头
    if (m_showHeader) {
        contentY -= m_headerHeight;
    }

    if (contentY < 0 || contentY >= (getViewportHeight() - (m_showHeader ? m_headerHeight : 0))) {
        return;
    }

    // 加上滚动偏移，得到实际内容坐标
    float actualY = contentY + m_scrollY;
    float actualX = contentX + m_scrollX;
    
    int rowIndex = getRowIndexAtY(actualY);
    int colIndex = getColumnIndexAtX(actualX);

    if (rowIndex >= 0 && rowIndex < static_cast<int>(m_rows.size())) {
        if (m_selectionMode == ListSelectionMode::Single) {
            setSelectedRow(rowIndex);
        } else if (m_selectionMode == ListSelectionMode::Multiple) {
            m_rows[rowIndex].selected = !m_rows[rowIndex].selected;
            markDirty();
            if (m_onSelectionChanged) {
                m_onSelectionChanged(rowIndex);
            }
        }

        if (m_onRowClicked) {
            m_onRowClicked(rowIndex);
        }

        if (colIndex >= 0 && m_onCellClicked) {
            m_onCellClicked(rowIndex, colIndex);
        }
    }
}

void LiteTable::onMouseMoved(const MouseEvent& event) {
    // 先处理滚动条拖拽
    LiteScrollView::onMouseMoved(event);

    // 计算鼠标位置相对于内容区域的坐标
    float borderT = getLayoutBorderTop();
    float borderL = getLayoutBorderLeft();
    float padT = getLayoutPaddingTop();
    float padL = getLayoutPaddingLeft();

    float contentX = event.x - borderL - padL;
    float contentY = event.y - borderT - padT;

    // 调整 contentY 以排除表头
    if (m_showHeader) {
        contentY -= m_headerHeight;
    }

    // 检查是否在视口范围内
    if (contentX < 0 || contentX >= getViewportWidth() ||
        contentY < 0 || contentY >= (getViewportHeight() - (m_showHeader ? m_headerHeight : 0))) {
        if (m_hoverRow != -1) {
            m_hoverRow = -1;
            markDirty();
        }
        return;
    }

    // 加上滚动偏移，得到实际内容坐标
    float actualY = contentY + m_scrollY;
    int newHoverRow = getRowIndexAtY(actualY);

    if (newHoverRow != m_hoverRow) {
        m_hoverRow = newHoverRow;
        markDirty();
    }
}

void LiteTable::onMouseExited(const MouseEvent& event) {
    if (m_hoverRow != -1) {
        m_hoverRow = -1;
        markDirty();
    }
    LiteScrollView::onMouseExited(event);
}

} // namespace liteDui
