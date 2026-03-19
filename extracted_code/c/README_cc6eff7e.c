/*
 * Auto-generated from: 01_Core_Knowledge_System\11_Internationalization\README.md
 * Line: 98
 * Language: c
 * Block ID: cc6eff7e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* UTF-8 编码特性:
 * - ASCII 兼容 (0-127)
 * - 变长编码: 1-4 字节
 * - 最高位识别: 0xxxxxxx | 110xxxxx 10xxxxxx | 1110xxxx ...
 */

#include <stdint.h>
#include <stdbool.h>

// 检查是否为有效的 UTF-8 起始字节
bool is_utf8_start_byte(uint8_t byte) {
    return (byte & 0x80) == 0x00 ||  // ASCII
           (byte & 0xE0) == 0xC0 ||  // 2字节序列开始
           (byte & 0xF0) == 0xE0 ||  // 3字节序列开始
           (byte & 0xF8) == 0xF0;     // 4字节序列开始
}

// 计算 UTF-8 字符长度
size_t utf8_char_len(uint8_t first_byte) {
    if ((first_byte & 0x80) == 0x00) return 1;
    if ((first_byte & 0xE0) == 0xC0) return 2;
    if ((first_byte & 0xF0) == 0xE0) return 3;
    if ((first_byte & 0xF8) == 0xF0) return 4;
    return 0;  // 无效字节
}

// 安全遍历 UTF-8 字符串
void utf8_iterate(const char* str, void (*callback)(const char*, size_t)) {
    const uint8_t* p = (const uint8_t*)str;
    while (*p) {
        size_t len = utf8_char_len(*p);
        if (len == 0) { p++; continue; }  // 跳过无效字节
        callback((const char*)p, len);
        p += len;
    }
}
