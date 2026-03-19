/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 1818
 * Language: c
 * Block ID: fe30d82d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* GCC 15 SIMD 字符串处理优化 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* strlen - GCC 15 使用 SIMD 优化 */
size_t optimized_strlen(const char* str) {
    /* 简单实现，编译器自动向量化 */
    const char* s = str;
    while (*s) {
        s++;
    }
    return s - str;
}

/* 字符查找 - GCC 15 优化 */
char* optimized_strchr(const char* str, int c) {
    /* 编译器自动使用 SIMD 指令 */
    char ch = (char)c;
    while (*str) {
        if (*str == ch) {
            return (char*)str;
        }
        str++;
    }
    return ch == '\0' ? (char*)str : NULL;
}

/* 内存比较 - GCC 15 优化 */
int optimized_memcmp(const void* s1, const void* s2, size_t n) {
    const uint8_t* p1 = s1;
    const uint8_t* p2 = s2;

    /* 编译器自动向量化大内存块比较 */
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return (int)p1[i] - (int)p2[i];
        }
    }
    return 0;
}

/* 行尾查找 - 文本处理常见操作 */
const char* find_line_end(const char* text) {
    /* GCC 15 使用 SIMD 同时搜索 \n 和 \r */
    while (*text && *text != '\n' && *text != '\r') {
        text++;
    }
    return text;
}
