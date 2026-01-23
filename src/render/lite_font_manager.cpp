/**
 * lite_font_manager.cpp - 全局字体管理器实现
 */

#include "lite_font_manager.h"
#include "include/core/SkTypeface.h"
#include "include/ports/SkFontMgr_fontconfig.h"
#include "include/ports/SkFontScanner_FreeType.h"

namespace liteDui {

LiteFontManager& LiteFontManager::getInstance() {
    static LiteFontManager instance;
    return instance;
}

LiteFontManager::LiteFontManager() 
    : m_defaultFontFamily(LITE_DEFAULT_FONT_FAMILY) {
    initialize();
}

void LiteFontManager::initialize() {
    // 创建平台相关的字体管理器
    m_fontMgr = SkFontMgr_New_FontConfig(nullptr, SkFontScanner_Make_FreeType());
    
    // 创建字体集合
    m_fontCollection = sk_make_sp<skia::textlayout::FontCollection>();
    m_fontCollection->setDefaultFontManager(m_fontMgr);
    m_fontCollection->enableFontFallback();
}

void LiteFontManager::setDefaultFontFamily(const std::string& family) {
    m_defaultFontFamily = family;
}

SkFont LiteFontManager::createFont(float fontSize, const std::string& fontFamily) const {
    SkFont font;
    font.setSize(fontSize);
    
    // 尝试加载指定字体
    const std::string& family = fontFamily.empty() ? m_defaultFontFamily : fontFamily;
    if (m_fontMgr && !family.empty()) {
        sk_sp<SkTypeface> typeface = m_fontMgr->matchFamilyStyle(family.c_str(), SkFontStyle::Normal());
        if (typeface) {
            font.setTypeface(typeface);
        }
    }
    
    return font;
}

skia::textlayout::TextStyle LiteFontManager::createTextStyle(
    const Color& color,
    float fontSize,
    const std::string& fontFamily) const {
    
    using namespace skia::textlayout;
    
    TextStyle style;
    style.setColor(color.toARGB());
    style.setFontSize(fontSize);
    
    const std::string& family = fontFamily.empty() ? m_defaultFontFamily : fontFamily;
    style.setFontFamilies({SkString(family.c_str())});
    
    return style;
}

skia::textlayout::ParagraphStyle LiteFontManager::createParagraphStyle(TextAlign textAlign) const {
    using namespace skia::textlayout;
    
    ParagraphStyle style;
    
    switch (textAlign) {
        case TextAlign::Center:
            style.setTextAlign(skia::textlayout::TextAlign::kCenter);
            break;
        case TextAlign::Right:
            style.setTextAlign(skia::textlayout::TextAlign::kRight);
            break;
        case TextAlign::Left:
        default:
            style.setTextAlign(skia::textlayout::TextAlign::kLeft);
            break;
    }
    
    return style;
}

} // namespace liteDui
