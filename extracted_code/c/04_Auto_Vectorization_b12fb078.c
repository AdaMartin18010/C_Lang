/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\12_Compiler_Optimization\04_Auto_Vectorization.md
 * Line: 139
 * Language: c
 * Block ID: b12fb078
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 技巧1: 使用restrict消除别名
void no_alias(float *restrict a,
               float *restrict b,
               float *restrict c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

// 技巧2: 对齐数据
#include <immintrin.h>

void aligned_arrays(float *a, float *b, float *c, int n) {
    // 假设64字节对齐
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

// 技巧3: 循环展开提示
#pragma GCC unroll 4
void unrolled_loop(float *a, int n) {
    for (int i = 0; i < n; i++) {
        a[i] *= 2.0f;
    }
}

// 技巧4: 使用OpenMP SIMD
#include <omp.h>

void omp_simd(float *a, float *b, float *c, int n) {
    #pragma omp simd
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}
