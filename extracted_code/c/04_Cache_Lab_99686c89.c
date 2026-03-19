/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\04_Cache_Lab.md
 * Line: 347
 * Language: c
 * Block ID: 99686c89
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 原始转置实现
void trans(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}
