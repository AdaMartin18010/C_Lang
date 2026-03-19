/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\09_Performance_Optimization.md
 * Line: 451
 * Language: c
 * Block ID: fd85b000
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化1: 循环展开
// 原始
for (int i = 0; i < n; i++) {
    sum += arr[i];
}

// 展开4次
for (int i = 0; i < n; i += 4) {
    sum += arr[i] + arr[i+1] + arr[i+2] + arr[i+3];
}

// 优化2: 循环分块 (矩阵乘法)
void matmul_blocked(float *C, float *A, float *B, int n) {
    const int BLOCK = 32;  // 适应L1缓存

    for (int ii = 0; ii < n; ii += BLOCK) {
        for (int jj = 0; jj < n; jj += BLOCK) {
            for (int kk = 0; kk < n; kk += BLOCK) {
                // 块内计算
                for (int i = ii; i < ii + BLOCK; i++) {
                    for (int j = jj; j < jj + BLOCK; j++) {
                        float sum = C[i*n+j];
                        for (int k = kk; k < kk + BLOCK; k++) {
                            sum += A[i*n+k] * B[k*n+j];
                        }
                        C[i*n+j] = sum;
                    }
                }
            }
        }
    }
}

// 优化3: 循环不变量外提
// 不好
for (int i = 0; i < n; i++) {
    int x = strlen(str);  // 每次都计算
    arr[i] = x;
}

// 好
int x = strlen(str);
for (int i = 0; i < n; i++) {
    arr[i] = x;
}
