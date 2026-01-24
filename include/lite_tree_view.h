/**
 * lite_tree_view.h - 树形控件
 */

#pragma once

#include "lite_scroll_view.h"

namespace liteDui {

class LiteTreeView;
class TreeNode;
using LiteTreeViewPtr = std::shared_ptr<LiteTreeView>;

class TreeNode {
public:
    TreeNode() = default;
    explicit TreeNode(const std::string& text) : m_text(text) {}
    ~TreeNode() { clearChildren(); }
    
    void setText(const std::string& text) { m_text = text; }
    const std::string& getText() const { return m_text; }
    void setData(void* data) { m_data = data; }
    void* getData() const { return m_data; }
    void setExpanded(bool expanded) { m_expanded = expanded; }
    bool isExpanded() const { return m_expanded; }
    void setSelected(bool selected) { m_selected = selected; }
    bool isSelected() const { return m_selected; }
    
    void addChild(TreeNode* child) {
        child->m_parent = this;
        m_children.push_back(child);
    }
    void removeChild(TreeNode* child) {
        auto it = std::find(m_children.begin(), m_children.end(), child);
        if (it != m_children.end()) {
            (*it)->m_parent = nullptr;
            m_children.erase(it);
        }
    }
    void clearChildren() {
        for (auto* child : m_children) delete child;
        m_children.clear();
    }
    size_t getChildCount() const { return m_children.size(); }
    TreeNode* getChildAt(size_t index) const { return index < m_children.size() ? m_children[index] : nullptr; }
    TreeNode* getParent() const { return m_parent; }
    bool hasChildren() const { return !m_children.empty(); }
    int getDepth() const {
        int depth = 0;
        TreeNode* p = m_parent;
        while (p) { depth++; p = p->m_parent; }
        return depth;
    }
    const std::vector<TreeNode*>& getChildren() const { return m_children; }

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

protected:
    void renderContent(SkCanvas* canvas) override;

private:
    int countVisibleNodes(TreeNode* node) const;
    void expandAllRecursive(TreeNode* node, bool expand);
    TreeNode* findNodeAtY(float y) const;
    TreeNode* findNodeAtYRecursive(TreeNode* node, float& currentY, float targetY) const;
    void renderNode(SkCanvas* canvas, TreeNode* node, float& y, float x);

    TreeNode* m_rootNode = nullptr;
    TreeSelectionMode m_selectionMode = TreeSelectionMode::Single;
    bool m_showRoot = true;
    bool m_showLines = true;
    float m_indentation = 20.0f;
    float m_itemHeight = 28.0f;
    TreeNode* m_selectedNode = nullptr;
    std::function<void(TreeNode*)> m_onNodeSelected;
    std::function<void(TreeNode*)> m_onNodeExpanded;
    std::function<void(TreeNode*)> m_onNodeCollapsed;
};

} // namespace liteDui
