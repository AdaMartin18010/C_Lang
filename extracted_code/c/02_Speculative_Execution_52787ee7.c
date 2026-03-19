/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\02_Speculative_Execution.md
 * Line: 273
 * Language: c
 * Block ID: 52787ee7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 恒定时间比较（防止时序攻击）
int constant_time_memcmp(const void *a, const void *b, size_t len) {
    const uint8_t *pa = a;
    const uint8_t *pb = b;
    uint8_t result = 0;

    for (size_t i = 0; i < len; i++) {
        result |= pa[i] ^ pb[i];  // 始终访问所有字节
    }

    return result;  // 0表示相等，非0表示不等
}

// 恒定时间选择
uint32_t constant_time_select(uint32_t mask, uint32_t a, uint32_t b) {
    // mask全1选择a，全0选择b
    return (a & mask) | (b & ~mask);
}

// 恒定时间条件移动
uint32_t constant_time_eq(uint32_t a, uint32_t b) {
    uint32_t diff = a ^ b;
    // 如果diff==0，返回全1；否则返回全0
    return ~((diff | -diff) >> 31);
}
