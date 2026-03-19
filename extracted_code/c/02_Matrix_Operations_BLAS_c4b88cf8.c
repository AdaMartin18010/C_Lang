/*
 * Auto-generated from: 11_Machine_Learning_C\02_Matrix_Operations_BLAS.md
 * Line: 53
 * Language: c
 * Block ID: c4b88cf8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 矩阵加法: C = A + B
void mat_add(const Matrix* A, const Matrix* B, Matrix* C) {
    int size = A->rows * A->cols;
    for (int i = 0; i < size; i++) {
        C->data[i] = A->data[i] + B->data[i];
    }
}

// 矩阵逐元素乘法(Hadamard积): C = A ⊙ B
void mat_hadamard(const Matrix* A, const Matrix* B, Matrix* C) {
    int size = A->rows * A->cols;
    for (int i = 0; i < size; i++) {
        C->data[i] = A->data[i] * B->data[i];
    }
}

// 标量乘法: B = a * A
void mat_scale(const Matrix* A, double scalar, Matrix* B) {
    int size = A->rows * A->cols;
    for (int i = 0; i < size; i++) {
        B->data[i] = A->data[i] * scalar;
    }
}

// 矩阵转置
void mat_transpose(const Matrix* A, Matrix* T) {
    for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->cols; j++) {
            mat_set(T, j, i, mat_get(A, i, j));
        }
    }
}
