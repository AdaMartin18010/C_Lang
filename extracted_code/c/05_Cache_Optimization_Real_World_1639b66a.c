/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\05_Cache_Optimization_Real_World.md
 * Line: 184
 * Language: c
 * Block ID: 1639b66a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 多级分块 - 利用L1、L2、L3缓存
void matmul_multilevel(const float* A, const float* B, float* C, int N) {
    // L3 Cache块 (假设L3 = 32MB)
    const int L3_BLOCK = 512;  // 512*512*4*3 = 3MB < 32MB

    // L2 Cache块 (假设L2 = 1MB per core)
    const int L2_BLOCK = 128;  // 128*128*4*3 = 192KB < 1MB

    // L1 Cache块 (假设L1 = 32KB)
    const int L1_BLOCK = 32;   // 32*32*4*3 = 12KB < 32KB

    for (int i3 = 0; i3 < N; i3 += L3_BLOCK) {
        for (int j3 = 0; j3 < N; j3 += L3_BLOCK) {
            for (int k3 = 0; k3 < N; k3 += L3_BLOCK) {

                for (int i2 = i3; i2 < min(i3 + L3_BLOCK, N); i2 += L2_BLOCK) {
                    for (int j2 = j3; j2 < min(j3 + L3_BLOCK, N); j2 += L2_BLOCK) {
                        for (int k2 = k3; k2 < min(k3 + L3_BLOCK, N); k2 += L2_BLOCK) {

                            for (int i1 = i2; i1 < min(i2 + L2_BLOCK, N); i1 += L1_BLOCK) {
                                for (int j1 = j2; j1 < min(j2 + L2_BLOCK, N); j1 += L1_BLOCK) {
                                    for (int k1 = k2; k1 < min(k2 + L2_BLOCK, N); k1 += L1_BLOCK) {

                                        // L1大小的内核计算
                                        kernel_l1(&A[i1 * N + k1],
                                                 &B[k1 * N + j1],
                                                 &C[i1 * N + j1],
                                                 N, L1_BLOCK);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

/* 性能对比 (N=4096, 双路Xeon):
 * 朴素:      245s,   0.56 GFLOPS
 * 单级分块:   18.5s,  7.4 GFLOPS
 * 多级分块:   6.2s,   22.1 GFLOPS (39x加速)
 * OpenBLAS:   4.1s,   33.4 GFLOPS
 */
