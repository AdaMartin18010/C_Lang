/*
 * Auto-generated from: 03_System_Technology_Domains\03_Edge_Detection.md
 * Line: 443
 * Language: c
 * Block ID: 8d3309de
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// SSE/AVX优化的Sobel实现
#include <immintrin.h>

// SSE4.1优化: 一次处理16字节
void sobel_sse(const uint8_t *src, uint8_t *dst, int w, int h) {
    // 加载Sobel核到寄存器
    __m128i kx_pos = _mm_set1_epi16(1);
    __m128i kx_neg = _mm_set1_epi16(-1);
    __m128i kx_center = _mm_set1_epi16(2);

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x += 16) {
            // 加载3行数据
            __m128i row0 = _mm_loadu_si128((__m128i*)(src + (y-1)*w + x - 1));
            __m128i row1 = _mm_loadu_si128((__m128i*)(src + y*w + x - 1));
            __m128i row2 = _mm_loadu_si128((__m128i*)(src + (y+1)*w + x - 1));

            // 解交错为16位以支持负数运算
            __m128i r0_lo = _mm_unpacklo_epi8(row0, _mm_setzero_si128());
            __m128i r0_hi = _mm_unpackhi_epi8(row0, _mm_setzero_si128());
            __m128i r2_lo = _mm_unpacklo_epi8(row2, _mm_setzero_si128());
            __m128i r2_hi = _mm_unpackhi_epi8(row2, _mm_setzero_si128());

            // Gx计算: (r2 - r0) 的水平部分
            __m128i gx_lo = _mm_sub_epi16(r2_lo, r0_lo);
            __m128i gx_hi = _mm_sub_epi16(r2_hi, r0_hi);

            // 计算绝对值并饱和到8位
            __m128i abs_lo = _mm_abs_epi16(gx_lo);
            __m128i abs_hi = _mm_abs_epi16(gx_hi);
            __m128i result = _mm_packus_epi16(abs_lo, abs_hi);

            // 存储结果
            _mm_storeu_si128((__m128i*)(dst + y*w + x), result);
        }
    }
}

// AVX2优化: 一次处理32字节
void sobel_avx2(const uint8_t *src, uint8_t *dst, int w, int h) {
    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x += 32) {
            // 加载3行，每行32字节 (使用AVX2 256位寄存器)
            __m256i row0 = _mm256_loadu_si256((__m256i*)(src + (y-1)*w + x - 1));
            __m256i row1 = _mm256_loadu_si256((__m256i*)(src + y*w + x - 1));
            __m256i row2 = _mm256_loadu_si256((__m256i*)(src + (y+1)*w + x - 1));

            // 类似SSE处理，但256位宽度
            __m256i r0_lo = _mm256_unpacklo_epi8(row0, _mm256_setzero_si256());
            __m256i r0_hi = _mm256_unpackhi_epi8(row0, _mm256_setzero_si256());
            __m256i r2_lo = _mm256_unpacklo_epi8(row2, _mm256_setzero_si256());
            __m256i r2_hi = _mm256_unpackhi_epi8(row2, _mm256_setzero_si256());

            __m256i gx_lo = _mm256_sub_epi16(r2_lo, r0_lo);
            __m256i gx_hi = _mm256_sub_epi16(r2_hi, r0_hi);

            __m256i abs_lo = _mm256_abs_epi16(gx_lo);
            __m256i abs_hi = _mm256_abs_epi16(gx_hi);
            __m256i result = _mm256_packus_epi16(abs_lo, abs_hi);

            _mm256_storeu_si256((__m256i*)(dst + y*w + x), result);
        }
    }
}
