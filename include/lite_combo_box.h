/**
 * lite_combo_box.h - 下拉组合框控件
 */

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
