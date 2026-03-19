/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\04_Cache_Lab.md
 * Line: 389
 * Language: c
 * Block ID: f53bad72
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 8x8分块，利用局部性
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, k, l;
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    for (i = 0; i < N; i += 8) {
        for (j = 0; j < M; j += 8) {
            for (k = i; k < i + 8; k++) {
                // 一次性读取A的一行8个元素
                tmp0 = A[k][j];
                tmp1 = A[k][j+1];
                tmp2 = A[k][j+2];
                tmp3 = A[k][j+3];
                tmp4 = A[k][j+4];
                tmp5 = A[k][j+5];
                tmp6 = A[k][j+6];
                tmp7 = A[k][j+7];

                // 写入B的列（在分块内按行存储）
                B[j][k] = tmp0;
                B[j+1][k] = tmp1;
                B[j+2][k] = tmp2;
                B[j+3][k] = tmp3;
                B[j+4][k] = tmp4;
                B[j+5][k] = tmp5;
                B[j+6][k] = tmp6;
                B[j+7][k] = tmp7;
            }
        }
    }
}
