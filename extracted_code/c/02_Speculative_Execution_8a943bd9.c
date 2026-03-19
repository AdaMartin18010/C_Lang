/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\02_Speculative_Execution.md
 * Line: 167
 * Language: c
 * Block ID: 8a943bd9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 方法1：使用 speculation barrier (lfence)
#include <immintrin.h>

void protected_with_lfence(size_t index, uint8_t *data) {
    if (index < array_size) {
        _mm_lfence();  // 序列化，阻止推测执行
        uint8_t value = data[index];
        temp &= public_data[value * 4096];
    }
}

// 方法2：使用条件移动（无分支）
void protected_with_cmov(size_t index, uint8_t *data) {
    uint8_t value = 0;
    // 即使index越界，也只是读取0
    if (index < array_size) {
        value = data[index];
    }
    temp &= public_data[value * 4096];
}

// 方法3：使用 __builtin_speculation_safe_value (GCC)
#ifdef __GNUC__
void protected_with_builtin(size_t index, uint8_t *data) {
    if (index < array_size) {
        uint8_t value = __builtin_speculation_safe_value(data[index]);
        temp &= public_data[value * 4096];
    }
}
#endif
