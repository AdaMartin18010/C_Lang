/*
 * Auto-generated from: 11_Machine_Learning_C\02_Matrix_Operations_BLAS.md
 * Line: 231
 * Language: c
 * Block ID: d911e2e6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 批量矩阵乘法(用于神经网络批量处理)
void batch_mat_mult_blas(const Matrix** A, const Matrix** B,
                         Matrix** C, int batch_size) {
    for (int i = 0; i < batch_size; i++) {
        mat_mult_blas(A[i], B[i], C[i]);
    }
}
