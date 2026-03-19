/*
 * Auto-generated from: 11_Machine_Learning_C\01_Neural_Network_Basics.md
 * Line: 1551
 * Language: c
 * Block ID: bce2d265
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifdef __AVX2__
#include <immintrin.h>

// AVX2优化的矩阵-向量乘法
// 使用8个double并行计算（AVX2寄存器256位 = 8 * 32位）
void matvec_mul_avx2(const double* A, const double* x, double* y,
                     int m, int n) {
    for (int i = 0; i < m; i++) {
        __m256d sum_vec = _mm256_setzero_pd();
        int j = 0;

        // 每次处理4个元素
        for (; j <= n - 4; j += 4) {
            __m256d a_vec = _mm256_loadu_pd(&A[i * n + j]);
            __m256d x_vec = _mm256_loadu_pd(&x[j]);
            sum_vec = _mm256_fmadd_pd(a_vec, x_vec, sum_vec);
        }

        // 水平求和
        double sum_arr[4];
        _mm256_storeu_pd(sum_arr, sum_vec);
        double sum = sum_arr[0] + sum_arr[1] + sum_arr[2] + sum_arr[3];

        // 处理剩余元素
        for (; j < n; j++) {
            sum += A[i * n + j] * x[j];
        }

        y[i] = sum;
    }
}

// AVX2激活函数（向量化ReLU）
void relu_avx2(double* data, int n) {
    __m256d zero = _mm256_setzero_pd();
    int i = 0;

    for (; i <= n - 4; i += 4) {
        __m256d x = _mm256_loadu_pd(&data[i]);
        __m256d result = _mm256_max_pd(x, zero);
        _mm256_storeu_pd(&data[i], result);
    }

    // 处理剩余
    for (; i < n; i++) {
        data[i] = data[i] > 0 ? data[i] : 0;
    }
}

// AVX2矩阵乘法（分块优化）
void matmul_avx2_blocked(const double* A, const double* B, double* C,
                         int M, int N, int K, int block_size) {
    // 清零C
    memset(C, 0, M * N * sizeof(double));

    for (int ii = 0; ii < M; ii += block_size) {
        for (int jj = 0; jj < N; jj += block_size) {
            for (int kk = 0; kk < K; kk += block_size) {
                // 处理当前块
                int i_end = (ii + block_size < M) ? ii + block_size : M;
                int j_end = (jj + block_size < N) ? jj + block_size : N;
                int k_end = (kk + block_size < K) ? kk + block_size : K;

                for (int i = ii; i < i_end; i++) {
                    for (int k = kk; k < k_end; k++) {
                        __m256d a_val = _mm256_set1_pd(A[i * K + k]);
                        int j = jj;

                        for (; j <= j_end - 4; j += 4) {
                            __m256d c_vec = _mm256_loadu_pd(&C[i * N + j]);
                            __m256d b_vec = _mm256_loadu_pd(&B[k * N + j]);
                            c_vec = _mm256_fmadd_pd(a_val, b_vec, c_vec);
                            _mm256_storeu_pd(&C[i * N + j], c_vec);
                        }

                        // 剩余元素
                        for (; j < j_end; j++) {
                            C[i * N + j] += A[i * K + k] * B[k * N + j];
                        }
                    }
                }
            }
        }
    }
}
#endif

// ARM NEON实现
#ifdef __ARM_NEON
#include <arm_neon.h>

void matvec_mul_neon(const double* A, const double* x, double* y,
                     int m, int n) {
    for (int i = 0; i < m; i++) {
        float64x2_t sum_vec = vdupq_n_f64(0.0);
        int j = 0;

        // NEON一次处理2个double
        for (; j <= n - 2; j += 2) {
            float64x2_t a_vec = vld1q_f64(&A[i * n + j]);
            float64x2_t x_vec = vld1q_f64(&x[j]);
            sum_vec = vfmaq_f64(sum_vec, a_vec, x_vec);
        }

        double sum = vgetq_lane_f64(sum_vec, 0) + vgetq_lane_f64(sum_vec, 1);

        for (; j < n; j++) {
            sum += A[i * n + j] * x[j];
        }

        y[i] = sum;
    }
}
#endif
