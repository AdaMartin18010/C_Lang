/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 911
 * Language: c
 * Block ID: 125905c9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 示例7：VLA（变长数组）的_Countof使用
 * 演示运行时数组大小计算
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 使用VLA的矩阵乘法
void matrix_multiply_vla(size_t m, size_t n, size_t p,
                         double A[m][n], double B[n][p], double C[m][p]) {
    // 使用_Countof验证维度（调试时）
    #ifdef DEBUG
    printf("Matrix A: %zu x %zu\n", _Countof(A), _Countof(A[0]));
    printf("Matrix B: %zu x %zu\n", _Countof(B), _Countof(B[0]));
    printf("Matrix C: %zu x %zu\n", _Countof(C), _Countof(C[0]));
    #endif

    // 初始化结果矩阵
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < p; j++) {
            C[i][j] = 0.0;
        }
    }

    // 矩阵乘法
    for (size_t i = 0; i < m; i++) {
        for (size_t k = 0; k < n; k++) {
            for (size_t j = 0; j < p; j++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// 打印VLA矩阵
void print_matrix_vla(size_t rows, size_t cols, double matrix[rows][cols]) {
    printf("Matrix (%zux%zu):\n", _Countof(matrix), _Countof(matrix[0]));
    for (size_t i = 0; i < rows; i++) {
        printf("  [");
        for (size_t j = 0; j < cols; j++) {
            printf("%8.2f", matrix[i][j]);
        }
        printf("  ]\n");
    }
}

// 处理任意大小的数据块
void process_data_block(size_t n, double data[n]) {
    printf("Processing %zu data points\n", _Countof(data));

    double sum = 0.0;
    for (size_t i = 0; i < _Countof(data); i++) {
        sum += data[i];
    }

    printf("  Sum: %.2f\n", sum);
    printf("  Average: %.2f\n", sum / _Countof(data));
}

int main(void) {
    srand((unsigned)time(NULL));

    // 一维VLA示例
    size_t n = 10;
    double data[n];

    printf("=== 1D VLA Example ===\n");
    printf("Array declared with size %zu\n", n);
    printf("_Countof returns: %zu\n\n", _Countof(data));

    for (size_t i = 0; i < n; i++) {
        data[i] = (double)(rand() % 100);
    }
    process_data_block(n, data);

    // 二维VLA示例
    printf("\n=== 2D VLA Example ===\n");
    size_t m = 3, p = 4;
    double A[m][n], B[n][p], C[m][p];

    // 初始化矩阵
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            A[i][j] = (double)(rand() % 10);
        }
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < p; j++) {
            B[i][j] = (double)(rand() % 10);
        }
    }

    printf("Matrix A dimensions: %zu x %zu\n", _Countof(A), _Countof(A[0]));
    printf("Matrix B dimensions: %zu x %zu\n", _Countof(B), _Countof(B[0]));

    matrix_multiply_vla(m, n, p, A, B, C);

    printf("\nResult Matrix C:\n");
    print_matrix_vla(m, p, C);

    return 0;
}
