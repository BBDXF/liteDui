/**
 * lite_font_manager.h - 全局字体管理器
 * 
 * 提供统一的字体资源管理，包括 SkFontMgr 和 FontCollection
 * 子类可以通过此管理器获取一致的字体配置
 */

#pragma once

#include "lite_common.h"
#include "include/core/SkFont.h"
#include "include/core/SkFontMgr.h"
#include "modules/skparagraph/include/FontCollection.h"
#include "modules/skparagraph/include/TextStyle.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include <memory>
#include <string>

namespace liteDui {

/**
 * LiteFontManager - 字体管理器单例
 * 
 * 负责管理全局的 Skia 字体资源，提供：
 * - 全局 SkFontMgr 实例
 * - 全局 FontCollection 实例（支持字体回退）
 * - 便捷的样式构造方法
 */
class LiteFontManager {
public:
    /**
     * 获取单例实例
     */
    static LiteFontManager& getInstance();

    /**
     * 禁用拷贝和移动
     */
    LiteFontManager(const LiteFontManager&) = delete;
    LiteFontManager& operator=(const LiteFontManager&) = delete;
    LiteFontManager(LiteFontManager&&) = delete;
    LiteFontManager& operator=(LiteFontManager&&) = delete;

    /**
     * 获取字体管理器
     */
    sk_sp<SkFontMgr> getFontMgr() const { return m_fontMgr; }

    /**
     * 获取字体集合（用于 skparagraph）
     */
    sk_sp<skia::textlayout::FontCollection> getFontCollection() const { return m_fontCollection; }

    /**
     * 获取默认字体族名称
     */
    const std::string& getDefaultFontFamily() const { return m_defaultFontFamily; }

    /**
     * 设置默认字体族名称
     */
    void setDefaultFontFamily(const std::string& family);

    /**
     * 创建 SkFont 实例
     * @param fontSize 字体大小
     * @param fontFamily 字体族名称（空则使用默认）
     */
    SkFont createFont(float fontSize, const std::string& fontFamily = "") const;

    /**
     * 创建 TextStyle
     * @param color 文本颜色
     * @param fontSize 字体大小
     * @param fontFamily 字体族名称（空则使用默认）
     */
    skia::textlayout::TextStyle createTextStyle(
        const Color& color,
        float fontSize,
        const std::string& fontFamily = "") const;

    /**
     * 创建 ParagraphStyle
     * @param textAlign 文本对齐方式
     */
    skia::textlayout::ParagraphStyle createParagraphStyle(TextAlign textAlign) const;

private:
    LiteFontManager();
    ~LiteFontManager() = default;

    void initialize();

    sk_sp<SkFontMgr> m_fontMgr;
    sk_sp<skia::textlayout::FontCollection> m_fontCollection;
    std::string m_defaultFontFamily;
};

} // namespace liteDui
