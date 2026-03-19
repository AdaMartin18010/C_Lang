/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\02_Code_Quality.md
 * Line: 311
 * Language: c
 * Block ID: 8f0aa99b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 整数溢出后分配不足
void *alloc_buffer(size_t count, size_t size) {
    return malloc(count * size);  // 溢出！
}

// ✅ 安全检查
define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>

void *alloc_buffer_safe(size_t count, size_t size) {
    if (count == 0 || size == 0) return NULL;

    // 检查溢出
    if (SIZE_MAX / count < size) {
        errno = ENOMEM;
        return NULL;
    }

    return calloc(count, size);  // calloc内部检查溢出
}
