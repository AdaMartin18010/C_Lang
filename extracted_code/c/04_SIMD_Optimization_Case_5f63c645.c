/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\04_SIMD_Optimization_Case.md
 * Line: 152
 * Language: c
 * Block ID: 5f63c645
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 算术运算
__m256 _mm256_add_ps(__m256 a, __m256 b);   // 加法
__m256 _mm256_sub_ps(__m256 a, __m256 b);   // 减法
__m256 _mm256_mul_ps(__m256 a, __m256 b);   // 乘法
__m256 _mm256_div_ps(__m256 a, __m256 b);   // 除法
__m256 _mm256_sqrt_ps(__m256 a);            // 平方根
__m256 _mm256_fma_ps(__m256 a, __m256 b, __m256 c);  // FMA: a*b + c

// 比较运算
__m256 _mm256_cmp_ps(__m256 a, __m256 b, int pred);
// 谓词: _CMP_EQ_OQ, _CMP_LT_OS, _CMP_LE_OS, _CMP_GT_OS, etc.

// 逻辑运算
__m256 _mm256_and_ps(__m256 a, __m256 b);
__m256 _mm256_or_ps(__m256 a, __m256 b);
__m256 _mm256_xor_ps(__m256 a, __m256 b);

// 整数运算 (AVX2)
__m256i _mm256_add_epi32(__m256i a, __m256i b);  // 32位整数加法
__m256i _mm256_mullo_epi32(__m256i a, __m256i b); // 32位整数乘法

// 移位运算
__m256i _mm256_slli_epi32(__m256i a, int imm8);  // 逻辑左移
__m256i _mm256_srli_epi32(__m256i a, int imm8);  // 逻辑右移
