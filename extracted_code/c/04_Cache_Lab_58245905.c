/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\04_Cache_Lab.md
 * Line: 584
 * Language: c
 * Block ID: 58245905
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// trans.c
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

// 32x32矩阵转置
void transpose_32x32(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, k;
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    for (i = 0; i < N; i += 8) {
        for (j = 0; j < M; j += 8) {
            for (k = i; k < i + 8; k++) {
                tmp0 = A[k][j];
                tmp1 = A[k][j+1];
                tmp2 = A[k][j+2];
                tmp3 = A[k][j+3];
                tmp4 = A[k][j+4];
                tmp5 = A[k][j+5];
                tmp6 = A[k][j+6];
                tmp7 = A[k][j+7];

                if (i == j) {
                    B[k][j] = tmp0;
                    B[k][j+1] = tmp1;
                    B[k][j+2] = tmp2;
                    B[k][j+3] = tmp3;
                    B[k][j+4] = tmp4;
                    B[k][j+5] = tmp5;
                    B[k][j+6] = tmp6;
                    B[k][j+7] = tmp7;
                } else {
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

            if (i == j) {
                for (k = i; k < i + 8; k++) {
                    for (int l = k + 1; l < i + 8; l++) {
                        int tmp = B[k][l];
                        B[k][l] = B[l][k];
                        B[l][k] = tmp;
                    }
                }
            }
        }
    }
}

// 64x64矩阵转置
void transpose_64x64(int M, int N, int A[N][M], int B[M][N]) {
    // ... 实现同上
}

// 主提交函数
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    if (M == 32 && N == 32) {
        transpose_32x32(M, N, A, B);
    } else if (M == 64 && N == 64) {
        transpose_64x64(M, N, A, B);
    } else if (M == 61 && N == 67) {
        transpose_61x67(M, N, A, B);
    }
}
