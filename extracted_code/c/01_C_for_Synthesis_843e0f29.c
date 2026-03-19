/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 1047
 * Language: c
 * Block ID: 843e0f29
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 矩阵乘法 - 不同优化级别的对比
 */

#define N 32

// 版本1：基础实现
void matmul_basic(int A[N][N], int B[N][N], int C[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}
// 性能：O(N³)周期，资源少，慢

// 版本2：流水线优化
void matmul_pipelined(int A[N][N], int B[N][N], int C[N][N]) {
    #pragma HLS PIPELINE II=1
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            #pragma HLS PIPELINE II=1
            int sum = 0;
            for (int k = 0; k < N; k++) {
                #pragma HLS UNROLL factor=4
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}
// 性能：O(N²)周期，中等资源

// 版本3：完全优化
void matmul_optimized(int A[N][N], int B[N][N], int C[N][N]) {
    #pragma HLS ARRAY_PARTITION variable=A dim=2 factor=8 cyclic
    #pragma HLS ARRAY_PARTITION variable=B dim=1 factor=8 cyclic
    #pragma HLS ARRAY_PARTITION variable=C dim=0

    int A_buffer[N][N];
    int B_buffer[N][N];

    // 加载到本地存储
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            #pragma HLS PIPELINE
            A_buffer[i][j] = A[i][j];
            B_buffer[i][j] = B[i][j];
        }
    }

    // 并行计算
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            #pragma HLS PIPELINE II=1
            int sum = 0;
            for (int k = 0; k < N; k++) {
                #pragma HLS UNROLL factor=8
                sum += A_buffer[i][k] * B_buffer[k][j];
            }
            C[i][j] = sum;
        }
    }
}
// 性能：O(N²/8)周期，8个并行MAC单元
