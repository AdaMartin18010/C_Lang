/*
 * Auto-generated from: 11_Machine_Learning_C\02_Matrix_Operations_BLAS.md
 * Line: 94
 * Language: c
 * Block ID: 757ca6ff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 基础矩阵乘法: C = A × B
void mat_mult_basic(const Matrix* A, const Matrix* B, Matrix* C) {
    int M = A->rows;
    int N = B->cols;
    int K = A->cols;

    memset(C->data, 0, M * N * sizeof(double));

    for (int i = 0; i < M; i++) {
        for (int k = 0; k < K; k++) {
            double a = mat_get(A, i, k);
            for (int j = 0; j < N; j++) {
                C->data[i * N + j] += a * mat_get(B, k, j);
            }
        }
    }
}
