/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\04_SIMD_Optimization_Case.md
 * Line: 181
 * Language: c
 * Block ID: 5fa61f3f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 8x8分块矩阵乘法 - AVX优化版本
void matmul_avx_8x8(const float* A, const float* B, float* C) {
    // 加载C的8行，每行8个元素
    __m256 c0 = _mm256_load_ps(&C[0]);
    __m256 c1 = _mm256_load_ps(&C[8]);
    __m256 c2 = _mm256_load_ps(&C[16]);
    __m256 c3 = _mm256_load_ps(&C[24]);
    __m256 c4 = _mm256_load_ps(&C[32]);
    __m256 c5 = _mm256_load_ps(&C[40]);
    __m256 c6 = _mm256_load_ps(&C[48]);
    __m256 c7 = _mm256_load_ps(&C[56]);

    for (int k = 0; k < 8; k++) {
        // 广播A的第k列元素
        __m256 a0 = _mm256_set1_ps(A[0*8 + k]);
        __m256 a1 = _mm256_set1_ps(A[1*8 + k]);
        __m256 a2 = _mm256_set1_ps(A[2*8 + k]);
        __m256 a3 = _mm256_set1_ps(A[3*8 + k]);
        __m256 a4 = _mm256_set1_ps(A[4*8 + k]);
        __m256 a5 = _mm256_set1_ps(A[5*8 + k]);
        __m256 a6 = _mm256_set1_ps(A[6*8 + k]);
        __m256 a7 = _mm256_set1_ps(A[7*8 + k]);

        // 加载B的第k行
        __m256 b = _mm256_load_ps(&B[k*8]);

        // FMA累加
        c0 = _mm256_fmadd_ps(a0, b, c0);
        c1 = _mm256_fmadd_ps(a1, b, c1);
        c2 = _mm256_fmadd_ps(a2, b, c2);
        c3 = _mm256_fmadd_ps(a3, b, c3);
        c4 = _mm256_fmadd_ps(a4, b, c4);
        c5 = _mm256_fmadd_ps(a5, b, c5);
        c6 = _mm256_fmadd_ps(a6, b, c6);
        c7 = _mm256_fmadd_ps(a7, b, c7);
    }

    // 存储结果
    _mm256_store_ps(&C[0], c0);
    _mm256_store_ps(&C[8], c1);
    _mm256_store_ps(&C[16], c2);
    _mm256_store_ps(&C[24], c3);
    _mm256_store_ps(&C[32], c4);
    _mm256_store_ps(&C[40], c5);
    _mm256_store_ps(&C[48], c6);
    _mm256_store_ps(&C[56], c7);
}
