/*
 * Auto-generated from: 12_Practice_Exercises\06_Performance_Optimization.md
 * Line: 142
 * Language: c
 * Block ID: fa508303
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 缓存不友好的实现
void transpose_naive(int *src, int *dst, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dst[j*n + i] = src[i*n + j];  // 写入跨步大
        }
    }
}

// 分块优化 (缓存友好)
void transpose_blocked(int *src, int *dst, int n) {
    const int BLOCK = 64;  // 适合L1缓存

    for (int ii = 0; ii < n; ii += BLOCK) {
        for (int jj = 0; jj < n; jj += BLOCK) {
            for (int i = ii; i < ii + BLOCK && i < n; i++) {
                for (int j = jj; j < jj + BLOCK && j < n; j++) {
                    dst[j*n + i] = src[i*n + j];
                }
            }
        }
    }
}
