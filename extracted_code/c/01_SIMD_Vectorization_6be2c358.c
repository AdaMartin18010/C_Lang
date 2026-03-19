/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 2249
 * Language: c
 * Block ID: 6be2c358
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 列优先访问行优先存储矩阵
for (int j = 0; j < n; j++) {
    for (int i = 0; i < m; i++) {
        sum += A[i][j];  // 跨行访问，缓存未命中
    }
}

// ✅ 正确: 行优先访问
for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
        sum += A[i][j];  // 连续访问，缓存友好
    }
}

// ✅ 优化: 分块访问
#define BLOCK_SIZE 64
for (int ii = 0; ii < m; ii += BLOCK_SIZE) {
    for (int jj = 0; jj < n; jj += BLOCK_SIZE) {
        for (int i = ii; i < min(ii + BLOCK_SIZE, m); i++) {
            for (int j = jj; j < min(jj + BLOCK_SIZE, n); j++) {
                sum += A[i][j];
            }
        }
    }
}
