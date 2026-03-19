/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\01_Cycle_Accurate_Semantics.md
 * Line: 129
 * Language: c
 * Block ID: 28d9c427
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单实现：缓存不友好
void transpose_naive(int *dst, const int *src, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dst[j * n + i] = src[i * n + j];  // dst列写入跳跃
        }
    }
}

// 分块优化：提高缓存利用率
define BLOCK 64  // 适应L1缓存大小

void transpose_blocked(int *dst, const int *src, int n) {
    for (int ii = 0; ii < n; ii += BLOCK) {
        for (int jj = 0; jj < n; jj += BLOCK) {
            // 处理BLOCK x BLOCK小块
            for (int i = ii; i < ii + BLOCK && i < n; i++) {
                for (int j = jj; j < jj + BLOCK && j < n; j++) {
                    dst[j * n + i] = src[i * n + j];
                }
            }
        }
    }
}
