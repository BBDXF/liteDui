/**
 * lite_utf8.h - UTF-8 字符串处理工具类
 * 
 * 提供 UTF-8 和 Unicode 码点之间的转换，以及字符边界检测功能
 */

#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace liteDui {

/**
 * Utf8Helper - UTF-8 字符串处理工具类
 * 
 * 提供以下功能：
 * - UTF-8 字符串与 Unicode 码点数组之间的转换
 * - 字节位置与码点索引之间的转换
 * - 字符边界检测（前一个/下一个字符位置）
 * - Unicode 码点转 UTF-8 字符串
 */
class Utf8Helper {
public:
    /**
     * 码点信息结构体
     * @param codepoint Unicode 码点值
     * @param byteOffset 该码点在原 UTF-8 字符串中的字节偏移
     * @param byteLength 该码点占用的字节数
     */
    struct CodePointInfo {
        uint32_t codepoint;
        int byteOffset;
        int byteLength;
    };

    /**
     * 将 UTF-8 字符串转换为 Unicode 码点数组
     * @param text UTF-8 编码的字符串
     * @return 码点信息数组，包含每个字符的码点值和字节偏移
     */
    static std::vector<CodePointInfo> toCodePoints(const std::string& text);

    /**
     * 根据字节位置找到对应的码点索引
     * @param codePoints 码点信息数组
     * @param bytePos 字节位置
     * @return 码点索引（0-based）
     */
    static int byteToCodePointIndex(const std::vector<CodePointInfo>& codePoints, int bytePos);

    /**
     * 根据码点索引找到对应的字节位置
     * @param codePoints 码点信息数组
     * @param cpIndex 码点索引
     * @param textLength 原字符串的总字节长度
     * @return 字节位置
     */
    static int codePointIndexToByte(const std::vector<CodePointInfo>& codePoints, int cpIndex, int textLength);

    /**
     * 找到前一个字符的起始字节位置
     * @param text UTF-8 编码的字符串
     * @param pos 当前字节位置
     * @return 前一个字符的起始字节位置
     */
    static int getPrevCharPos(const std::string& text, int pos);

    /**
     * 找到下一个字符的起始字节位置
     * @param text UTF-8 编码的字符串
     * @param pos 当前字节位置
     * @return 下一个字符的起始字节位置
     */
    static int getNextCharPos(const std::string& text, int pos);

    /**
     * 将 Unicode 码点转换为 UTF-8 字符串
     * @param codepoint Unicode 码点值
     * @return UTF-8 编码的字符串
     */
    static std::string codepointToUtf8(uint32_t codepoint);

    /**
     * 获取 UTF-8 字符串的字符数（码点数）
     * @param text UTF-8 编码的字符串
     * @return 字符数
     */
    static int getCharCount(const std::string& text);

    /**
     * 将 UTF-8 字节偏移转换为 UTF-16 代码单元索引
     * Skia 的 skparagraph API 使用 UTF-16 索引，而非 UTF-8 字节偏移
     * @param text UTF-8 编码的字符串
     * @param bytePos UTF-8 字节位置
     * @return UTF-16 代码单元索引
     */
    static int utf8ByteToUtf16Index(const std::string& text, int bytePos);

    /**
     * 检查字节是否是 UTF-8 续字节 (10xxxxxx)
     * @param c 字节值
     * @return 是续字节返回 true
     */
    static bool isContinuationByte(unsigned char c);

    /**
     * 获取 UTF-8 字符的字节长度（根据首字节判断）
     * @param c 首字节
     * @return 字节长度 (1-4)
     */
    static int getCharByteLength(unsigned char c);
};

} // namespace liteDui
