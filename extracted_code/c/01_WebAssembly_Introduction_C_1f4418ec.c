/*
 * Auto-generated from: 10_WebAssembly_C\01_WebAssembly_Introduction_C.md
 * Line: 2364
 * Language: c
 * Block ID: 1f4418ec
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// matrix.c
#include <emscripten/emscripten.h>
#include <stdlib.h>
#include <math.h>

// 矩阵结构
typedef struct {
    float* data;
    int rows;
    int cols;
} Matrix;

// 创建矩阵
EMSCRIPTEN_KEEPALIVE
Matrix* matrix_create(int rows, int cols) {
    Matrix* mat = (Matrix*)malloc(sizeof(Matrix));
    if (!mat) return NULL;

    mat->rows = rows;
    mat->cols = cols;
    mat->data = (float*)calloc(rows * cols, sizeof(float));

    if (!mat->data) {
        free(mat);
        return NULL;
    }

    return mat;
}

// 销毁矩阵
EMSCRIPTEN_KEEPALIVE
void matrix_destroy(Matrix* mat) {
    if (mat) {
        if (mat->data) free(mat->data);
        free(mat);
    }
}

// 获取/设置元素
EMSCRIPTEN_KEEPALIVE
float matrix_get(Matrix* mat, int row, int col) {
    if (!mat || row < 0 || row >= mat->rows || col < 0 || col >= mat->cols) return 0;
    return mat->data[row * mat->cols + col];
}

EMSCRIPTEN_KEEPALIVE
void matrix_set(Matrix* mat, int row, int col, float value) {
    if (!mat || row < 0 || row >= mat->rows || col < 0 || col >= mat->cols) return;
    mat->data[row * mat->cols + col] = value;
}

// 获取数据指针
EMSCRIPTEN_KEEPALIVE
float* matrix_get_data(Matrix* mat) {
    return mat ? mat->data : NULL;
}

// 矩阵加法: C = A + B
EMSCRIPTEN_KEEPALIVE
void matrix_add(const Matrix* A, const Matrix* B, Matrix* C) {
    if (!A || !B || !C) return;
    if (A->rows != B->rows || A->cols != B->cols) return;
    if (A->rows != C->rows || A->cols != C->cols) return;

    int size = A->rows * A->cols;
    for (int i = 0; i < size; i++) {
        C->data[i] = A->data[i] + B->data[i];
    }
}

// 矩阵减法: C = A - B
EMSCRIPTEN_KEEPALIVE
void matrix_subtract(const Matrix* A, const Matrix* B, Matrix* C) {
    if (!A || !B || !C) return;
    if (A->rows != B->rows || A->cols != B->cols) return;
    if (A->rows != C->rows || A->cols != C->cols) return;

    int size = A->rows * A->cols;
    for (int i = 0; i < size; i++) {
        C->data[i] = A->data[i] - B->data[i];
    }
}

// 矩阵乘法: C = A × B
EMSCRIPTEN_KEEPALIVE
void matrix_multiply(const Matrix* A, const Matrix* B, Matrix* C) {
    if (!A || !B || !C) return;
    if (A->cols != B->rows) return;
    if (C->rows != A->rows || C->cols != B->cols) return;

    int m = A->rows;
    int n = A->cols;
    int p = B->cols;

    // 初始化 C 为零
    for (int i = 0; i < m * p; i++) {
        C->data[i] = 0;
    }

    // 矩阵乘法
    for (int i = 0; i < m; i++) {
        for (int k = 0; k < n; k++) {
            float a_ik = A->data[i * n + k];
            for (int j = 0; j < p; j++) {
                C->data[i * p + j] += a_ik * B->data[k * p + j];
            }
        }
    }
}

// 矩阵转置
EMSCRIPTEN_KEEPALIVE
void matrix_transpose(const Matrix* A, Matrix* B) {
    if (!A || !B) return;
    if (B->rows != A->cols || B->cols != A->rows) return;

    for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->cols; j++) {
            B->data[j * A->rows + i] = A->data[i * A->cols + j];
        }
    }
}

// 标量乘法
EMSCRIPTEN_KEEPALIVE
void matrix_scale(Matrix* A, float scalar) {
    if (!A) return;

    int size = A->rows * A->cols;
    for (int i = 0; i < size; i++) {
        A->data[i] *= scalar;
    }
}

// 矩阵填充随机值
EMSCRIPTEN_KEEPALIVE
void matrix_randomize(Matrix* A) {
    if (!A) return;

    int size = A->rows * A->cols;
    for (int i = 0; i < size; i++) {
        A->data[i] = (float)rand() / RAND_MAX * 2.0f - 1.0f;
    }
}

// 单位矩阵
EMSCRIPTEN_KEEPALIVE
void matrix_identity(Matrix* A) {
    if (!A || A->rows != A->cols) return;

    int n = A->rows;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A->data[i * n + j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

// 高斯消元法求逆矩阵
EMSCRIPTEN_KEEPALIVE
int matrix_inverse(const Matrix* A, Matrix* B) {
    if (!A || !B || A->rows != A->cols || B->rows != B->cols) return 0;
    if (A->rows != B->rows) return 0;

    int n = A->rows;

    // 创建增广矩阵 [A | I]
    float* aug = (float*)malloc(n * 2 * n * sizeof(float));
    if (!aug) return 0;

    // 初始化增广矩阵
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            aug[i * 2 * n + j] = A->data[i * n + j];
            aug[i * 2 * n + j + n] = (i == j) ? 1.0f : 0.0f;
        }
    }

    // 高斯消元
    for (int i = 0; i < n; i++) {
        // 找主元
        float maxVal = fabs(aug[i * 2 * n + i]);
        int maxRow = i;
        for (int k = i + 1; k < n; k++) {
            float val = fabs(aug[k * 2 * n + i]);
            if (val > maxVal) {
                maxVal = val;
                maxRow = k;
            }
        }

        if (maxVal < 1e-10) {
            free(aug);
            return 0;  // 奇异矩阵
        }

        // 交换行
        if (maxRow != i) {
            for (int j = 0; j < 2 * n; j++) {
                float temp = aug[i * 2 * n + j];
                aug[i * 2 * n + j] = aug[maxRow * 2 * n + j];
                aug[maxRow * 2 * n + j] = temp;
            }
        }

        // 归一化主元行
        float pivot = aug[i * 2 * n + i];
        for (int j = 0; j < 2 * n; j++) {
            aug[i * 2 * n + j] /= pivot;
        }

        // 消去其他行
        for (int k = 0; k < n; k++) {
            if (k != i) {
                float factor = aug[k * 2 * n + i];
                for (int j = 0; j < 2 * n; j++) {
                    aug[k * 2 * n + j] -= factor * aug[i * 2 * n + j];
                }
            }
        }
    }

    // 提取逆矩阵
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            B->data[i * n + j] = aug[i * 2 * n + j + n];
        }
    }

    free(aug);
    return 1;
}

// 矩阵行列式
EMSCRIPTEN_KEEPALIVE
float matrix_determinant(const Matrix* A) {
    if (!A || A->rows != A->cols) return 0;

    int n = A->rows;
    if (n == 1) return A->data[0];
    if (n == 2) {
        return A->data[0] * A->data[3] - A->data[1] * A->data[2];
    }

    // 复制矩阵用于LU分解
    float* temp = (float*)malloc(n * n * sizeof(float));
    if (!temp) return 0;
    memcpy(temp, A->data, n * n * sizeof(float));

    float det = 1.0f;
    int swaps = 0;

    for (int i = 0; i < n; i++) {
        // 找主元
        float maxVal = fabs(temp[i * n + i]);
        int maxRow = i;
        for (int k = i + 1; k < n; k++) {
            float val = fabs(temp[k * n + i]);
            if (val > maxVal) {
                maxVal = val;
                maxRow = k;
            }
        }

        if (maxVal < 1e-10) {
            free(temp);
            return 0;
        }

        // 交换行
        if (maxRow != i) {
            for (int j = 0; j < n; j++) {
                float t = temp[i * n + j];
                temp[i * n + j] = temp[maxRow * n + j];
                temp[maxRow * n + j] = t;
            }
            swaps++;
        }

        // 消元
        for (int k = i + 1; k < n; k++) {
            float factor = temp[k * n + i] / temp[i * n + i];
            for (int j = i; j < n; j++) {
                temp[k * n + j] -= factor * temp[i * n + j];
            }
        }

        det *= temp[i * n + i];
    }

    free(temp);
    return (swaps % 2 == 0) ? det : -det;
}

// 求特征值（简化版 - 幂迭代法求最大特征值）
EMSCRIPTEN_KEEPALIVE
float matrix_max_eigenvalue(const Matrix* A, int maxIterations) {
    if (!A || A->rows != A->cols) return 0;

    int n = A->rows;
    float* b = (float*)malloc(n * sizeof(float));
    float* Ab = (float*)malloc(n * sizeof(float));
    if (!b || !Ab) return 0;

    // 初始向量
    for (int i = 0; i < n; i++) b[i] = 1.0f;

    float eigenvalue = 0;

    for (int iter = 0; iter < maxIterations; iter++) {
        // 计算 Ab = A * b
        for (int i = 0; i < n; i++) {
            Ab[i] = 0;
            for (int j = 0; j < n; j++) {
                Ab[i] += A->data[i * n + j] * b[j];
            }
        }

        // 归一化
        float norm = 0;
        for (int i = 0; i < n; i++) norm += Ab[i] * Ab[i];
        norm = sqrt(norm);

        eigenvalue = norm;

        for (int i = 0; i < n; i++) {
            b[i] = Ab[i] / norm;
        }
    }

    free(b);
    free(Ab);
    return eigenvalue;
}

// 获取矩阵维度
EMSCRIPTEN_KEEPALIVE
int matrix_get_rows(const Matrix* mat) { return mat ? mat->rows : 0; }
EMSCRIPTEN_KEEPALIVE
int matrix_get_cols(const Matrix* mat) { return mat ? mat->cols : 0; }
