/*
 * Auto-generated from: 11_Machine_Learning_C\01_Neural_Network_Basics.md
 * Line: 1670
 * Language: c
 * Block ID: f67fb0b1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 分块矩阵乘法 - 优化缓存局部性
 * 策略:
 * 1. 将大矩阵分成适合L1/L2缓存的小块
 * 2. 对每个块进行常规矩阵乘法
 * 3. 利用空间局部性和时间局部性
 */

#define BLOCK_SIZE 64  // 根据CPU缓存大小调整

void matmul_blocked(const double* A, const double* B, double* C,
                    int M, int N, int K) {
    // 清零结果矩阵
    memset(C, 0, M * N * sizeof(double));

    // 分块遍历
    for (int ii = 0; ii < M; ii += BLOCK_SIZE) {
        for (int jj = 0; jj < N; jj += BLOCK_SIZE) {
            for (int kk = 0; kk < K; kk += BLOCK_SIZE) {

                // 计算当前块的边界
                int i_max = (ii + BLOCK_SIZE < M) ? ii + BLOCK_SIZE : M;
                int j_max = (jj + BLOCK_SIZE < N) ? jj + BLOCK_SIZE : N;
                int k_max = (kk + BLOCK_SIZE < K) ? kk + BLOCK_SIZE : K;

                // 块内矩阵乘法（内核）
                for (int i = ii; i < i_max; i++) {
                    for (int k = kk; k < k_max; k++) {
                        double a_ik = A[i * K + k];
                        // 展开内层循环以提高性能
                        int j = jj;
                        for (; j <= j_max - 8; j += 8) {
                            C[i * N + j]     += a_ik * B[k * N + j];
                            C[i * N + j + 1] += a_ik * B[k * N + j + 1];
                            C[i * N + j + 2] += a_ik * B[k * N + j + 2];
                            C[i * N + j + 3] += a_ik * B[k * N + j + 3];
                            C[i * N + j + 4] += a_ik * B[k * N + j + 4];
                            C[i * N + j + 5] += a_ik * B[k * N + j + 5];
                            C[i * N + j + 6] += a_ik * B[k * N + j + 6];
                            C[i * N + j + 7] += a_ik * B[k * N + j + 7];
                        }
                        for (; j < j_max; j++) {
                            C[i * N + j] += a_ik * B[k * N + j];
                        }
                    }
                }
            }
        }
    }
}
