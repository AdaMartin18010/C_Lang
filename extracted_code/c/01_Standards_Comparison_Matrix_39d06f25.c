/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 2870
 * Language: c
 * Block ID: 39d06f25
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ========================================
 * C89 版本：传统实现
 * ======================================== */
#ifdef USE_C89

#include <stdlib.h>
#include <string.h>

/* C89: 使用一维数组模拟二维数组 */
typedef struct {
    double* data;
    int rows;
    int cols;
} Matrix89;

Matrix89* matrix_create(int rows, int cols) {
    Matrix89* m = malloc(sizeof(Matrix89));
    if (m == NULL) return NULL;

    m->data = malloc(rows * cols * sizeof(double));
    if (m->data == NULL) {
        free(m);
        return NULL;
    }

    m->rows = rows;
    m->cols = cols;
    memset(m->data, 0, rows * cols * sizeof(double));

    return m;
}

void matrix_destroy(Matrix89* m) {
    if (m) {
        free(m->data);
        free(m);
    }
}

/* 访问元素：m[i][j] = m->data[i * cols + j] */
#define M(m, i, j) ((m)->data[(i) * (m)->cols + (j)])

int matrix_multiply(const Matrix89* a, const Matrix89* b, Matrix89* result) {
    int i, j, k;

    if (a->cols != b->rows || result->rows != a->rows || result->cols != b->cols) {
        return -1;
    }

    for (i = 0; i < a->rows; i++) {
        for (j = 0; j < b->cols; j++) {
            double sum = 0.0;
            for (k = 0; k < a->cols; k++) {
                sum += M(a, i, k) * M(b, k, j);
            }
            M(result, i, j) = sum;
        }
    }

    return 0;
}

/* ========================================
 * C99 版本：VLA支持
 * ======================================== */
#elif defined(USE_C99)

#include <stdio.h>

/* C99: 使用VLA */
void matrix_multiply_vla(int m, int n, int p,
                         const double a[m][n],
                         const double b[n][p],
                         double result[m][p]) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += a[i][k] * b[k][j];
            }
            result[i][j] = sum;
        }
    }
}

/* ========================================
 * C11 版本：restrict优化
 * ======================================== */
#elif defined(USE_C11)

/* C11: 使用restrict指针优化 */
void matrix_multiply_restrict(int m, int n, int p,
                              const double* restrict a,
                              const double* restrict b,
                              double* restrict result) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += a[i * n + k] * b[k * p + j];
            }
            result[i * p + j] = sum;
        }
    }
}

/* ========================================
 * C23 版本：类型推导和constexpr
 * ======================================== */
#elif defined(USE_C23)

/* C23: 使用constexpr和类型推导 */
constexpr int TILE_SIZE = 32;

void matrix_multiply_modern(auto m, auto n, auto p,
                            const auto* restrict a,
                            const auto* restrict b,
                            auto* restrict result) {
    for (auto i = 0; i < m; i++) {
        for (auto j = 0; j < p; j++) {
            auto sum = 0.0;
            for (auto k = 0; k < n; k++) {
                sum += a[i * n + k] * b[k * p + j];
            }
            result[i * p + j] = sum;
        }
    }
}

#endif
