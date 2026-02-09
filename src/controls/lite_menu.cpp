/**
 * lite_menu.cpp - 菜单系统实现
 */

#include "lite_menu.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPath.h"

namespace liteDui {

// ==================== MenuOverlay ====================

MenuOverlay::MenuOverlay(LiteMenu* menu) : m_menu(menu) {
    setBackgroundColor(Color::Transparent());
}

void MenuOverlay::render(SkCanvas* canvas) {
    if (!m_menu || m_menu->m_items.empty()) return;
    
    float menuX = m_menu->m_menuX;
    float menuY = m_menu->m_menuY;
    float menuWidth = 180.0f;
    float menuHeight = 0;
    
    for (auto* item : m_menu->m_items) {
        menuHeight += (item->getType() == MenuItemType::Separator) ? 8.0f : m_menu->m_itemHeight;
    }

    // 背景
    SkPaint bgPaint;
    bgPaint.setColor(m_menu->m_bgColor.toARGB());
    canvas->drawRect(SkRect::MakeXYWH(menuX, menuY, menuWidth, menuHeight), bgPaint);

    // 边框
    SkPaint borderPaint;
    borderPaint.setStyle(SkPaint::kStroke_Style);
    borderPaint.setColor(Color::fromRGB(200, 200, 200).toARGB());
    canvas->drawRect(SkRect::MakeXYWH(menuX, menuY, menuWidth, menuHeight), borderPaint);

    float y = menuY;
    for (size_t i = 0; i < m_menu->m_items.size(); i++) {
        auto* item = m_menu->m_items[i];
        
        if (item->getType() == MenuItemType::Separator) {
            SkPaint sepPaint;
            sepPaint.setColor(Color::fromRGB(220, 220, 220).toARGB());
            canvas->drawLine(menuX + 8, y + 4, menuX + menuWidth - 8, y + 4, sepPaint);
            y += 8.0f;
            continue;
        }

        if (static_cast<int>(i) == m_hoverIndex && item->isEnabled()) {
            SkPaint selPaint;
            selPaint.setColor(m_menu->m_selectionColor.toARGB());
            canvas->drawRect(SkRect::MakeXYWH(menuX, y, menuWidth, m_menu->m_itemHeight), selPaint);
        }

        setTextColor(item->isEnabled() ? Color::Black() : Color::Gray());
        setText(item->getText());
        // 使用相对于 overlay 的坐标
        drawText(canvas, menuX + 12 - getLeft(), y - getTop(), menuWidth - 24, m_menu->m_itemHeight);

        if (!item->getShortcut().empty()) {
            setText(item->getShortcut());
            setTextColor(Color::Gray());
            setTextAlign(TextAlign::Right);
            drawText(canvas, menuX - getLeft(), y - getTop(), menuWidth - 12, m_menu->m_itemHeight);
            setTextAlign(TextAlign::Left);
        }

        if (item->getType() == MenuItemType::Checkable && item->isChecked()) {
            SkPaint checkPaint;
            checkPaint.setColor(Color::Black().toARGB());
            checkPaint.setStrokeWidth(2);
            checkPaint.setStyle(SkPaint::kStroke_Style);
            float cx = menuX + 8, cy = y + m_menu->m_itemHeight / 2;
            canvas->drawLine(cx - 3, cy, cx, cy + 3, checkPaint);
            canvas->drawLine(cx, cy + 3, cx + 5, cy - 3, checkPaint);
        }

        if (item->getType() == MenuItemType::Submenu) {
            SkPaint arrowPaint;
            arrowPaint.setColor(Color::Gray().toARGB());
            arrowPaint.setStyle(SkPaint::kStroke_Style);
            arrowPaint.setStrokeWidth(1.5f);
            float ax = menuX + menuWidth - 12, ay = y + m_menu->m_itemHeight / 2;
            canvas->drawLine(ax - 3, ay - 4, ax + 2, ay, arrowPaint);
            canvas->drawLine(ax + 2, ay, ax - 3, ay + 4, arrowPaint);
        }

        y += m_menu->m_itemHeight;
    }
}

void MenuOverlay::onMousePressed(const MouseEvent& event) {
    if (!m_menu) return;
    
    float menuX = m_menu->m_menuX;
    float menuY = m_menu->m_menuY;
    float menuWidth = 180.0f;
    float menuHeight = 0;
    
    for (auto* item : m_menu->m_items) {
        menuHeight += (item->getType() == MenuItemType::Separator) ? 8.0f : m_menu->m_itemHeight;
    }
    
    // 检查是否点击在菜单区域内
    if (event.x >= menuX && event.x < menuX + menuWidth &&
        event.y >= menuY && event.y < menuY + menuHeight) {
        if (m_hoverIndex >= 0 && m_hoverIndex < static_cast<int>(m_menu->m_items.size())) {
            auto* item = m_menu->m_items[m_hoverIndex];
            if (item->isEnabled() && item->getType() != MenuItemType::Separator) {
                if (item->getType() == MenuItemType::Checkable) {
                    item->setChecked(!item->isChecked());
                }
                item->triggerClick();
            }
        }
    }
    
    // 关闭菜单
    m_menu->hide();
}

void MenuOverlay::onMouseMoved(const MouseEvent& event) {
    if (!m_menu) return;
    
    float menuX = m_menu->m_menuX;
    float menuY = m_menu->m_menuY;
    float menuWidth = 180.0f;
    
    int newHover = -1;
    if (event.x >= menuX && event.x < menuX + menuWidth) {
        float y = menuY;
        for (size_t i = 0; i < m_menu->m_items.size(); i++) {
            float itemH = (m_menu->m_items[i]->getType() == MenuItemType::Separator) ? 8.0f : m_menu->m_itemHeight;
            if (event.y >= y && event.y < y + itemH) {
                newHover = static_cast<int>(i);
                break;
            }
            y += itemH;
        }
    }
    
    if (newHover != m_hoverIndex) {
        m_hoverIndex = newHover;
        markDirty();
    }
}

// ==================== LiteMenu ====================

LiteMenu::LiteMenu() {
    setBackgroundColor(m_bgColor);
    setBorderColor(Color::fromRGB(200, 200, 200));
    setBorder(EdgeInsets::All(1.0f));
}

LiteMenu::~LiteMenu() {
    clearItems();
}

LiteMenuItem* LiteMenu::addItem(const std::string& text, std::function<void()> callback) {
    auto* item = new LiteMenuItem(text);
    item->setOnClick(callback);
    m_items.push_back(item);
    return item;
}

void LiteMenu::addSeparator() {
    auto* item = new LiteMenuItem();
    item->setType(MenuItemType::Separator);
    m_items.push_back(item);
}

void LiteMenu::addSubmenu(const std::string& text, LiteMenu* submenu) {
    auto* item = new LiteMenuItem(text);
    item->setSubmenu(submenu);
    m_items.push_back(item);
}

void LiteMenu::removeItem(size_t index) {
    if (index < m_items.size()) {
        delete m_items[index];
        m_items.erase(m_items.begin() + index);
    }
}

void LiteMenu::clearItems() {
    for (auto* item : m_items) delete item;
    m_items.clear();
}

void LiteMenu::show(float x, float y) {
    show(x, y, getWindow());
}

void LiteMenu::show(float x, float y, LiteWindow* window) {
    m_menuX = x;
    m_menuY = y;
    m_isShown = true;
    
    // 如果传入了 window，更新内部引用
    if (window && !getWindow()) {
        setWindow(window);
    }
    
    auto win = getWindow();
    if (win) {
        if (!m_overlay) {
            m_overlay = std::make_shared<MenuOverlay>(this);
        }
        win->pushOverlay(m_overlay);
    }
    markDirty();
}

void LiteMenu::hide() {
    m_isShown = false;
    
    auto window = getWindow();
    if (window && m_overlay) {
        window->removeOverlay(m_overlay);
    }
    markDirty();
}

void LiteMenu::setItemHeight(float height) { m_itemHeight = height; }
void LiteMenu::setBackgroundColor(const Color& color) { m_bgColor = color; LiteContainer::setBackgroundColor(color); }
void LiteMenu::setSelectionColor(const Color& color) { m_selectionColor = color; }

void LiteMenu::render(SkCanvas* canvas) {
    // 菜单内容由 overlay 渲染，这里不需要做任何事
}

void LiteMenu::onMousePressed(const MouseEvent& event) {
    // 事件由 overlay 处理
}

void LiteMenu::onMouseMoved(const MouseEvent& event) {
    // 事件由 overlay 处理
}

// ==================== LiteMenuBar ====================

LiteMenuBar::LiteMenuBar() {
    setBackgroundColor(Color::fromRGB(245, 245, 245));
    setHeight(28.0f);
}

LiteMenuBar::~LiteMenuBar() {
    for (auto& info : m_menus) delete info.menu;
}

void LiteMenuBar::addMenu(const std::string& title, LiteMenu* menu) {
    m_menus.push_back({title, menu, 0, 0});
    markDirty();
}

void LiteMenuBar::removeMenu(size_t index) {
    if (index < m_menus.size()) {
        delete m_menus[index].menu;
        m_menus.erase(m_menus.begin() + index);
        markDirty();
    }
}

void LiteMenuBar::clearMenus() {
    for (auto& info : m_menus) delete info.menu;
    m_menus.clear();
    markDirty();
}

void LiteMenuBar::render(SkCanvas* canvas) {
    float w = getLayoutWidth();
    float h = getLayoutHeight();

    drawBackground(canvas, 0, 0, w, h);

    float menuX = 8;
    for (size_t i = 0; i < m_menus.size(); i++) {
        auto& info = m_menus[i];
        info.x = menuX;
        
        SkFont font = getFont();
        float textWidth = font.measureText(info.title.c_str(), info.title.size(), SkTextEncoding::kUTF8);
        info.width = textWidth + 16;

        if (static_cast<int>(i) == m_hoverIndex || static_cast<int>(i) == m_activeIndex) {
            SkPaint hoverPaint;
            hoverPaint.setColor(Color::fromRGB(200, 200, 200).toARGB());
            canvas->drawRect(SkRect::MakeXYWH(menuX, 0, info.width, h), hoverPaint);
        }

        setText(info.title);
        setTextColor(Color::Black());
        drawText(canvas, menuX + 8, 0, info.width - 16, h);

        menuX += info.width;
    }
}

void LiteMenuBar::onMousePressed(const MouseEvent& event) {
    float h = getLayoutHeight();
    
    if (event.y >= 0 && event.y < h) {
        for (size_t i = 0; i < m_menus.size(); i++) {
            auto& info = m_menus[i];
            if (event.x >= info.x && event.x < info.x + info.width) {
                if (m_activeIndex == static_cast<int>(i)) {
                    m_menus[m_activeIndex].menu->hide();
                    m_activeIndex = -1;
                } else {
                    if (m_activeIndex >= 0) m_menus[m_activeIndex].menu->hide();
                    m_activeIndex = static_cast<int>(i);
                    // 使用绝对坐标
                    info.menu->setWindow(getWindow());
                    info.menu->show(getAbsoluteLeft() + info.x, getAbsoluteTop() + h);
                }
                markDirty();
                return;
            }
        }
    }
}

void LiteMenuBar::onMouseMoved(const MouseEvent& event) {
    float h = getLayoutHeight();
    
    int newHover = -1;
    if (event.y >= 0 && event.y < h) {
        for (size_t i = 0; i < m_menus.size(); i++) {
            auto& info = m_menus[i];
            if (event.x >= info.x && event.x < info.x + info.width) {
                newHover = static_cast<int>(i);
                break;
            }
        }
    }
    
    if (newHover != m_hoverIndex) {
        m_hoverIndex = newHover;
        if (m_activeIndex >= 0 && newHover >= 0 && newHover != m_activeIndex) {
            m_menus[m_activeIndex].menu->hide();
            m_activeIndex = newHover;
            auto& info = m_menus[m_activeIndex];
            info.menu->setWindow(getWindow());
            info.menu->show(getAbsoluteLeft() + info.x, getAbsoluteTop() + h);
        }
        markDirty();
    }
}

} // namespace liteDui
