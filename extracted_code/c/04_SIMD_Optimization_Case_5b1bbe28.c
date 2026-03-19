/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\04_SIMD_Optimization_Case.md
 * Line: 535
 * Language: c
 * Block ID: 5b1bbe28
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// benchmark.c - SIMD性能测试

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifdef __x86_64__
#include <immintrin.h>
#endif

#ifdef __aarch64__
#include <arm_neon.h>
#endif

#ifdef __riscv
#include <riscv_vector.h>
#endif

// 计时函数
static inline double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

// 向量加法 - 标量版本
void vec_add_scalar(const float* a, const float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

// 向量加法 - AVX版本
#ifdef __AVX__
void vec_add_avx(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_load_ps(&a[i]);
        __m256 vb = _mm256_load_ps(&b[i]);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_store_ps(&c[i], vc);
    }
    for (; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}
#endif

// 向量加法 - NEON版本
#ifdef __ARM_NEON
void vec_add_neon(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;
    for (; i + 4 <= n; i += 4) {
        float32x4_t va = vld1q_f32(&a[i]);
        float32x4_t vb = vld1q_f32(&b[i]);
        float32x4_t vc = vaddq_f32(va, vb);
        vst1q_f32(&c[i], vc);
    }
    for (; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}
#endif

// 向量加法 - RVV版本
#ifdef __riscv
void vec_add_rvv(const float* a, const float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; ) {
        size_t vl = vsetvl_e32m8(n - i);
        vfloat32m8_t va = vle32_v_f32m8(&a[i], vl);
        vfloat32m8_t vb = vle32_v_f32m8(&b[i], vl);
        vfloat32m8_t vc = vfadd_vv_f32m8(va, vb, vl);
        vse32_v_f32m8(&c[i], vc, vl);
        i += vl;
    }
}
#endif

// 性能测试
void benchmark_vec_add(size_t n, int iterations) {
    float* a = aligned_alloc(64, n * sizeof(float));
    float* b = aligned_alloc(64, n * sizeof(float));
    float* c = aligned_alloc(64, n * sizeof(float));

    // 初始化
    for (size_t i = 0; i < n; i++) {
        a[i] = (float)rand() / RAND_MAX;
        b[i] = (float)rand() / RAND_MAX;
    }

    printf("Vector Add Benchmark (n=%zu, iterations=%d)\n", n, iterations);
    printf("================================================\n\n");

    // 标量版本
    {
        double start = get_time();
        for (int iter = 0; iter < iterations; iter++) {
            vec_add_scalar(a, b, c, n);
        }
        double elapsed = get_time() - start;
        double gflops = (double)n * iterations / elapsed / 1e9;
        printf("Scalar:  %.4f s, %.2f GFLOPS\n", elapsed, gflops);
    }

#ifdef __AVX__
    {
        double start = get_time();
        for (int iter = 0; iter < iterations; iter++) {
            vec_add_avx(a, b, c, n);
        }
        double elapsed = get_time() - start;
        double gflops = (double)n * iterations / elapsed / 1e9;
        printf("AVX:     %.4f s, %.2f GFLOPS\n", elapsed, gflops);
    }
#endif

#ifdef __ARM_NEON
    {
        double start = get_time();
        for (int iter = 0; iter < iterations; iter++) {
            vec_add_neon(a, b, c, n);
        }
        double elapsed = get_time() - start;
        double gflops = (double)n * iterations / elapsed / 1e9;
        printf("NEON:    %.4f s, %.2f GFLOPS\n", elapsed, gflops);
    }
#endif

#ifdef __riscv
    {
        double start = get_time();
        for (int iter = 0; iter < iterations; iter++) {
            vec_add_rvv(a, b, c, n);
        }
        double elapsed = get_time() - start;
        double gflops = (double)n * iterations / elapsed / 1e9;
        printf("RVV:     %.4f s, %.2f GFLOPS\n", elapsed, gflops);
    }
#endif

    free(a);
    free(b);
    free(c);
}

int main() {
    benchmark_vec_add(10000000, 100);
    return 0;
}
