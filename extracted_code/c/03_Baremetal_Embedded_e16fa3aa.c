/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\03_Baremetal_Embedded.md
 * Line: 1093
 * Language: c
 * Block ID: e16fa3aa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// math_opt.c - 针对RISC-V优化的数学函数

#include <math.h>

// 使用RISC-V F/D扩展的快速平方根
static inline float fast_sqrtf(float x) {
    float result;
    __asm__ volatile (
        "fsqrt.s %0, %1"
        : "=f"(result)
        : "f"(x)
    );
    return result;
}

// 快速倒数 (使用牛顿迭代)
float fast_inv(float x) {
    float estimate;

    // 初始估计 (使用FPU指令)
    __asm__ volatile (
        "fdiv.s %0, fa1, %1"  // estimate = 1.0 / x
        : "=f"(estimate)
        : "f"(x)
    );

    // 一次牛顿迭代提高精度
    // x_{n+1} = x_n * (2 - a * x_n)
    float two = 2.0f;
    estimate = estimate * (two - x * estimate);

    return estimate;
}

// 优化的memcpy (使用字访问)
void *fast_memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = dst;
    const uint8_t *s = src;

    // 对齐到8字节边界
    while (n > 0 && ((uintptr_t)d & 7)) {
        *d++ = *s++;
        n--;
    }

    // 8字节块复制
    uint64_t *d64 = (uint64_t *)d;
    const uint64_t *s64 = (const uint64_t *)s;
    while (n >= 8) {
        *d64++ = *s64++;
        n -= 8;
    }

    // 剩余字节
    d = (uint8_t *)d64;
    s = (const uint8_t *)s64;
    while (n > 0) {
        *d++ = *s++;
        n--;
    }

    return dst;
}

// 优化的memset
void *fast_memset(void *s, int c, size_t n) {
    uint8_t *p = s;

    // 填充模式
    uint64_t pattern = (uint8_t)c;
    pattern |= pattern << 8;
    pattern |= pattern << 16;
    pattern |= pattern << 32;

    // 对齐到8字节
    while (n > 0 && ((uintptr_t)p & 7)) {
        *p++ = c;
        n--;
    }

    // 8字节块填充
    uint64_t *p64 = (uint64_t *)p;
    while (n >= 8) {
        *p64++ = pattern;
        n -= 8;
    }

    // 剩余字节
    p = (uint8_t *)p64;
    while (n > 0) {
        *p++ = c;
        n--;
    }

    return s;
}
