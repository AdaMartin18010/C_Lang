/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 458
 * Language: c
 * Block ID: 1aac163c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 基础SIMD向量运算示例
 * 支持: x86(AVX2/AVX-512) + ARM(NEON/SVE)
 * 编译: gcc -O3 -march=native -o basic_ops basic_ops.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#if defined(__x86_64__) || defined(_M_X64)
    #include <immintrin.h>
    #define ARCH_X86
    #if defined(__AVX512F__)
        #define USE_AVX512
        #define VEC_WIDTH 16
        typedef __m512 vfloat;
        typedef __m512i vint;
    #elif defined(__AVX2__)
        #define USE_AVX2
        #define VEC_WIDTH 8
        typedef __m256 vfloat;
        typedef __m256i vint;
    #else
        #define USE_SSE
        #define VEC_WIDTH 4
        typedef __m128 vfloat;
        typedef __m128i vint;
    #endif
#elif defined(__aarch64__) || defined(_M_ARM64)
    #include <arm_neon.h>
    #define ARCH_ARM
    #if defined(__ARM_FEATURE_SVE)
        #include <arm_sve.h>
        #define USE_SVE
        #define VEC_WIDTH (svcntw())  // 运行时确定
        typedef svfloat32_t vfloat;
        typedef svint32_t vint;
    #else
        #define USE_NEON
        #define VEC_WIDTH 4
        typedef float32x4_t vfloat;
        typedef int32x4_t vint;
    #endif
#endif

// 对齐分配宏
#define ALIGN_ALLOC(ptr, size, alignment) \
    posix_memalign((void**)&(ptr), (alignment), (size))

/**
 * 向量加法: c[i] = a[i] + b[i]
 */
#if defined(USE_AVX512)
void vec_add(const float *a, const float *b, float *c, int n) {
    int i = 0;
    // 主循环: 每次处理16个float
    for (; i <= n - 16; i += 16) {
        __m512 va = _mm512_loadu_ps(&a[i]);
        __m512 vb = _mm512_loadu_ps(&b[i]);
        __m512 vc = _mm512_add_ps(va, vb);
        _mm512_storeu_ps(&c[i], vc);
    }
    // 尾部处理
    for (; i < n; i++) c[i] = a[i] + b[i];
}
#elif defined(USE_AVX2)
void vec_add(const float *a, const float *b, float *c, int n) {
    int i = 0;
    for (; i <= n - 8; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(&c[i], vc);
    }
    for (; i < n; i++) c[i] = a[i] + b[i];
}
#elif defined(USE_NEON)
void vec_add(const float *a, const float *b, float *c, int n) {
    int i = 0;
    for (; i <= n - 4; i += 4) {
        float32x4_t va = vld1q_f32(&a[i]);
        float32x4_t vb = vld1q_f32(&b[i]);
        float32x4_t vc = vaddq_f32(va, vb);
        vst1q_f32(&c[i], vc);
    }
    for (; i < n; i++) c[i] = a[i] + b[i];
}
#elif defined(USE_SVE)
void vec_add(const float *a, const float *b, float *c, int n) {
    int i = 0;
    svbool_t pg = svptrue_b32();
    while (i < n) {
        svbool_t mask = svwhilelt_b32(i, n);
        svfloat32_t va = svld1_f32(mask, &a[i]);
        svfloat32_t vb = svld1_f32(mask, &b[i]);
        svfloat32_t vc = svadd_f32_x(mask, va, vb);
        svst1_f32(mask, &c[i], vc);
        i += svcntw();
    }
}
#endif

/**
 * 向量乘加: c[i] = a[i] * b[i] + c[i] (FMA)
 */
#if defined(USE_AVX512)
void vec_fma(const float *a, const float *b, float *c, int n) {
    int i = 0;
    for (; i <= n - 16; i += 16) {
        __m512 va = _mm512_loadu_ps(&a[i]);
        __m512 vb = _mm512_loadu_ps(&b[i]);
        __m512 vc = _mm512_loadu_ps(&c[i]);
        vc = _mm512_fmadd_ps(va, vb, vc);
        _mm512_storeu_ps(&c[i], vc);
    }
    for (; i < n; i++) c[i] = a[i] * b[i] + c[i];
}
#elif defined(USE_AVX2)
void vec_fma(const float *a, const float *b, float *c, int n) {
    int i = 0;
    for (; i <= n - 8; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vc = _mm256_loadu_ps(&c[i]);
        vc = _mm256_fmadd_ps(va, vb, vc);
        _mm256_storeu_ps(&c[i], vc);
    }
    for (; i < n; i++) c[i] = fmaf(a[i], b[i], c[i]);
}
#elif defined(USE_NEON)
void vec_fma(const float *a, const float *b, float *c, int n) {
    int i = 0;
    for (; i <= n - 4; i += 4) {
        float32x4_t va = vld1q_f32(&a[i]);
        float32x4_t vb = vld1q_f32(&b[i]);
        float32x4_t vc = vld1q_f32(&c[i]);
        vc = vfmaq_f32(vc, va, vb);  // vc + va*vb
        vst1q_f32(&c[i], vc);
    }
    for (; i < n; i++) c[i] = fmaf(a[i], b[i], c[i]);
}
#endif

/**
 * 性能测试函数
 */
double benchmark(void (*func)(const float*, const float*, float*, int),
                 const float *a, const float *b, float *c, int n, int iterations) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int iter = 0; iter < iterations; iter++) {
        func(a, b, c, n);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    return elapsed;
}

int main() {
    const int N = 1024 * 1024;  // 1M元素
    const int ITERATIONS = 1000;

    float *a, *b, *c;
    ALIGN_ALLOC(a, N * sizeof(float), 64);
    ALIGN_ALLOC(b, N * sizeof(float), 64);
    ALIGN_ALLOC(c, N * sizeof(float), 64);

    // 初始化
    for (int i = 0; i < N; i++) {
        a[i] = (float)i / N;
        b[i] = (float)(N - i) / N;
        c[i] = 0.0f;
    }

    printf("=== SIMD基础运算性能测试 ===\n");
    printf("数组大小: %d elements (%.2f MB)\n", N, N * sizeof(float) / (1024.0 * 1024.0));
    printf("向量宽度: %d floats (%zu bits)\n\n", VEC_WIDTH, VEC_WIDTH * 32);

    // 预热缓存
    vec_add(a, b, c, N);

    // 测试加法
    double t_add = benchmark(vec_add, a, b, c, N, ITERATIONS);
    printf("向量加法:\n");
    printf("  总时间: %.3f s\n", t_add);
    printf("  吞吐量: %.2f GFLOPS\n", (double)N * ITERATIONS / t_add / 1e9);
    printf("  带宽:   %.2f GB/s\n", (double)N * ITERATIONS * 3 * sizeof(float) / t_add / 1e9);

    free(a); free(b); free(c);
    return 0;
}
