/*
 * Auto-generated from: 12_Practice_Exercises\06_Performance_Optimization.md
 * Line: 52
 * Language: c
 * Block ID: 0003f8ad
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void matmul_blocked_avx2(float *a, float *b, float *c, int n) {
    const int BLOCK = 32;  // 缓存块大小

    for (int ii = 0; ii < n; ii += BLOCK) {
        for (int jj = 0; jj < n; jj += BLOCK) {
            for (int kk = 0; kk < n; kk += BLOCK) {
                // 处理BLOCK x BLOCK子矩阵
                for (int i = ii; i < ii + BLOCK && i < n; i++) {
                    for (int j = jj; j < jj + BLOCK && j < n; j += 8) {
                        __m256 c_vec = _mm256_load_ps(&c[i*n + j]);

                        for (int k = kk; k < kk + BLOCK && k < n; k++) {
                            __m256 a_val = _mm256_set1_ps(a[i*n + k]);
                            __m256 b_vec = _mm256_load_ps(&b[k*n + j]);
                            c_vec = _mm256_fmadd_ps(a_val, b_vec, c_vec);
                        }

                        _mm256_store_ps(&c[i*n + j], c_vec);
                    }
                }
            }
        }
    }
}
