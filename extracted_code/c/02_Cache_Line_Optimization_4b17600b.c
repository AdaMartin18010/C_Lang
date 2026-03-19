/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\02_Cache_Line_Optimization.md
 * Line: 261
 * Language: c
 * Block ID: 4b17600b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 行优先 (C默认) - 行访问快
float matrix_row_major[N][N];

// 快: 行访问，缓存友好
for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
        sum += matrix_row_major[i][j];  // 连续访问
    }
}

// 慢: 列访问，缓存不友好
for (int j = 0; j < N; j++) {
    for (int i = 0; i < N; i++) {
        sum += matrix_row_major[i][j];  // 跨行访问，步长大
    }
}

// 解决方案：分块访问
#define BLOCK_SIZE 64
for (int ii = 0; ii < N; ii += BLOCK_SIZE) {
    for (int jj = 0; jj < N; jj += BLOCK_SIZE) {
        for (int i = ii; i < ii + BLOCK_SIZE && i < N; i++) {
            for (int j = jj; j < jj + BLOCK_SIZE && j < N; j++) {
                sum += matrix_row_major[i][j];
            }
        }
    }
}
