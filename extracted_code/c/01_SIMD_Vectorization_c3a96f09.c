/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 920
 * Language: c
 * Block ID: c3a96f09
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 矩阵乘法向量化（GEMM优化）
 * 基于BLAS dgemm算法的简化版本
 * 优化技术: 分块、向量化、寄存器分块
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(__x86_64__)
    #include <immintrin.h>
#endif
#if defined(__aarch64__)
    #include <arm_neon.h>
#endif

// 分块大小参数 (根据L1缓存大小调整)
#define MC 256  // 行分块
#define KC 256  // 内积分块
#define NC 256  // 列分块
#define MR 8    // 寄存器分块行
#define NR 8    // 寄存器分块列

// 宏内核: 8x8分块计算
#if defined(__AVX2__)
void gemm_micro_kernel_8x8(int k, const float *a, const float *b, float *c, int ldc) {
    __m256 c_vec[8];  // C的8列

    // 加载C矩阵到寄存器
    for (int j = 0; j < 8; j++) {
        c_vec[j] = _mm256_loadu_ps(&c[j * ldc]);
    }

    // 内积循环
    for (int l = 0; l < k; l++) {
        __m256 a_vec = _mm256_loadu_ps(&a[l * MR]);  // 加载A的一列

        for (int j = 0; j < 8; j++) {
            __m256 b_broadcast = _mm256_broadcast_ss(&b[l * NR + j]);
            c_vec[j] = _mm256_fmadd_ps(a_vec, b_broadcast, c_vec[j]);
        }
    }

    // 存储回C
    for (int j = 0; j < 8; j++) {
        _mm256_storeu_ps(&c[j * ldc], c_vec[j]);
    }
}
#endif

#if defined(__aarch64__)
void gemm_micro_kernel_8x8_neon(int k, const float *a, const float *b, float *c, int ldc) {
    float32x4_t c_vec[16];  // 8x8 = 64个元素, 用16个128位寄存器

    // 初始化累加器为0
    for (int i = 0; i < 16; i++) {
        c_vec[i] = vdupq_n_f32(0.0f);
    }

    // 内积循环
    for (int l = 0; l < k; l++) {
        // 加载A的8个元素 (分成2个128位寄存器)
        float32x4_t a0 = vld1q_f32(&a[l * 8]);
        float32x4_t a1 = vld1q_f32(&a[l * 8 + 4]);

        // 计算与B的乘积
        for (int j = 0; j < 8; j++) {
            float b_val = b[l * 8 + j];
            float32x4_t b_vec0 = vdupq_n_f32(b_val);
            float32x4_t b_vec1 = vdupq_n_f32(b_val);

            c_vec[j * 2] = vfmaq_f32(c_vec[j * 2], a0, b_vec0);
            c_vec[j * 2 + 1] = vfmaq_f32(c_vec[j * 2 + 1], a1, b_vec1);
        }
    }

    // 存储结果
    for (int j = 0; j < 8; j++) {
        vst1q_f32(&c[j * ldc], c_vec[j * 2]);
        vst1q_f32(&c[j * ldc + 4], c_vec[j * 2 + 1]);
    }
}
#endif

// 简化版GEMM: C = A * B + C
void gemm_simple(int m, int n, int k, const float *A, int lda,
                 const float *B, int ldb, float *C, int ldc) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            float sum = C[i * ldc + j];
            for (int l = 0; l < k; l++) {
                sum += A[i * lda + l] * B[l * ldb + j];
            }
            C[i * ldc + j] = sum;
        }
    }
}

// SIMD优化版GEMM
#if defined(__AVX2__)
void gemm_avx2(int m, int n, int k, const float *A, int lda,
               const float *B, int ldb, float *C, int ldc) {
    for (int j = 0; j < n; j += 8) {
        for (int i = 0; i < m; i += 8) {
            // 初始化8x8 C块
            __m256 c[8];
            for (int jj = 0; jj < 8; jj++) {
                c[jj] = _mm256_setzero_ps();
            }

            // 内积
            for (int l = 0; l < k; l++) {
                __m256 a_vec = _mm256_loadu_ps(&A[i * lda + l]);
                for (int jj = 0; jj < 8; jj++) {
                    __m256 b_broadcast = _mm256_broadcast_ss(&B[l * ldb + j + jj]);
                    c[jj] = _mm256_fmadd_ps(a_vec, b_broadcast, c[jj]);
                }
            }

            // 累加到C
            for (int jj = 0; jj < 8; jj++) {
                __m256 c_old = _mm256_loadu_ps(&C[(i) * ldc + j + jj]);
                c[jj] = _mm256_add_ps(c[jj], c_old);
                _mm256_storeu_ps(&C[(i) * ldc + j + jj], c[jj]);
            }
        }
    }
}
#endif

int main() {
    int m = 512, n = 512, k = 512;

    float *A, *B, *C1, *C2;
    posix_memalign((void**)&A, 64, m * k * sizeof(float));
    posix_memalign((void**)&B, 64, k * n * sizeof(float));
    posix_memalign((void**)&C1, 64, m * n * sizeof(float));
    posix_memalign((void**)&C2, 64, m * n * sizeof(float));

    // 初始化
    for (int i = 0; i < m * k; i++) A[i] = (float)rand() / RAND_MAX;
    for (int i = 0; i < k * n; i++) B[i] = (float)rand() / RAND_MAX;
    memset(C1, 0, m * n * sizeof(float));
    memcpy(C2, C1, m * n * sizeof(float));

    struct timespec start, end;

    printf("=== 矩阵乘法性能对比 (%dx%d x %dx%d) ===\n\n", m, k, k, n);

    // 标量版本
    clock_gettime(CLOCK_MONOTONIC, &start);
    gemm_simple(m, n, k, A, k, B, n, C1, n);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double t_scalar = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("标量GEMM:      %.3f ms, GFLOPS=%.2f\n",
           t_scalar * 1000, (2.0 * m * n * k) / t_scalar / 1e9);

#if defined(__AVX2__)
    clock_gettime(CLOCK_MONOTONIC, &start);
    gemm_avx2(m, n, k, A, k, B, n, C2, n);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double t_avx2 = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("AVX2 GEMM:     %.3f ms, GFLOPS=%.2f, 加速比=%.1fx\n",
           t_avx2 * 1000, (2.0 * m * n * k) / t_avx2 / 1e9, t_scalar / t_avx2);
#endif

    free(A); free(B); free(C1); free(C2);
    return 0;
}
