/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\07_Performance_Optimization.md
 * Line: 290
 * Language: c
 * Block ID: 50490dd3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 原始循环 - 多重嵌套
for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
        for (int k = 0; k < P; k++) {
            C[i][j] += A[i][k] * B[k][j];
        }
    }
}

// ✅ 循环分块 (Loop Tiling) - 提高缓存命中率
#define BLOCK 32
for (int ii = 0; ii < N; ii += BLOCK) {
    for (int jj = 0; jj < M; jj += BLOCK) {
        for (int kk = 0; kk < P; kk += BLOCK) {
            for (int i = ii; i < min(ii + BLOCK, N); i++) {
                for (int j = jj; j < min(jj + BLOCK, M); j++) {
                    for (int k = kk; k < min(kk + BLOCK, P); k++) {
                        C[i][j] += A[i][k] * B[k][j];
                    }
                }
            }
        }
    }
}

// ✅ 循环展开减少开销
for (int i = 0; i < N; i += 4) {
    sum += arr[i];
    sum += arr[i+1];
    sum += arr[i+2];
    sum += arr[i+3];
}
