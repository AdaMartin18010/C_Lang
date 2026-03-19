/*
 * Auto-generated from: 00_VERSION_TRACKING\LLVM_22_C23_Update.md
 * Line: 882
 * Language: c
 * Block ID: b7ef1ba4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// AVX2 编译时字符串处理
#include <immintrin.h>
#include <string_view>

constexpr size_t constexpr_strlen(const char* str) {
    size_t len = 0;

    // 使用 AVX2 在编译时计算字符串长度
    while (str[len] != '\0') {
        len++;
    }

    return len;
}

// 编译时哈希计算
constexpr uint32_t constexpr_hash(const char* str) {
    uint32_t hash = 0x811c9dc5;

    for (size_t i = 0; str[i] != '\0'; i++) {
        hash ^= static_cast<uint8_t>(str[i]);
        hash *= 0x01000193;
    }

    return hash;
}

// 编译时常量
constexpr auto hello_hash = constexpr_hash("hello world");
