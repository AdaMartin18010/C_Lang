/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 2368
 * Language: c
 * Block ID: 8550457a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: gather用于连续内存
for (int i = 0; i < n; i += 8) {
    int idx[8] = {i, i+1, i+2, i+3, i+4, i+5, i+6, i+7};
    __m256i vidx = _mm256_loadu_si256((__m256i*)idx);
    __m256 v = _mm256_i32gather_ps(a, vidx, 4);  // 慢！
}

// ✅ 正确: 使用连续加载
for (int i = 0; i < n; i += 8) {
    __m256 v = _mm256_loadu_ps(&a[i]);  // 快！
}

// ✅ 正确: gather用于真正的随机访问
// 场景: 稀疏矩阵向量乘法
for (int i = 0; i < nnz; i += 8) {
    __m256i col_idx = _mm256_loadu_si256((__m256i*)&col_indices[i]);
    __m256 values = _mm256_loadu_ps(&matrix_values[i]);
    __m256 x_gathered = _mm256_i32gather_ps(x, col_idx, 4);  // 必要
    sum_vec = _mm256_fmadd_ps(values, x_gathered, sum_vec);
}
