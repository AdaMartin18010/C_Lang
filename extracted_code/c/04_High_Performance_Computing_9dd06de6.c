/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\04_High_Performance_Computing.md
 * Line: 238
 * Language: c
 * Block ID: 9dd06de6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 矩阵转置优化

// ❌ 缓存不友好（列访问）
void transpose_naive(float *dst, const float *src, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dst[j*n + i] = src[i*n + j];  // 列写入，缓存缺失
        }
    }
}

// ✅ 分块优化
#define BLOCK_SIZE 64

void transpose_blocked(float *dst, const float *src, int n) {
    for (int ii = 0; ii < n; ii += BLOCK_SIZE) {
        for (int jj = 0; jj < n; jj += BLOCK_SIZE) {
            // 处理BLOCK_SIZE x BLOCK_SIZE块
            for (int i = ii; i < ii + BLOCK_SIZE && i < n; i++) {
                for (int j = jj; j < jj + BLOCK_SIZE && j < n; j++) {
                    dst[j*n + i] = src[i*n + j];
                }
            }
        }
    }
}

// 矩阵乘法分块
void matmul_blocked(float *C, const float *A, const float *B,
                    int M, int N, int K) {
    for (int ii = 0; ii < M; ii += BLOCK_M) {
        for (int jj = 0; jj < N; jj += BLOCK_N) {
            for (int kk = 0; kk < K; kk += BLOCK_K) {
                // 计算块乘法
                for (int i = ii; i < min(ii + BLOCK_M, M); i++) {
                    for (int j = jj; j < min(jj + BLOCK_N, N); j++) {
                        float sum = C[i*N + j];
                        for (int k = kk; k < min(kk + BLOCK_K, K); k++) {
                            sum += A[i*K + k] * B[k*N + j];
                        }
                        C[i*N + j] = sum;
                    }
                }
            }
        }
    }
}
