/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\README.md
 * Line: 590
 * Language: c
 * Block ID: 8d4a28ac
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 问题: 矩阵乘法缓存不友好
void matmul_naive(float* A, float* B, float* C, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++)
                C[i*n+j] += A[i*n+k] * B[k*n+j];  // B 列访问不连续!
}

// 优化: 循环分块
#define BLOCK_SIZE 32  // 适合 L1 缓存

void matmul_tiled(float* A, float* B, float* C, int n) {
    for (int ii = 0; ii < n; ii += BLOCK_SIZE)
        for (int jj = 0; jj < n; jj += BLOCK_SIZE)
            for (int kk = 0; kk < n; kk += BLOCK_SIZE)
                // 处理 BLOCK_SIZE × BLOCK_SIZE 子块
                for (int i = ii; i < min(ii+BLOCK_SIZE, n); i++)
                    for (int j = jj; j < min(jj+BLOCK_SIZE, n); j++)
                        for (int k = kk; k < min(kk+BLOCK_SIZE, n); k++)
                            C[i*n+j] += A[i*n+k] * B[k*n+j];
}
