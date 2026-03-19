/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\12_Compiler_Optimization\04_Auto_Vectorization.md
 * Line: 46
 * Language: c
 * Block ID: abd25552
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 编译器向量化示例
// 原始循环
void add_arrays(float *a, float *b, float *c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

// 编译器生成（AVX-512）
// vmovups zmm0, [rdi + rcx*4]
// vaddps  zmm0, zmm0, [rsi + rcx*4]
// vmovups [rdx + rcx*4], zmm0
// 每次处理16个float
