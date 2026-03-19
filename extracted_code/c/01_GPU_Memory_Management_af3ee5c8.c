/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_GPU_Memory_Management.md
 * Line: 286
 * Language: c
 * Block ID: af3ee5c8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用共享内存的矩阵乘法示例
__global__ void matrixMulShared(float *C, float *A, float *B, int width) {
    __shared__ float As[16][16];
    __shared__ float Bs[16][16];

    int bx = blockIdx.x, by = blockIdx.y;
    int tx = threadIdx.x, ty = threadIdx.y;

    int row = by * 16 + ty;
    int col = bx * 16 + tx;

    float Cvalue = 0;

    // 分块计算
    for (int m = 0; m < width / 16; m++) {
        // 协作加载A和B到共享内存
        As[ty][tx] = A[row * width + (m * 16 + tx)];
        Bs[ty][tx] = B[(m * 16 + ty) * width + col];

        __syncthreads();  // 等待所有线程加载完成

        // 计算部分结果
        for (int k = 0; k < 16; k++) {
            Cvalue += As[ty][k] * Bs[k][tx];
        }

        __syncthreads();  // 确保计算完成再加载下一块
    }

    C[row * width + col] = Cvalue;
}
