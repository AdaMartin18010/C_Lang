/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\04_SIMD_Optimization_Case.md
 * Line: 86
 * Language: c
 * Block ID: 912e1129
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// immintrin.h - Intel SIMD头文件
#include <immintrin.h>

// SSE 128-bit寄存器类型
__m128  - 4个float
__m128d - 2个double
__m128i - 16个int8 / 8个int16 / 4个int32 / 2个int64

// AVX 256-bit寄存器类型
__m256  - 8个float
__m256d - 4个double
__m256i - 32个int8 / 16个int16 / 8个int32 / 4个int64

// AVX-512 512-bit寄存器类型
__m512  - 16个float
__m512d - 8个double
__m512i - 64个int8 / 32个int16 / 16个int32 / 8个int64
