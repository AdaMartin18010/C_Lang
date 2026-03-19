/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\README.md
 * Line: 666
 * Language: c
 * Block ID: c9b4e5cd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* nr_simd.h - SIMD优化 */
#ifndef NR_SIMD_H
#define NR_SIMD_H

#include "nr_phy.h"

/* 检测SIMD支持 */
#if defined(__AVX2__)
    #define USE_AVX2
#elif defined(__AVX__)
    #define USE_AVX
#elif defined(__SSE4_2__)
    #define USE_SSE4
#elif defined(__ARM_NEON)
    #define USE_NEON
#endif

/* SIMD复数运算 */
void simd_complex_multiply(const cf_t *a, const cf_t *b, cf_t *c, uint32_t n);
void simd_complex_add(const cf_t *a, const cf_t *b, cf_t *c, uint32_t n);
void simd_complex_conj_multiply(const cf_t *a, const cf_t *b, cf_t *c, uint32_t n);
float simd_complex_energy(const cf_t *a, uint32_t n);

/* SIMD FFT加速 */
void simd_fft_execute(cf_t *data, uint32_t n, bool inverse);

/* SIMD矩阵运算 */
void simd_matrix_multiply_cf(const cf_t *a, const cf_t *b, cf_t *c,
                              uint32_t m, uint32_t n, uint32_t p);

#endif /* NR_SIMD_H */
