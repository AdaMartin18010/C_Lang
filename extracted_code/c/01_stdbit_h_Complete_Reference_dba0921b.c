/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\C23_Standard_Library\01_stdbit_h_Complete_Reference.md
 * Line: 190
 * Language: c
 * Block ID: dba0921b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 1. 快速 log2
int fast_log2(unsigned int x) {
    return 31 - stdc_leading_zeros(x);
}

// 2. 找到最高位设置
int highest_bit_set(unsigned int x) {
    return x == 0 ? -1 : 31 - stdc_leading_zeros(x);
}

// 3. 浮点分解
void float_decompose(float f, int* sign, int* exponent, unsigned int* mantissa) {
    union { float f; unsigned int i; } u = {f};
    *sign = u.i >> 31;
    *exponent = ((u.i >> 23) & 0xFF) - 127;
    *mantissa = u.i & 0x7FFFFF;
}
