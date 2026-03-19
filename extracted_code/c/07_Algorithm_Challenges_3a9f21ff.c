/*
 * Auto-generated from: 12_Practice_Exercises\07_Algorithm_Challenges.md
 * Line: 300
 * Language: c
 * Block ID: 3a9f21ff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    size_t rows, cols;
    double *data;
} Matrix;

Matrix* matrix_create(size_t rows, size_t cols);
void matrix_free(Matrix *m);

Matrix* matrix_add(const Matrix *a, const Matrix *b);
Matrix* matrix_mul(const Matrix *a, const Matrix *b);
Matrix* matrix_transpose(const Matrix *m);

// 高斯消元
bool matrix_solve(const Matrix *a, const double *b, double *x);
double matrix_determinant(const Matrix *m);
Matrix* matrix_inverse(const Matrix *m);

// Strassen快速矩阵乘法
Matrix* matrix_mul_strassen(const Matrix *a, const Matrix *b);
