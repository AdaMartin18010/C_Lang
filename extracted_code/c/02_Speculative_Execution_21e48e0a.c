/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\02_Speculative_Execution.md
 * Line: 410
 * Language: c
 * Block ID: 21e48e0a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：试图恒定时间但编译器优化掉了
int problematic_compare(const uint8_t *a, const uint8_t *b) {
    for (size_t i = 0; i < 16; i++) {
        if (a[i] != b[i]) return 0;  // 编译器可能提前返回
    }
    return 1;
}

// 正确：使用volatile或内存屏障
int safe_compare(const uint8_t *a, const uint8_t *b) {
    volatile uint8_t diff = 0;
    for (size_t i = 0; i < 16; i++) {
        diff |= a[i] ^ b[i];
    }
    return diff == 0;
}
