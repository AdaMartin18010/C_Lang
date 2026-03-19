/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 2054
 * Language: c
 * Block ID: 1205b7cb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * OpenMP SIMD指令
 */
#include <omp.h>

void vec_add_openmp(float *a, float *b, float *c, int n) {
    #pragma omp simd aligned(a, b, c : 32) safelen(8)
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

/**
 * GCC向量化属性
 */
void __attribute__((vector_size(16))) vec_add_gcc(float *a, float *b, float *c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

/**
 * ICC向量化pragma
 */
#ifdef __ICC
void vec_add_icc(float *a, float *b, float *c, int n) {
    #pragma ivdep
    #pragma vector aligned
    #pragma simd
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}
#endif

/**
 * 显式向量化类型 (GCC/Clang)
 */
typedef float v4sf __attribute__((vector_size(16)));

v4sf add_vectors(v4sf a, v4sf b) {
    return a + b;  // 编译器生成SIMD指令
}
