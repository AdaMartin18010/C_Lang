/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\05_Cache_Optimization_Real_World.md
 * Line: 142
 * Language: c
 * Block ID: 206a8cc7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 分块矩阵乘法 - 缓存优化
void matmul_blocked(const float* A, const float* B, float* C, int N) {
    // BLOCK选择为L1 Cache能容纳的大小
    // L1 = 32KB = 8192 float
    // 3个BLOCK: A块 + B块 + C块 = 3 * BLOCK^2 * 4 bytes < 32KB
    // BLOCK^2 < 2730 => BLOCK <= 52, 取32或64
    const int BLOCK = 64;

    for (int ii = 0; ii < N; ii += BLOCK) {
        for (int jj = 0; jj < N; jj += BLOCK) {
            for (int kk = 0; kk < N; kk += BLOCK) {
                // 处理BLOCK x BLOCK子矩阵
                int i_max = (ii + BLOCK < N) ? ii + BLOCK : N;
                int j_max = (jj + BLOCK < N) ? jj + BLOCK : N;
                int k_max = (kk + BLOCK < N) ? kk + BLOCK : N;

                for (int i = ii; i < i_max; i++) {
                    for (int j = jj; j < j_max; j++) {
                        float sum = (kk == 0) ? 0.0f : C[i * N + j];

                        // 现在A和B块都驻留在缓存中
                        for (int k = kk; k < k_max; k++) {
                            sum += A[i * N + k] * B[k * N + j];
                        }
                        C[i * N + j] = sum;
                    }
                }
            }
        }
    }
}

/* 性能提升:
 * N=1024时:
 * 朴素版本: 2450 ms, 0.44 GFLOPS
 * 分块版本:  185 ms, 5.8 GFLOPS  (13x加速)
 */
