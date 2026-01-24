/**
 * lite_tree_view.cpp - 树形控件实现
 */

#include "lite_tree_view.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPath.h"

namespace liteDui {

LiteTreeView::LiteTreeView() {
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

void LiteTreeView::setSelectionMode(TreeSelectionMode mode) { m_selectionMode = mode; }
void LiteTreeView::setShowRoot(bool show) { m_showRoot = show; markDirty(); }
void LiteTreeView::setShowLines(bool show) { m_showLines = show; markDirty(); }
void LiteTreeView::setIndentation(float indent) { m_indentation = indent; markDirty(); }
void LiteTreeView::setItemHeight(float height) { m_itemHeight = height; markDirty(); }

void LiteTreeView::expandAll() { if (m_rootNode) expandAllRecursive(m_rootNode, true); markDirty(); }
void LiteTreeView::collapseAll() { if (m_rootNode) expandAllRecursive(m_rootNode, false); markDirty(); }

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

TreeNode* LiteTreeView::getSelectedNode() const { return m_selectedNode; }

void LiteTreeView::setOnNodeSelected(std::function<void(TreeNode*)> callback) { m_onNodeSelected = callback; }
void LiteTreeView::setOnNodeExpanded(std::function<void(TreeNode*)> callback) { m_onNodeExpanded = callback; }
void LiteTreeView::setOnNodeCollapsed(std::function<void(TreeNode*)> callback) { m_onNodeCollapsed = callback; }

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
    
    float y = -m_scrollY;
    float w = getLayoutWidth();
    
    if (m_showRoot) {
        renderNode(canvas, m_rootNode, y, 0);
    } else {
        for (size_t i = 0; i < m_rootNode->getChildCount(); i++) {
            renderNode(canvas, m_rootNode->getChildAt(i), y, 0);
        }
    }
}

void LiteTreeView::renderNode(SkCanvas* canvas, TreeNode* node, float& y, float x) {
    float viewH = getLayoutHeight();
    float w = getLayoutWidth();
    float nodeX = x + node->getDepth() * m_indentation;
    
    if (y + m_itemHeight > 0 && y < viewH) {
        // 选中背景
        if (node->isSelected()) {
            SkPaint selPaint;
            selPaint.setColor(Color::fromRGB(66, 133, 244, 80).toARGB());
            canvas->drawRect(SkRect::MakeXYWH(0, y, w, m_itemHeight), selPaint);
        }

        // 展开/折叠图标
        if (node->hasChildren()) {
            SkPaint iconPaint;
            iconPaint.setAntiAlias(true);
            iconPaint.setColor(Color::Gray().toARGB());
            iconPaint.setStyle(SkPaint::kStroke_Style);
            iconPaint.setStrokeWidth(1.5f);
            
            float iconX = nodeX + 8;
            float iconY = y + m_itemHeight / 2;
            if (node->isExpanded()) {
                canvas->drawLine(iconX - 4, iconY - 2, iconX, iconY + 2, iconPaint);
                canvas->drawLine(iconX, iconY + 2, iconX + 4, iconY - 2, iconPaint);
            } else {
                canvas->drawLine(iconX - 2, iconY - 4, iconX + 2, iconY, iconPaint);
                canvas->drawLine(iconX + 2, iconY, iconX - 2, iconY + 4, iconPaint);
            }
        }

        // 文本
        setText(node->getText());
        setTextColor(Color::Black());
        float textX = nodeX + (node->hasChildren() ? 20 : 4);
        drawText(canvas, textX, y, w - textX, m_itemHeight);
    }
    
    y += m_itemHeight;
    
    if (node->isExpanded()) {
        for (size_t i = 0; i < node->getChildCount(); i++) {
            renderNode(canvas, node->getChildAt(i), y, x);
        }
    }
}

TreeNode* LiteTreeView::findNodeAtY(float y) const {
    if (!m_rootNode) return nullptr;
    float currentY = -m_scrollY;
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
    // 转换为本地坐标
    float localY = event.y - getTop();
    float localX = event.x - getLeft();
    
    TreeNode* clickedNode = findNodeAtY(localY);
    if (!clickedNode) {
        LiteScrollView::onMousePressed(event);
        return;
    }
    
    float nodeX = clickedNode->getDepth() * m_indentation;
    
    // 点击展开图标区域
    if (clickedNode->hasChildren() && localX >= nodeX && localX < nodeX + 20) {
        clickedNode->setExpanded(!clickedNode->isExpanded());
        if (clickedNode->isExpanded() && m_onNodeExpanded) {
            m_onNodeExpanded(clickedNode);
        } else if (!clickedNode->isExpanded() && m_onNodeCollapsed) {
            m_onNodeCollapsed(clickedNode);
        }
        markDirty();
    } else if (m_selectionMode != TreeSelectionMode::None) {
        setSelectedNode(clickedNode);
    }
}

} // namespace liteDui
