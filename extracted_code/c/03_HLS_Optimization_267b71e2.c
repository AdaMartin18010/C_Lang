/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 938
 * Language: c
 * Block ID: 267b71e2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 矩阵乘法优化案例
 * 目标：32x32矩阵，最大化吞吐率
 */

// 版本1：基础实现
void matmul_v1(int A[32][32], int B[32][32], int C[32][32]) {
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
// 性能：32^3 = 32768周期

// 版本2：内层流水线
void matmul_v2(int A[32][32], int B[32][32], int C[32][32]) {
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
// 性能：32*32 + 32 = 1056周期

// 版本3：优化内存访问
void matmul_v3(int A[32][32], int B[32][32], int C[32][32]) {
    #pragma HLS ARRAY_PARTITION variable=A dim=2 cyclic factor=8
    #pragma HLS ARRAY_PARTITION variable=B dim=1 cyclic factor=8

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
// 性能：32*32 + 4 = 1028周期，8个乘法器

// 版本4： systolic数组实现（高级）
void matmul_v4_systolic(int A[32][32], int B[32][32], int C[32][32]) {
    // 使用脉动数组结构
    // 需要重构算法，这里给出概念

    #pragma HLS ARRAY_PARTITION variable=A complete dim=1
    #pragma HLS ARRAY_PARTITION variable=B complete dim=2
    #pragma HLS ARRAY_PARTITION variable=C complete

    // 脉动流水：每个周期计算一个输出元素
    // 空间-时间交换：使用更多资源换取更少时间
}
// 性能：约32周期（理想情况）
