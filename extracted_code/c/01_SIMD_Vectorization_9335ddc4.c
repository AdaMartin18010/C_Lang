/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 2279
 * Language: c
 * Block ID: 9335ddc4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 过度展开导致寄存器溢出
float sum[32];  // 太多部分累加器
for (int i = 0; i < n; i += 32) {
    sum[0] += a[i] * b[i];
    sum[1] += a[i+1] * b[i+1];
    // ... 重复32次
    sum[31] += a[i+31] * b[i+31];  // 寄存器溢出到内存
}

// ✅ 正确: 控制在架构寄存器数量内
// x86-64 AVX2: 16个YMM寄存器，建议最多使用12-14个
// ARM NEON: 32个128位寄存器，可用更多
#if defined(__AVX2__)
__m256 sum[4];  // 4个累加器 = 4 YMM寄存器
for (int i = 0; i < n; i += 32) {  // 每次处理32个float
    sum[0] = _mm256_fmadd_ps(_mm256_loadu_ps(&a[i]),
                              _mm256_loadu_ps(&b[i]), sum[0]);
    sum[1] = _mm256_fmadd_ps(_mm256_loadu_ps(&a[i+8]),
                              _mm256_loadu_ps(&b[i+8]), sum[1]);
    sum[2] = _mm256_fmadd_ps(_mm256_loadu_ps(&a[i+16]),
                              _mm256_loadu_ps(&b[i+16]), sum[2]);
    sum[3] = _mm256_fmadd_ps(_mm256_loadu_ps(&a[i+24]),
                              _mm256_loadu_ps(&b[i+24]), sum[3]);
}
#endif
