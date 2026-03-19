/*
 * Auto-generated from: 04_Industrial_Scenarios\12_Blockchain_Crypto\01_Cryptographic_Primitives.md
 * Line: 675
 * Language: c
 * Block ID: afad9f47
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 常数时间内存比较（防时序攻击）
int constant_time_compare(const volatile unsigned char* a,
                          const volatile unsigned char* b,
                          size_t len) {
    volatile unsigned char result = 0;
    for (size_t i = 0; i < len; i++) {
        result |= a[i] ^ b[i];
    }
    return result;  // 0 if equal, non-zero otherwise
}

// 常数时间选择（避免分支）
unsigned char constant_time_select(unsigned char condition,
                                   unsigned char a,
                                   unsigned char b) {
    // condition: 0xff (true) or 0x00 (false)
    return (condition & a) | ((~condition) & b);
}

// 常数时间整数比较
int constant_time_lt(uint64_t a, uint64_t b) {
    // 返回0xff如果a < b，否则0x00
    return (unsigned char)((a - b) >> 63) * 0xff;
}

// 使用sodium的常数时间函数
#include <sodium/utils.h>
void sodium_constant_time_example(void) {
    unsigned char a[32], b[32];
    // ... 填充数据

    // 常数时间比较
    int eq = sodium_memcmp(a, b, 32);

    // 常数时间是否为零检查
    int is_zero = sodium_is_zero(a, 32);

    // 安全清零
    sodium_memzero(a, 32);
}
