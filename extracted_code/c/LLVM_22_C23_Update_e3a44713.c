/*
 * Auto-generated from: 00_VERSION_TRACKING\LLVM_22_C23_Update.md
 * Line: 1274
 * Language: c
 * Block ID: e3a44713
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// c2y_features_demo.c
// 综合演示 Clang 22 支持的 C2y 特性

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// 使用 __COUNTER__ 生成唯一标识符
#define UNIQUE_ID(prefix) prefix##__COUNTER__
#define REGISTER_COMPONENT(name) \
    static const int UNIQUE_ID(_component_id_) = __COUNTER__; \
    static void UNIQUE_ID(_init_)(void) __attribute__((constructor)); \
    static void UNIQUE_ID(_init_)(void) { \
        printf("Initializing component: %s (ID: %d)\n", \
               name, UNIQUE_ID(_component_id_)); \
    }

// 注册组件
REGISTER_COMPONENT("Network")
REGISTER_COMPONENT("Storage")
REGISTER_COMPONENT("Security")

// static_assert 无 UB 示例
static_assert(sizeof(void*) == 8, "64-bit system required");
static_assert(1 + 1 == 2, "Basic arithmetic works");

// extern inline 调用 static inline
static inline uint32_t rotate_left(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

extern inline uint32_t hash_mix(uint32_t a, uint32_t b) {
    // C2y: 允许 extern inline 调用 static inline
    a = rotate_left(a, 13);
    b = rotate_left(b, 27);
    return a ^ b;
}

// 在另一个文件中定义外部版本
extern inline uint32_t hash_mix(uint32_t a, uint32_t b);

// Named Loops 实现的高效算法

// 矩阵乘法优化
void matrix_multiply(int n, const float A[n][n],
                     const float B[n][n],
                     float C[n][n]) {
    // 初始化结果矩阵
    init: for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0.0f;
        }
    }

    // 三重循环乘法，使用命名循环优化
    i_loop: for (int i = 0; i < n; i++) {
        j_loop: for (int j = 0; j < n; j++) {
            float sum = 0.0f;

            k_loop: for (int k = 0; k < n; k++) {
                // 检测 NaN，提前终止
                if (A[i][k] != A[i][k]) {
                    C[i][j] = 0.0f / 0.0f;  // NaN
                    continue j_loop;  // 跳过当前 j
                }

                sum += A[i][k] * B[k][j];

                // 溢出检测
                if (sum > 1e38f) {
                    break i_loop;  // 终止整个计算
                }
            }

            C[i][j] = sum;
        }
    }
}

// 图遍历算法
void dfs_traversal(int n, bool adjacency[n][n],
                   int start, bool visited[n]) {
    int stack[n];
    int top = 0;

    stack[top++] = start;
    visited[start] = true;

    traverse: while (top > 0) {
        int node = stack[--top];

        process_node: for (int i = 0; i < n; i++) {
            if (!adjacency[node][i] || visited[i]) {
                continue process_node;
            }

            visited[i] = true;

            // 栈溢出检查
            if (top >= n) {
                fprintf(stderr, "Stack overflow in DFS\n");
                break traverse;
            }

            stack[top++] = i;
        }
    }
}

// 使用 Named Loops 的错误处理
bool validate_data(int rows, int cols, double data[rows][cols]) {
    row_check: for (int i = 0; i < rows; i++) {
        col_check: for (int j = 0; j < cols; j++) {
            if (data[i][j] < 0.0) {
                fprintf(stderr, "Negative value at (%d, %d)\n", i, j);
                continue row_check;  // 跳过整行
            }

            if (data[i][j] != data[i][j]) {
                fprintf(stderr, "NaN value at (%d, %d)\n", i, j);
                return false;  // 立即失败
            }
        }
    }
    return true;
}

int main(void) {
    printf("C2y Features Demo\n");
    printf("=================\n\n");

    // 演示 __COUNTER__
    printf("__COUNTER__ values: %d, %d, %d\n",
           __COUNTER__, __COUNTER__, __COUNTER__);

    // 演示 hash_mix
    uint32_t h = hash_mix(0x12345678, 0x9ABCDEF0);
    printf("Hash mix result: 0x%08X\n", h);

    // 演示矩阵乘法
    float A[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    float B[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    float C[3][3];

    matrix_multiply(3, A, B, C);

    printf("\nMatrix multiplication result:\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%6.1f ", C[i][j]);
        }
        printf("\n");
    }

    return 0;
}
