/**
 * lite_tree_view.cpp - 树形控件实现
 * 
 * 基于 LiteScrollView 实现，利用其 canvas 裁剪和滚动功能
 */

#include "lite_tree_view.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPath.h"

namespace liteDui {

LiteTreeView::LiteTreeView() {
    setScrollDirection(ScrollDirection::Vertical);
    setBackgroundColor(Color::White());
    setBorderColor(Color::fromRGB(200, 200, 200));
    setBorder(EdgeInsets::All(1.0f));
}

LiteTreeView::~LiteTreeView() {
    delete m_rootNode;
}

void LiteTreeView::setRootNode(TreeNode* root) {
    delete m_rootNode;
    m_rootNode = root;
    markDirty();
}

void LiteTreeView::setSelectionMode(TreeSelectionMode mode) { 
    m_selectionMode = mode; 
}

void LiteTreeView::setShowRoot(bool show) { 
    m_showRoot = show; 
    markDirty(); 
}

void LiteTreeView::setShowLines(bool show) { 
    m_showLines = show; 
    markDirty(); 
}

void LiteTreeView::setIndentation(float indent) { 
    m_indentation = indent; 
    markDirty(); 
}

void LiteTreeView::setItemHeight(float height) { 
    m_itemHeight = height; 
    markDirty(); 
}

void LiteTreeView::expandAll() { 
    if (m_rootNode) expandAllRecursive(m_rootNode, true); 
    markDirty(); 
}

void LiteTreeView::collapseAll() { 
    if (m_rootNode) expandAllRecursive(m_rootNode, false); 
    markDirty(); 
}

void LiteTreeView::expandAllRecursive(TreeNode* node, bool expand) {
    node->setExpanded(expand);
    for (size_t i = 0; i < node->getChildCount(); i++) {
        expandAllRecursive(node->getChildAt(i), expand);
    }
}

void LiteTreeView::setSelectedNode(TreeNode* node) {
    if (m_selectedNode) m_selectedNode->setSelected(false);
    m_selectedNode = node;
    if (node) node->setSelected(true);
    if (m_onNodeSelected) m_onNodeSelected(node);
    markDirty();
}

TreeNode* LiteTreeView::getSelectedNode() const { 
    return m_selectedNode; 
}

void LiteTreeView::setOnNodeSelected(std::function<void(TreeNode*)> callback) { 
    m_onNodeSelected = callback; 
}

void LiteTreeView::setOnNodeExpanded(std::function<void(TreeNode*)> callback) { 
    m_onNodeExpanded = callback; 
}

void LiteTreeView::setOnNodeCollapsed(std::function<void(TreeNode*)> callback) { 
    m_onNodeCollapsed = callback; 
}

int LiteTreeView::countVisibleNodes(TreeNode* node) const {
    if (!node) return 0;
    int count = 1;
    if (node->isExpanded()) {
        for (size_t i = 0; i < node->getChildCount(); i++) {
            count += countVisibleNodes(node->getChildAt(i));
        }
    }
    return count;
}

float LiteTreeView::getContentHeight() const {
    if (!m_rootNode) return 0;
    int count = m_showRoot ? countVisibleNodes(m_rootNode) : countVisibleNodes(m_rootNode) - 1;
    return count * m_itemHeight;
}

void LiteTreeView::render(SkCanvas* canvas) {
    LiteScrollView::render(canvas);
}

void LiteTreeView::renderContent(SkCanvas* canvas) {
    if (!m_rootNode) return;
    
    // 注意：此时 canvas 已经应用了滚动偏移，所以从 0 开始绘制
    float y = 0;
    
    if (m_showRoot) {
        renderNode(canvas, m_rootNode, y, 0);
    } else {
        for (size_t i = 0; i < m_rootNode->getChildCount(); i++) {
            renderNode(canvas, m_rootNode->getChildAt(i), y, 0);
        }
    }
}

void LiteTreeView::renderNode(SkCanvas* canvas, TreeNode* node, float& y, float x) {
    float viewportW = getViewportWidth();
    float nodeX = x + node->getDepth() * m_indentation;
    
    // 绘制选中背景
    if (node->isSelected()) {
        SkPaint selPaint;
        selPaint.setColor(Color::fromRGB(66, 133, 244, 80).toARGB());
        canvas->drawRect(SkRect::MakeXYWH(0, y, viewportW, m_itemHeight), selPaint);
    }

    // 绘制展开/折叠图标
    if (node->hasChildren()) {
        SkPaint iconPaint;
        iconPaint.setAntiAlias(true);
        iconPaint.setColor(Color::Gray().toARGB());
        iconPaint.setStyle(SkPaint::kStroke_Style);
        iconPaint.setStrokeWidth(1.5f);
        
        float iconX = nodeX + 8;
        float iconY = y + m_itemHeight / 2;
        if (node->isExpanded()) {
            // 向下箭头
            canvas->drawLine(iconX - 4, iconY - 2, iconX, iconY + 2, iconPaint);
            canvas->drawLine(iconX, iconY + 2, iconX + 4, iconY - 2, iconPaint);
        } else {
            // 向右箭头
            canvas->drawLine(iconX - 2, iconY - 4, iconX + 2, iconY, iconPaint);
            canvas->drawLine(iconX + 2, iconY, iconX - 2, iconY + 4, iconPaint);
        }
    }

    // 绘制文本
    setText(node->getText());
    setTextColor(Color::Black());
    float textX = nodeX + (node->hasChildren() ? 20 : 4);
    drawText(canvas, textX, y, viewportW - textX, m_itemHeight);
    
    y += m_itemHeight;
    
    // 递归绘制子节点
    if (node->isExpanded()) {
        for (size_t i = 0; i < node->getChildCount(); i++) {
            renderNode(canvas, node->getChildAt(i), y, x);
        }
    }
}

TreeNode* LiteTreeView::findNodeAtY(float y) const {
    if (!m_rootNode) return nullptr;
    
    // y 是相对于内容区域的坐标（已经考虑了滚动偏移）
    float currentY = 0;
    
    if (m_showRoot) {
        return findNodeAtYRecursive(m_rootNode, currentY, y);
    } else {
        for (size_t i = 0; i < m_rootNode->getChildCount(); i++) {
            TreeNode* found = findNodeAtYRecursive(m_rootNode->getChildAt(i), currentY, y);
            if (found) return found;
        }
    }
    return nullptr;
}

TreeNode* LiteTreeView::findNodeAtYRecursive(TreeNode* node, float& currentY, float targetY) const {
    if (targetY >= currentY && targetY < currentY + m_itemHeight) {
        return node;
    }
    currentY += m_itemHeight;
    
    if (node->isExpanded()) {
        for (size_t i = 0; i < node->getChildCount(); i++) {
            TreeNode* found = findNodeAtYRecursive(node->getChildAt(i), currentY, targetY);
            if (found) return found;
        }
    }
    return nullptr;
}

void LiteTreeView::onMousePressed(const MouseEvent& event) {
    // 先检查是否点击了滚动条
    if (isPointInVerticalScrollbar(event.x, event.y) || 
        isPointInHorizontalScrollbar(event.x, event.y)) {
        LiteScrollView::onMousePressed(event);
        return;
    }
    
    // 计算点击位置相对于内容区域的坐标
    float borderL = getLayoutBorderLeft();
    float borderT = getLayoutBorderTop();
    float padL = getLayoutPaddingLeft();
    float padT = getLayoutPaddingTop();

    // 转换为内容区域坐标
    float contentX = event.x - borderL - padL;
    float contentY = event.y - borderT - padT;
    
    // 检查是否在视口范围内
    if (contentX < 0 || contentX >= getViewportWidth() ||
        contentY < 0 || contentY >= getViewportHeight()) {
        return;
    }
    
    // 加上滚动偏移，得到实际内容坐标
    float actualY = contentY + m_scrollY;
    TreeNode* clickedNode = findNodeAtY(actualY);
    
    if (!clickedNode) {
        return;
    }
    
    // 计算节点的 X 位置
    float nodeX = clickedNode->getDepth() * m_indentation;
    
    // 点击展开图标区域（图标区域大约是 nodeX 到 nodeX + 20）
    if (clickedNode->hasChildren() && contentX >= nodeX && contentX < nodeX + 20) {
        clickedNode->setExpanded(!clickedNode->isExpanded());
        if (clickedNode->isExpanded() && m_onNodeExpanded) {
            m_onNodeExpanded(clickedNode);
        } else if (!clickedNode->isExpanded() && m_onNodeCollapsed) {
            m_onNodeCollapsed(clickedNode);
        }
        markDirty();
    } else if (m_selectionMode != TreeSelectionMode::None) {
        // 点击文本区域，选中节点
        setSelectedNode(clickedNode);
    }
}

} // namespace liteDui
