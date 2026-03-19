/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\04_Cache_Lab.md
 * Line: 564
 * Language: c
 * Block ID: e3a8fee4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void transpose_61x67(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, k, l;
    int block_size = 16;  // 使用16x16分块

    for (i = 0; i < N; i += block_size) {
        for (j = 0; j < M; j += block_size) {
            // 处理每个块，注意边界
            for (k = i; k < i + block_size && k < N; k++) {
                for (l = j; l < j + block_size && l < M; l++) {
                    B[l][k] = A[k][l];
                }
            }
        }
    }
}
