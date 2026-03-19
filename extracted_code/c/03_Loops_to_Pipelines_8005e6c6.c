/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\03_Loops_to_Pipelines.md
 * Line: 1059
 * Language: c
 * Block ID: 8005e6c6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 版本1：基础三重循环
void matmul_basic(int A[32][32], int B[32][32], int C[32][32]) {
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            int sum = 0;
            for (int k = 0; k < 32; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}
// 周期：32*32*32 = 32768

// 版本2：内层流水线
void matmul_inner_pipeline(int A[32][32], int B[32][32], int C[32][32]) {
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            #pragma HLS PIPELINE II=1
            int sum = 0;
            for (int k = 0; k < 32; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}
// 周期：32*32 + 32 = 1056，但k循环串行

// 版本3：循环交换 + 展开
void matmul_optimized(int A[32][32], int B[32][32], int C[32][32]) {
    #pragma HLS ARRAY_PARTITION variable=A dim=2 factor=8 cyclic
    #pragma HLS ARRAY_PARTITION variable=B dim=1 factor=8 cyclic
    #pragma HLS ARRAY_PARTITION variable=C dim=0

    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            #pragma HLS PIPELINE II=1

            int sum = 0;
            for (int k = 0; k < 32; k += 8) {
                #pragma HLS UNROLL factor=8
                sum += A[i][k+0] * B[k+0][j] +
                       A[i][k+1] * B[k+1][j] +
                       A[i][k+2] * B[k+2][j] +
                       A[i][k+3] * B[k+3][j] +
                       A[i][k+4] * B[k+4][j] +
                       A[i][k+5] * B[k+5][j] +
                       A[i][k+6] * B[k+6][j] +
                       A[i][k+7] * B[k+7][j];
            }
            C[i][j] = sum;
        }
    }
}
// 周期：32*32 + 4 = 1028，8个并行乘法器

// 版本4：分块矩阵乘法（Cache友好）
void matmul_blocked(int A[32][32], int B[32][32], int C[32][32]) {
    #define BLOCK 8

    for (int ii = 0; ii < 32; ii += BLOCK) {
        for (int jj = 0; jj < 32; jj += BLOCK) {
            for (int kk = 0; kk < 32; kk += BLOCK) {

                // 加载块到本地存储
                int A_block[BLOCK][BLOCK];
                int B_block[BLOCK][BLOCK];
                #pragma HLS ARRAY_PARTITION variable=A_block complete
                #pragma HLS ARRAY_PARTITION variable=B_block complete

                for (int i = 0; i < BLOCK; i++) {
                    for (int k = 0; k < BLOCK; k++) {
                        #pragma HLS PIPELINE
                        A_block[i][k] = A[ii+i][kk+k];
                        B_block[i][k] = B[kk+k][jj+k];
                    }
                }

                // 块内矩阵乘法
                for (int i = 0; i < BLOCK; i++) {
                    for (int j = 0; j < BLOCK; j++) {
                        #pragma HLS PIPELINE II=1
                        int sum = C[ii+i][jj+j];
                        for (int k = 0; k < BLOCK; k++) {
                            #pragma HLS UNROLL
                            sum += A_block[i][k] * B_block[k][j];
                        }
                        C[ii+i][jj+j] = sum;
                    }
                }
            }
        }
    }
}
// 更好的数据局部性，适合大规模矩阵
