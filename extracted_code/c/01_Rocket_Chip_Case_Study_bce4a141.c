/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\01_Rocket_Chip_Case_Study.md
 * Line: 926
 * Language: c
 * Block ID: bce4a141
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化前：缓存不友好的矩阵乘法
void matmul_naive(float* A, float* B, float* C, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float sum = 0;
            for (int k = 0; k < N; k++) {
                sum += A[i*N + k] * B[k*N + j];  // B的访问不连续
            }
            C[i*N + j] = sum;
        }
    }
}

// 优化后：分块+缓存优化
void matmul_optimized(float* A, float* B, float* C, int N) {
    const int BLOCK = 32;  // 适合L1 Cache大小

    for (int ii = 0; ii < N; ii += BLOCK) {
        for (int jj = 0; jj < N; jj += BLOCK) {
            for (int kk = 0; kk < N; kk += BLOCK) {
                // 处理一个块
                for (int i = ii; i < min(ii + BLOCK, N); i++) {
                    for (int j = jj; j < min(jj + BLOCK, N); j++) {
                        float sum = (kk == 0) ? 0 : C[i*N + j];
                        for (int k = kk; k < min(kk + BLOCK, N); k++) {
                            sum += A[i*N + k] * B[k*N + j];
                        }
                        C[i*N + j] = sum;
                    }
                }
            }
        }
    }
}
