/*
 * Auto-generated from: 10_WebAssembly_C\04_WASM_Performance_Optimization.md
 * Line: 49
 * Language: c
 * Block ID: 14c0ac35
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// loop_optimization.c
#include <emscripten/emscripten.h>
#include <stdlib.h>
#include <string.h>

// ========== 差的循环实现 ==========

// 1. 边界检查在循环内部（WASM 会添加额外检查）
int sum_unoptimized(int* arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {  // n 每次都需要加载
        sum += arr[i];              // 索引计算 i * 4 + 边界检查
    }
    return sum;
}

// 2. 指针别名问题
void add_arrays_aliased(float* a, float* b, float* c, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = b[i] + c[i];  // 编译器不知道 a,b,c 是否重叠
    }
}

// 3. 非连续内存访问
void process_2d_bad(float** matrix, int rows, int cols) {
    for (int j = 0; j < cols; j++) {      // 列优先 - 缓存不友好
        for (int i = 0; i < rows; i++) {
            matrix[i][j] *= 2.0f;
        }
    }
}

// ========== 优化的循环实现 ==========

// 1. 使用指针遍历
int sum_optimized(int* arr, int n) {
    int sum = 0;
    int* end = arr + n;  // 预计算边界
    for (int* p = arr; p < end; p++) {  // 指针比较比索引快
        sum += *p;                       // 直接解引用
    }
    return sum;
}

// 2. 使用 restrict 关键字（C99）
void add_arrays_restrict(float* restrict a,
                         const float* restrict b,
                         const float* restrict c,
                         int n) {
    for (int i = 0; i < n; i++) {
        a[i] = b[i] + c[i];  // 编译器知道没有别名
    }
}

// 3. 行优先访问 + 循环展开
void process_2d_good(float* matrix, int rows, int cols) {
    // 使用一维数组 + 行优先
    for (int i = 0; i < rows; i++) {
        int row_offset = i * cols;
        // 手动循环展开（4x）
        int j = 0;
        for (; j + 4 <= cols; j += 4) {
            matrix[row_offset + j] *= 2.0f;
            matrix[row_offset + j + 1] *= 2.0f;
            matrix[row_offset + j + 2] *= 2.0f;
            matrix[row_offset + j + 3] *= 2.0f;
        }
        // 处理剩余
        for (; j < cols; j++) {
            matrix[row_offset + j] *= 2.0f;
        }
    }
}

// 4. 使用计数下降（某些架构更快）
int sum_countdown(int* arr, int n) {
    int sum = 0;
    while (n--) {  // 与零比较可能更快
        sum += *arr++;
    }
    return sum;
}

// 5. 分块处理（提高缓存利用率）
#define BLOCK_SIZE 64

void matrix_multiply_blocked(float* C, const float* A, const float* B,
                             int M, int N, int K) {
    for (int i0 = 0; i0 < M; i0 += BLOCK_SIZE) {
        for (int j0 = 0; j0 < N; j0 += BLOCK_SIZE) {
            for (int k0 = 0; k0 < K; k0 += BLOCK_SIZE) {
                // 处理块
                for (int i = i0; i < i0 + BLOCK_SIZE && i < M; i++) {
                    for (int j = j0; j < j0 + BLOCK_SIZE && j < N; j++) {
                        float sum = (k0 == 0) ? 0 : C[i * N + j];
                        for (int k = k0; k < k0 + BLOCK_SIZE && k < K; k++) {
                            sum += A[i * K + k] * B[k * N + j];
                        }
                        C[i * N + j] = sum;
                    }
                }
            }
        }
    }
}
