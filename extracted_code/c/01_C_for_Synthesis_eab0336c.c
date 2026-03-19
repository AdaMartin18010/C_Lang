/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 338
 * Language: c
 * Block ID: eab0336c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ✅ 支持的struct用法
typedef struct {
    int real;
    int imag;
} complex_t;

void complex_multiply(complex_t a, complex_t b, complex_t *c) {
    c->real = a.real * b.real - a.imag * b.imag;
    c->imag = a.real * b.imag + a.imag * b.real;
}

// ✅ 位域支持（但需谨慎使用）
typedef struct {
    unsigned int flag1 : 1;
    unsigned int field : 7;
    unsigned int data  : 24;
} packed_t;

// ⚠️ union有限支持 - 通常用于类型转换
typedef union {
    float f;
    int i;
} float_int_t;

// ✅ 现代HLS推荐：使用位运算代替union
int float_to_int_bits(float f) {
    return *(int*)&f;  // 未定义行为，不推荐
}

// ✅ 更好的方式：使用HLS提供的库函数
#include "ap_int.h"
ap_uint<32> safe_float_to_bits(float f) {
    return *(ap_uint<32>*)&f;  // 使用ap类型更安全
}
