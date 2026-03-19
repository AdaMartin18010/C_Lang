/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 659
 * Language: c
 * Block ID: fec31aa1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 向量点积优化实现对比
 * 展示从标量到SIMD再到多核的优化路径
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#if defined(__x86_64__)
    #include <immintrin.h>
#endif
#if defined(__aarch64__)
    #include <arm_neon.h>
#endif

#define ALIGN64 __attribute__((aligned(64)))

/**
 * 版本1: 标量实现（基准）
 */
float dot_scalar(const float *a, const float *b, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

/**
 * 版本2: 循环展开（减少循环开销）
 */
float dot_unroll4(const float *a, const float *b, int n) {
    float sum0 = 0.0f, sum1 = 0.0f, sum2 = 0.0f, sum3 = 0.0f;
    int i = 0;

    // 每次迭代处理4个元素，消除循环依赖
    for (; i <= n - 4; i += 4) {
        sum0 += a[i]   * b[i];
        sum1 += a[i+1] * b[i+1];
        sum2 += a[i+2] * b[i+2];
        sum3 += a[i+3] * b[i+3];
    }

    // 合并部分和
    float sum = sum0 + sum1 + sum2 + sum3;

    // 尾部处理
    for (; i < n; i++) sum += a[i] * b[i];
    return sum;
}

/**
 * 版本3: NEON实现（ARM）
 */
#if defined(__aarch64__)
float dot_neon(const float *a, const float *b, int n) {
    float32x4_t sum_vec = vdupq_n_f32(0.0f);
    int i = 0;

    // 主循环: 每次处理4×4=16个元素
    for (; i <= n - 16; i += 16) {
        float32x4_t a0 = vld1q_f32(&a[i]);
        float32x4_t a1 = vld1q_f32(&a[i+4]);
        float32x4_t a2 = vld1q_f32(&a[i+8]);
        float32x4_t a3 = vld1q_f32(&a[i+12]);

        float32x4_t b0 = vld1q_f32(&b[i]);
        float32x4_t b1 = vld1q_f32(&b[i+4]);
        float32x4_t b2 = vld1q_f32(&b[i+8]);
        float32x4_t b3 = vld1q_f32(&b[i+12]);

        // 乘加累积
        sum_vec = vfmaq_f32(sum_vec, a0, b0);
        sum_vec = vfmaq_f32(sum_vec, a1, b1);
        sum_vec = vfmaq_f32(sum_vec, a2, b2);
        sum_vec = vfmaq_f32(sum_vec, a3, b3);
    }

    // 水平归约
    float sum = vaddvq_f32(sum_vec);

    // 尾部处理
    for (; i < n; i++) sum += a[i] * b[i];
    return sum;
}
#endif

/**
 * 版本4: AVX2实现（x86, 8-way并行）
 */
#if defined(__AVX2__)
float dot_avx2(const float *a, const float *b, int n) {
    __m256 sum_vec = _mm256_setzero_ps();
    int i = 0;

    // 主循环: 每次处理8×4=32个元素
    for (; i <= n - 32; i += 32) {
        __m256 a0 = _mm256_loadu_ps(&a[i]);
        __m256 a1 = _mm256_loadu_ps(&a[i+8]);
        __m256 a2 = _mm256_loadu_ps(&a[i+16]);
        __m256 a3 = _mm256_loadu_ps(&a[i+24]);

        __m256 b0 = _mm256_loadu_ps(&b[i]);
        __m256 b1 = _mm256_loadu_ps(&b[i+8]);
        __m256 b2 = _mm256_loadu_ps(&b[i+16]);
        __m256 b3 = _mm256_loadu_ps(&b[i+24]);

        sum_vec = _mm256_fmadd_ps(a0, b0, sum_vec);
        sum_vec = _mm256_fmadd_ps(a1, b1, sum_vec);
        sum_vec = _mm256_fmadd_ps(a2, b2, sum_vec);
        sum_vec = _mm256_fmadd_ps(a3, b3, sum_vec);
    }

    // 水平归约: sum_vec[0..7] -> scalar
    __m256 hsum = _mm256_hadd_ps(sum_vec, sum_vec);
    hsum = _mm256_hadd_ps(hsum, hsum);
    float sum = _mm_cvtss_f32(_mm256_castps256_ps128(hsum)) +
                _mm_cvtss_f32(_mm256_extractf128_ps(hsum, 1));

    // 尾部处理
    for (; i < n; i++) sum += a[i] * b[i];
    return sum;
}
#endif

/**
 * 版本5: AVX-512实现（16-way并行）
 */
#if defined(__AVX512F__)
float dot_avx512(const float *a, const float *b, int n) {
    __m512 sum_vec = _mm512_setzero_ps();
    int i = 0;

    for (; i <= n - 64; i += 64) {
        __m512 a0 = _mm512_loadu_ps(&a[i]);
        __m512 a1 = _mm512_loadu_ps(&a[i+16]);
        __m512 a2 = _mm512_loadu_ps(&a[i+32]);
        __m512 a3 = _mm512_loadu_ps(&a[i+48]);

        __m512 b0 = _mm512_loadu_ps(&b[i]);
        __m512 b1 = _mm512_loadu_ps(&b[i+16]);
        __m512 b2 = _mm512_loadu_ps(&b[i+32]);
        __m512 b3 = _mm512_loadu_ps(&b[i+48]);

        sum_vec = _mm512_fmadd_ps(a0, b0, sum_vec);
        sum_vec = _mm512_fmadd_ps(a1, b1, sum_vec);
        sum_vec = _mm512_fmadd_ps(a2, b2, sum_vec);
        sum_vec = _mm512_fmadd_ps(a3, b3, sum_vec);
    }

    // 水平归约
    float sum = _mm512_reduce_add_ps(sum_vec);

    // 尾部处理
    for (; i < n; i++) sum += a[i] * b[i];
    return sum;
}
#endif

/**
 * 版本6: 双精度累加（Kahan补偿算法，提高精度）
 */
float dot_kahan(const float *a, const float *b, int n) {
    float sum = 0.0f;
    float c = 0.0f;  // 补偿项

    for (int i = 0; i < n; i++) {
        float y = a[i] * b[i] - c;
        float t = sum + y;
        c = (t - sum) - y;
        sum = t;
    }
    return sum;
}

// 性能测试
double time_diff(struct timespec *start, struct timespec *end) {
    return (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / 1e9;
}

int main() {
    const int N = 1024 * 1024;  // 1M元素
    const int ITER = 100;

    float *a, *b;
    posix_memalign((void**)&a, 64, N * sizeof(float));
    posix_memalign((void**)&b, 64, N * sizeof(float));

    // 初始化
    for (int i = 0; i < N; i++) {
        a[i] = sinf(i * 0.01f);
        b[i] = cosf(i * 0.01f);
    }

    struct timespec start, end;
    float result;
    double elapsed;

    printf("=== 向量点积性能对比 (N=%d) ===\n\n", N);

    // 标量
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < ITER; i++) result = dot_scalar(a, b, N);
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = time_diff(&start, &end);
    printf("标量实现:       %.3f ms, 结果=%.6f, GFLOPS=%.2f\n",
           elapsed * 1000, result, (2.0 * N * ITER) / elapsed / 1e9);

    // 循环展开
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < ITER; i++) result = dot_unroll4(a, b, N);
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = time_diff(&start, &end);
    printf("循环展开4x:     %.3f ms, 结果=%.6f, GFLOPS=%.2f\n",
           elapsed * 1000, result, (2.0 * N * ITER) / elapsed / 1e9);

#if defined(__aarch64__)
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < ITER; i++) result = dot_neon(a, b, N);
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = time_diff(&start, &end);
    printf("NEON 4x32:      %.3f ms, 结果=%.6f, GFLOPS=%.2f\n",
           elapsed * 1000, result, (2.0 * N * ITER) / elapsed / 1e9);
#endif

#if defined(__AVX2__)
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < ITER; i++) result = dot_avx2(a, b, N);
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = time_diff(&start, &end);
    printf("AVX2 8x32:      %.3f ms, 结果=%.6f, GFLOPS=%.2f\n",
           elapsed * 1000, result, (2.0 * N * ITER) / elapsed / 1e9);
#endif

#if defined(__AVX512F__)
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < ITER; i++) result = dot_avx512(a, b, N);
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = time_diff(&start, &end);
    printf("AVX-512 16x32:  %.3f ms, 结果=%.6f, GFLOPS=%.2f\n",
           elapsed * 1000, result, (2.0 * N * ITER) / elapsed / 1e9);
#endif

    // Kahan算法
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < ITER; i++) result = dot_kahan(a, b, N);
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = time_diff(&start, &end);
    printf("Kahan高精度:    %.3f ms, 结果=%.6f, GFLOPS=%.2f\n",
           elapsed * 1000, result, (2.0 * N * ITER) / elapsed / 1e9);

    free(a); free(b);
    return 0;
}
