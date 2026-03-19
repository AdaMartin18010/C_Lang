/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Depth.md
 * Line: 1542
 * Language: c
 * Block ID: 6b476d0e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ UNSAFE: 违反严格别名规则 */
void strict_aliasing_violation(void) {
    float f = 3.14f;
    int *pi = (int *)&f;     /* 通过int*访问float */
    *pi &= 0x7FFFFFFF;       /* UB! */
    printf("%f\n", f);
}

/* ❌ UNSAFE: 类型双关的常见错误 */
uint32_t bad_float_to_bits(float f) {
    return *(uint32_t *)&f;  /* UB: 类型双关 */
}

/* ✅ SAFE: 使用union（C99有效类型规则） */
typedef union {
    float f;
    uint32_t i;
} FloatBits;

uint32_t safe_float_to_bits(float f) {
    FloatBits fb = {.f = f};
    return fb.i;
}

/* ✅ SAFE: 使用memcpy（最可移植） */
#include <string.h>

uint32_t portable_float_to_bits(float f) {
    uint32_t i;
    memcpy(&i, &f, sizeof(f));
    return i;
}

/* ✅ SAFE: char* 可以别名任何类型 */
void byte_manipulation(void *data, size_t n) {
    unsigned char *bytes = data;  /* char* 允许别名 */
    for (size_t i = 0; i < n; i++) {
        bytes[i] ^= 0xFF;  /* 按字节操作 */
    }
}
