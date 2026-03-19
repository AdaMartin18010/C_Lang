/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\05_Cache_Optimization_Real_World.md
 * Line: 112
 * Language: c
 * Block ID: d65c6eb7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 朴素矩阵乘法 - 缓存不友好
void matmul_naive(const float* A, const float* B, float* C, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float sum = 0.0f;
            for (int k = 0; k < N; k++) {
                // A访问连续，但B访问跨度N
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}

/* 访问模式分析 (N=1024):
 *
 * A: 顺序访问，每次缓存行命中16个float
 *    缓存效率: 高
 *
 * B: 列访问，每次跳跃N*sizeof(float)=4KB
 *    L1缓存: 只有4个条目 (32KB/8-way)，无法保持整列
 *    缓存效率: 极低，每次访问都是冷未命中
 *
 * 缓存未命中次数: O(N^3)，每次k迭代都未命中
 */
