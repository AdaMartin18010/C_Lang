/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\05_Cache_Optimization_Real_World.md
 * Line: 234
 * Language: c
 * Block ID: 082f5c6b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 转置B矩阵以获得更好的访问模式
void matmul_transpose(const float* A, const float* B, float* C, int N) {
    // 分配转置缓冲区
    float* BT = aligned_alloc(64, N * N * sizeof(float));

    // 转置B
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            BT[j * N + i] = B[i * N + j];
        }
    }

    // 乘法 (现在A和BT都是行优先)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float sum = 0.0f;
            for (int k = 0; k < N; k++) {
                // 现在两个访问都是连续的
                sum += A[i * N + k] * BT[j * N + k];
            }
            C[i * N + j] = sum;
        }
    }

    free(BT);
}

// 性能: 比朴素版本快2-3x，但比分块版本慢
// 最佳实践: 分块+转置结合使用
