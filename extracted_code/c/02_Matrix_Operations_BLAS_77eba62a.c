/*
 * Auto-generated from: 11_Machine_Learning_C\02_Matrix_Operations_BLAS.md
 * Line: 116
 * Language: c
 * Block ID: 77eba62a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define BLOCK_SIZE 64

void mat_mult_blocked(const Matrix* A, const Matrix* B, Matrix* C) {
    int M = A->rows;
    int N = B->cols;
    int K = A->cols;

    memset(C->data, 0, M * N * sizeof(double));

    for (int i0 = 0; i0 < M; i0 += BLOCK_SIZE) {
        for (int j0 = 0; j0 < N; j0 += BLOCK_SIZE) {
            for (int k0 = 0; k0 < K; k0 += BLOCK_SIZE) {
                // 处理块
                int imax = (i0 + BLOCK_SIZE < M) ? i0 + BLOCK_SIZE : M;
                int jmax = (j0 + BLOCK_SIZE < N) ? j0 + BLOCK_SIZE : N;
                int kmax = (k0 + BLOCK_SIZE < K) ? k0 + BLOCK_SIZE : K;

                for (int i = i0; i < imax; i++) {
                    for (int k = k0; k < kmax; k++) {
                        double a = mat_get(A, i, k);
                        for (int j = j0; j < jmax; j++) {
                            C->data[i * N + j] += a * mat_get(B, k, j);
                        }
                    }
                }
            }
        }
    }
}
