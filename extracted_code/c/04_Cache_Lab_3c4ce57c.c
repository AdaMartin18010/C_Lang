/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\04_Cache_Lab.md
 * Line: 495
 * Language: c
 * Block ID: 3c4ce57c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void transpose_64x64(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, k;
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    for (i = 0; i < N; i += 8) {
        for (j = 0; j < M; j += 8) {
            // 处理上半4行
            for (k = i; k < i + 4; k++) {
                tmp0 = A[k][j];
                tmp1 = A[k][j+1];
                tmp2 = A[k][j+2];
                tmp3 = A[k][j+3];
                tmp4 = A[k][j+4];
                tmp5 = A[k][j+5];
                tmp6 = A[k][j+6];
                tmp7 = A[k][j+7];

                // 先存到B的左上和右上
                B[j][k] = tmp0;
                B[j+1][k] = tmp1;
                B[j+2][k] = tmp2;
                B[j+3][k] = tmp3;
                B[j][k+4] = tmp4;    // 暂存位置
                B[j+1][k+4] = tmp5;
                B[j+2][k+4] = tmp6;
                B[j+3][k+4] = tmp7;
            }

            // 处理下半4行，同时完成上半的转置
            for (k = i + 4; k < i + 8; k++) {
                // 读取A的下半
                tmp0 = A[k][j];
                tmp1 = A[k][j+1];
                tmp2 = A[k][j+2];
                tmp3 = A[k][j+3];
                tmp4 = A[k][j+4];
                tmp5 = A[k][j+5];
                tmp6 = A[k][j+6];
                tmp7 = A[k][j+7];

                // 读取之前暂存的数据
                int t0 = B[j][k];
                int t1 = B[j+1][k];
                int t2 = B[j+2][k];
                int t3 = B[j+3][k];

                // 写入正确位置
                B[j][k] = tmp0;
                B[j+1][k] = tmp1;
                B[j+2][k] = tmp2;
                B[j+3][k] = tmp3;
                B[j+4][k-4] = t0;
                B[j+5][k-4] = t1;
                B[j+6][k-4] = t2;
                B[j+7][k-4] = t3;

                B[j+4][k] = tmp4;
                B[j+5][k] = tmp5;
                B[j+6][k] = tmp6;
                B[j+7][k] = tmp7;
            }
        }
    }
}
