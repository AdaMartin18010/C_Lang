/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\01_SIMD_Vectorization.md
 * Line: 2161
 * Language: c
 * Block ID: e518970e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 未对齐指针使用对齐加载
float data[100];
__m256 vec = _mm256_load_ps(data + 1);  // 可能崩溃或性能下降

// ✅ 正确: 使用非对齐加载指令
__m256 vec = _mm256_loadu_ps(data + 1);  // 'u' = unaligned

// ✅ 更优: 确保对齐
alignas(32) float aligned_data[100];
__m256 vec = _mm256_load_ps(aligned_data);  // 安全且更快
