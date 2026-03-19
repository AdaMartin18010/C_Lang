/*
 * Auto-generated from: 11_Machine_Learning_C\01_Neural_Network_Basics.md
 * Line: 689
 * Language: c
 * Block ID: 0dbf167b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef MATRIX_OPS_H
#define MATRIX_OPS_H

#include <stdlib.h>
#include <string.h>
#include <math.h>

// 矩阵结构（行优先存储）
typedef struct {
    int rows;
    int cols;
    double* data;  // data[i * cols + j] = M[i][j]
} Matrix;

// 创建矩阵
static inline Matrix* matrix_create(int rows, int cols) {
    Matrix* m = malloc(sizeof(Matrix));
    m->rows = rows;
    m->cols = cols;
    m->data = calloc(rows * cols, sizeof(double));
    return m;
}

// 释放矩阵
static inline void matrix_free(Matrix* m) {
    if (m) {
        free(m->data);
        free(m);
    }
}

// 获取元素
static inline double matrix_get(const Matrix* m, int i, int j) {
    return m->data[i * m->cols + j];
}

// 设置元素
static inline void matrix_set(Matrix* m, int i, int j, double val) {
    m->data[i * m->cols + j] = val;
}

// 矩阵乘法 C = A * B
// A: [m x k], B: [k x n], C: [m x n]
void matrix_multiply(const Matrix* A, const Matrix* B, Matrix* C) {
    int m = A->rows, k = A->cols, n = B->cols;
    memset(C->data, 0, m * n * sizeof(double));

    // 基础三重循环
    for (int i = 0; i < m; i++) {
        for (int l = 0; l < k; l++) {
            double a_il = matrix_get(A, i, l);
            for (int j = 0; j < n; j++) {
                C->data[i * n + j] += a_il * matrix_get(B, l, j);
            }
        }
    }
}

// 矩阵-向量乘法: y = A * x
// A: [m x n], x: [n], y: [m]
void matrix_vector_multiply(const Matrix* A, const double* x, double* y) {
    int m = A->rows, n = A->cols;
    for (int i = 0; i < m; i++) {
        y[i] = 0.0;
        for (int j = 0; j < n; j++) {
            y[i] += matrix_get(A, i, j) * x[j];
        }
    }
}

// 矩阵转置: B = A^T
void matrix_transpose(const Matrix* A, Matrix* B) {
    for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->cols; j++) {
            matrix_set(B, j, i, matrix_get(A, i, j));
        }
    }
}

// 逐元素加法: C = A + B
void matrix_add(const Matrix* A, const Matrix* B, Matrix* C) {
    int size = A->rows * A->cols;
    for (int i = 0; i < size; i++) {
        C->data[i] = A->data[i] + B->data[i];
    }
}

// 逐元素乘法 (Hadamard积): C = A ⊙ B
void matrix_hadamard(const Matrix* A, const Matrix* B, Matrix* C) {
    int size = A->rows * A->cols;
    for (int i = 0; i < size; i++) {
        C->data[i] = A->data[i] * B->data[i];
    }
}

// 标量乘法: B = alpha * A
void matrix_scale(const Matrix* A, double alpha, Matrix* B) {
    int size = A->rows * A->cols;
    for (int i = 0; i < size; i++) {
        B->data[i] = alpha * A->data[i];
    }
}

// 外积: C = a * b^T, a: [m], b: [n], C: [m x n]
void matrix_outer_product(const double* a, int m, const double* b, int n, Matrix* C) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            matrix_set(C, i, j, a[i] * b[j]);
        }
    }
}

// 打印矩阵
void matrix_print(const Matrix* m, const char* name) {
    printf("Matrix %s [%d x %d]:\n", name, m->rows, m->cols);
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->cols; j++) {
            printf("%10.6f ", matrix_get(m, i, j));
        }
        printf("\n");
    }
    printf("\n");
}

// 随机初始化 (Xavier/He)
typedef enum { INIT_XAVIER, INIT_HE, INIT_UNIFORM } InitType;

void matrix_init_random(Matrix* m, InitType type) {
    int n_in = m->cols;
    int n_out = m->rows;
    double scale = 1.0;

    switch (type) {
        case INIT_XAVIER:  // tanh/sigmoid用
            scale = sqrt(6.0 / (n_in + n_out));
            break;
        case INIT_HE:      // ReLU用
            scale = sqrt(2.0 / n_in);
            break;
        case INIT_UNIFORM:
            scale = 0.5;
            break;
    }

    for (int i = 0; i < m->rows * m->cols; i++) {
        double u = (double)rand() / RAND_MAX;  // [0, 1)
        m->data[i] = (2.0 * u - 1.0) * scale;   // [-scale, scale)
    }
}

#endif
