/*
 * Auto-generated from: 11_Machine_Learning_C\04_CNN_Fundamentals.md
 * Line: 462
 * Language: c
 * Block ID: cce91513
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// NCHW vs NHWC
// NCHW (Batch, Channel, Height, Width) - cuDNN默认
// NHWC - TensorFlow默认，CPU缓存友好

// 使用SIMD向量化
#include <immintrin.h>

void relu_avx(float* data, int n) {
    __m256 zero = _mm256_setzero_ps();
    int i = 0;
    for (; i <= n - 8; i += 8) {
        __m256 x = _mm256_loadu_ps(&data[i]);
        x = _mm256_max_ps(x, zero);
        _mm256_storeu_ps(&data[i], x);
    }
    // 处理剩余元素
    for (; i < n; i++) {
        if (data[i] < 0) data[i] = 0;
    }
}
