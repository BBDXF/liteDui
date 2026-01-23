/**
 * lite_utf8.cpp - UTF-8 字符串处理工具类实现
 */

#include "lite_utf8.h"

namespace liteDui {

std::vector<Utf8Helper::CodePointInfo> Utf8Helper::toCodePoints(const std::string& text) {
    std::vector<CodePointInfo> result;
    size_t i = 0;
    
    while (i < text.length()) {
        CodePointInfo info;
        info.byteOffset = static_cast<int>(i);
        unsigned char c = text[i];
        
        if ((c & 0x80) == 0) {
            // ASCII: 0xxxxxxx
            info.codepoint = c;
            info.byteLength = 1;
            i += 1;
        } else if ((c & 0xE0) == 0xC0) {
            // 2字节: 110xxxxx 10xxxxxx
            info.codepoint = (c & 0x1F) << 6;
            if (i + 1 < text.length()) {
                info.codepoint |= (text[i + 1] & 0x3F);
            }
            info.byteLength = 2;
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            // 3字节: 1110xxxx 10xxxxxx 10xxxxxx
            info.codepoint = (c & 0x0F) << 12;
            if (i + 1 < text.length()) {
                info.codepoint |= (text[i + 1] & 0x3F) << 6;
            }
            if (i + 2 < text.length()) {
                info.codepoint |= (text[i + 2] & 0x3F);
            }
            info.byteLength = 3;
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            // 4字节: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            info.codepoint = (c & 0x07) << 18;
            if (i + 1 < text.length()) {
                info.codepoint |= (text[i + 1] & 0x3F) << 12;
            }
            if (i + 2 < text.length()) {
                info.codepoint |= (text[i + 2] & 0x3F) << 6;
            }
            if (i + 3 < text.length()) {
                info.codepoint |= (text[i + 3] & 0x3F);
            }
            info.byteLength = 4;
            i += 4;
        } else {
            // 无效字节，当作单字节处理
            info.codepoint = c;
            info.byteLength = 1;
            i += 1;
        }
        
        result.push_back(info);
    }
    
    return result;
}

int Utf8Helper::byteToCodePointIndex(const std::vector<CodePointInfo>& codePoints, int bytePos) {
    for (size_t i = 0; i < codePoints.size(); ++i) {
        if (codePoints[i].byteOffset == bytePos) {
            return static_cast<int>(i);
        }
        if (codePoints[i].byteOffset > bytePos) {
            return static_cast<int>(i > 0 ? i - 1 : 0);
        }
    }
    return static_cast<int>(codePoints.size());
}

int Utf8Helper::codePointIndexToByte(const std::vector<CodePointInfo>& codePoints, int cpIndex, int textLength) {
    if (cpIndex <= 0) return 0;
    if (cpIndex >= static_cast<int>(codePoints.size())) return textLength;
    return codePoints[cpIndex].byteOffset;
}

int Utf8Helper::getPrevCharPos(const std::string& text, int pos) {
    if (pos <= 0 || text.empty()) return 0;
    
    auto codePoints = toCodePoints(text);
    if (codePoints.empty()) return 0;
    
    // 找到当前位置对应的码点索引
    int cpIndex = byteToCodePointIndex(codePoints, pos);
    
    // 移动到前一个码点
    if (cpIndex > 0) {
        cpIndex--;
    }
    
    // 返回前一个码点的字节位置
    return codePointIndexToByte(codePoints, cpIndex, static_cast<int>(text.length()));
}

int Utf8Helper::getNextCharPos(const std::string& text, int pos) {
    if (pos >= static_cast<int>(text.length()) || text.empty()) {
        return static_cast<int>(text.length());
    }
    
    auto codePoints = toCodePoints(text);
    if (codePoints.empty()) return static_cast<int>(text.length());
    
    // 找到当前位置对应的码点索引
    int cpIndex = byteToCodePointIndex(codePoints, pos);
    
    // 移动到下一个码点
    cpIndex++;
    
    // 返回下一个码点的字节位置
    return codePointIndexToByte(codePoints, cpIndex, static_cast<int>(text.length()));
}

std::string Utf8Helper::codepointToUtf8(uint32_t codepoint) {
    std::string result;
    
    if (codepoint < 0x80) {
        result += static_cast<char>(codepoint);
    } else if (codepoint < 0x800) {
        result += static_cast<char>(0xC0 | (codepoint >> 6));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint < 0x10000) {
        result += static_cast<char>(0xE0 | (codepoint >> 12));
        result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else {
        result += static_cast<char>(0xF0 | (codepoint >> 18));
        result += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    }
    
    return result;
}

int Utf8Helper::getCharCount(const std::string& text) {
    return static_cast<int>(toCodePoints(text).size());
}

bool Utf8Helper::isContinuationByte(unsigned char c) {
    return (c & 0xC0) == 0x80;
}

int Utf8Helper::getCharByteLength(unsigned char c) {
    if ((c & 0x80) == 0) return 1;        // 0xxxxxxx - ASCII
    else if ((c & 0xE0) == 0xC0) return 2; // 110xxxxx
    else if ((c & 0xF0) == 0xE0) return 3; // 1110xxxx
    else if ((c & 0xF8) == 0xF0) return 4; // 11110xxx
    return 1; // 无效字节，当作1字节处理
}

int Utf8Helper::utf8ByteToUtf16Index(const std::string& text, int bytePos) {
    int utf16Index = 0;
    int i = 0;
    
    while (i < bytePos && i < static_cast<int>(text.length())) {
        unsigned char c = text[i];
        int charBytes = getCharByteLength(c);
        
        // 解码当前字符的码点
        uint32_t codepoint = 0;
        if ((c & 0x80) == 0) {
            // ASCII: 1 字节
            codepoint = c;
        } else if ((c & 0xE0) == 0xC0) {
            // 2 字节
            codepoint = (c & 0x1F) << 6;
            if (i + 1 < static_cast<int>(text.length())) {
                codepoint |= (text[i + 1] & 0x3F);
            }
        } else if ((c & 0xF0) == 0xE0) {
            // 3 字节
            codepoint = (c & 0x0F) << 12;
            if (i + 1 < static_cast<int>(text.length())) {
                codepoint |= (text[i + 1] & 0x3F) << 6;
            }
            if (i + 2 < static_cast<int>(text.length())) {
                codepoint |= (text[i + 2] & 0x3F);
            }
        } else if ((c & 0xF8) == 0xF0) {
            // 4 字节
            codepoint = (c & 0x07) << 18;
            if (i + 1 < static_cast<int>(text.length())) {
                codepoint |= (text[i + 1] & 0x3F) << 12;
            }
            if (i + 2 < static_cast<int>(text.length())) {
                codepoint |= (text[i + 2] & 0x3F) << 6;
            }
            if (i + 3 < static_cast<int>(text.length())) {
                codepoint |= (text[i + 3] & 0x3F);
            }
        }
        
        // 码点 >= 0x10000 需要 2 个 UTF-16 代码单元 (surrogate pair)
        // 否则只需要 1 个 UTF-16 代码单元
        utf16Index += (codepoint >= 0x10000) ? 2 : 1;
        
        i += charBytes;
    }
    
    return utf16Index;
}

} // namespace liteDui
