/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\07_Performance_Optimization.md
 * Line: 348
 * Language: c
 * Block ID: 2fc4c217
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 朴素实现 - 大量缓存未命中
void transpose_naive(float *dst, const float *src, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dst[j * n + i] = src[i * n + j];
        }
    }
}

// ✅ 分块转置 - 缓存友好
#define BLOCK_SIZE 32
void transpose_blocked(float *dst, const float *src, int n) {
    for (int ii = 0; ii < n; ii += BLOCK_SIZE) {
        for (int jj = 0; jj < n; jj += BLOCK_SIZE) {
            for (int i = ii; i < min(ii + BLOCK_SIZE, n); i++) {
                for (int j = jj; j < min(jj + BLOCK_SIZE, n); j++) {
                    dst[j * n + i] = src[i * n + j];
                }
            }
        }
    }
}
