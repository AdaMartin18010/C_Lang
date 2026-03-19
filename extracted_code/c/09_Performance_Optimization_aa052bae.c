/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\09_Performance_Optimization.md
 * Line: 956
 * Language: c
 * Block ID: aa052bae
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化1: 原始代码 (基线)
void blur_naive(uint8_t *src, uint8_t *dst, int w, int h) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int sum = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    sum += src[(y+dy)*w + (x+dx)];
                }
            }
            dst[y*w + x] = sum / 9;
        }
    }
}

// 优化2: 分离滤波器 (O(n²) -> O(n))
void blur_separable(uint8_t *src, uint8_t *dst, int w, int h) {
    uint8_t *tmp = malloc(w * h);

    // 水平滤波
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            tmp[y*w + x] = (src[y*w + x-1] + src[y*w + x] + src[y*w + x+1]) / 3;
        }
    }

    // 垂直滤波
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            dst[y*w + x] = (tmp[(y-1)*w + x] + tmp[y*w + x] + tmp[(y+1)*w + x]) / 3;
        }
    }

    free(tmp);
}

// 优化3: SIMD
#include <immintrin.h>

void blur_simd(uint8_t *src, uint8_t *dst, int w, int h);

// 优化4: 多线程
void blur_parallel(uint8_t *src, uint8_t *dst, int w, int h) {
    #pragma omp parallel for
    for (int y = 0; y < h; y++) {
        // 处理一行
    }
}
