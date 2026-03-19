/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 2309
 * Language: c
 * Block ID: 587b610a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 小规模数据使用SIMD (设置开销 > 收益)
float a[4], b[4], c[4];
// SIMD设置开销 ~20-30 cycles
// 标量4次操作 ~12-16 cycles

// ✅ 正确: 小规模使用标量或编译器自动向量化
for (int i = 0; i < 4; i++) {
    c[i] = a[i] + b[i];  // 编译器会内联优化
}

// ✅ 经验法则: SIMD门槛
// - NEON/AVX2: n >= 32-64
// - AVX-512: n >= 64-128
