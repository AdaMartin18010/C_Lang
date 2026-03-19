/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 1761
 * Language: c
 * Block ID: db96297d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* GCC 15 优化的搜索算法 */
#include <stddef.h>
#include <stdint.h>
#include <immintrin.h>

/* 向量化线性搜索 - GCC 15 自动生成类似代码 */
int vectorized_search(const int32_t* arr, size_t n, int32_t target) {
    size_t i = 0;

    /* SIMD 向量化部分 */
    const size_t simd_width = 8;  /* AVX2: 256-bit / 32-bit = 8 */
    const size_t simd_end = n - (n % simd_width);

    __m256i vtarget = _mm256_set1_epi32(target);

    for (; i < simd_end; i += simd_width) {
        __m256i vdata = _mm256_loadu_si256((__m256i*)&arr[i]);
        __m256i vcmp = _mm256_cmpeq_epi32(vdata, vtarget);
        int mask = _mm256_movemask_ps((__m256)vcmp);

        if (mask != 0) {
            /* 找到匹配 */
            int pos = __builtin_ctz(mask);
            return (int)(i + pos);
        }
    }

    /* 标量收尾 */
    for (; i < n; i++) {
        if (arr[i] == target) {
            return (int)i;
        }
    }

    return -1;
}

/* GCC 15 编译器自动优化的版本 */
int compiler_optimized_search(const int32_t* arr, size_t n, int32_t target) {
    /* 简单代码，编译器自动向量化 */
    for (size_t i = 0; i < n; i++) {
        if (arr[i] == target) {
            return (int)i;
        }
    }
    return -1;
}

/* 编译命令 */
/* gcc-15 -O3 -march=sapphire-rapids -o search search.c */
