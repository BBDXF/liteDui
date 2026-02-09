/**
 * lite_menu.h - 菜单系统
 */

#pragma once

#include "lite_container.h"
#include "lite_window.h"

namespace liteDui {

class LiteMenu;
class LiteMenuBar;
class LiteMenuItem;
class MenuOverlay;
using LiteMenuPtr = std::shared_ptr<LiteMenu>;
using LiteMenuBarPtr = std::shared_ptr<LiteMenuBar>;

enum class MenuItemType { Normal, Separator, Submenu, Checkable };

class LiteMenuItem {
public:
    LiteMenuItem() = default;
    explicit LiteMenuItem(const std::string& text) : m_text(text) {}
    ~LiteMenuItem() = default;
    
    void setText(const std::string& text) { m_text = text; }
    const std::string& getText() const { return m_text; }
    void setType(MenuItemType type) { m_type = type; }
    MenuItemType getType() const { return m_type; }
    void setChecked(bool checked) { m_checked = checked; }
    bool isChecked() const { return m_checked; }
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }
    void setShortcut(const std::string& shortcut) { m_shortcut = shortcut; }
    const std::string& getShortcut() const { return m_shortcut; }
    void setSubmenu(LiteMenu* submenu) { m_submenu = submenu; m_type = MenuItemType::Submenu; }
    LiteMenu* getSubmenu() const { return m_submenu; }
    void setOnClick(std::function<void()> callback) { m_onClick = callback; }
    void triggerClick() { if (m_onClick && m_enabled) m_onClick(); }

private:
    std::string m_text;
    MenuItemType m_type = MenuItemType::Normal;
    bool m_checked = false;
    bool m_enabled = true;
    std::string m_shortcut;
    LiteMenu* m_submenu = nullptr;
    std::function<void()> m_onClick;
    friend class LiteMenu;
    friend class MenuOverlay;
};

// Menu Overlay
class MenuOverlay : public LiteContainer {
public:
    MenuOverlay(LiteMenu* menu);
    void render(SkCanvas* canvas) override;
    void onMousePressed(const MouseEvent& event) override;
    void onMouseMoved(const MouseEvent& event) override;
    
private:
    LiteMenu* m_menu;
    int m_hoverIndex = -1;
};

class LiteMenu : public LiteContainer {
    friend class MenuOverlay;
public:
    LiteMenu();
    ~LiteMenu() override;
    
    LiteMenuItem* addItem(const std::string& text, std::function<void()> callback = nullptr);
    void addSeparator();
    void addSubmenu(const std::string& text, LiteMenu* submenu);
    void removeItem(size_t index);
    void clearItems();
    size_t getItemCount() const { return m_items.size(); }
    LiteMenuItem* getItem(size_t index) const { return index < m_items.size() ? m_items[index] : nullptr; }
    
    void show(float x, float y);
    void show(float x, float y, LiteWindow* window);
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
    float m_itemHeight = 24.0f;
    float m_menuX = 0, m_menuY = 0;
    Color m_bgColor = Color::White();
    Color m_selectionColor = Color::fromRGB(66, 133, 244, 80);
    std::shared_ptr<MenuOverlay> m_overlay;
};

class LiteMenuBar : public LiteContainer {
public:
    LiteMenuBar();
    ~LiteMenuBar() override;
    
    void addMenu(const std::string& title, LiteMenu* menu);
    void removeMenu(size_t index);
    void clearMenus();
    size_t getMenuCount() const { return m_menus.size(); }
    
    void render(SkCanvas* canvas) override;
    void onMousePressed(const MouseEvent& event) override;
    void onMouseMoved(const MouseEvent& event) override;

private:
    struct MenuInfo { std::string title; LiteMenu* menu; float x; float width; };
    std::vector<MenuInfo> m_menus;
    int m_activeIndex = -1;
    int m_hoverIndex = -1;
};

} // namespace liteDui
