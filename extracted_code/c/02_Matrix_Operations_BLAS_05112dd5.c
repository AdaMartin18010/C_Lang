/*
 * Auto-generated from: 11_Machine_Learning_C\02_Matrix_Operations_BLAS.md
 * Line: 245
 * Language: c
 * Block ID: 05112dd5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifdef USE_MKL
#include <mkl.h>

// MKL矩阵乘法
void mat_mult_mkl(const Matrix* A, const Matrix* B, Matrix* C) {
    double alpha = 1.0;
    double beta = 0.0;

    cblas_dgemm(CblasRowMajor,
                CblasNoTrans,
                CblasNoTrans,
                A->rows,
                B->cols,
                A->cols,
                alpha,
                A->data,
                A->cols,
                B->data,
                B->cols,
                beta,
                C->data,
                C->cols);
}

// MKL内存对齐分配
Matrix* mat_create_aligned(int rows, int cols) {
    Matrix* m = malloc(sizeof(Matrix));
    m->rows = rows;
    m->cols = cols;
    m->data = mkl_malloc(rows * cols * sizeof(double), 64);
    memset(m->data, 0, rows * cols * sizeof(double));
    return m;
}

void mat_free_aligned(Matrix* m) {
    mkl_free(m->data);
    free(m);
}
#endif
