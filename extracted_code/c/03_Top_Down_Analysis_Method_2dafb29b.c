/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\03_Top_Down_Analysis_Method.md
 * Line: 1410
 * Language: c
 * Block ID: 2dafb29b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* image_filter_optimized.c - 优化版本 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 4096
#define HEIGHT 4096
#define BLOCK_SIZE 64

/* 分块处理 + SIMD友好的内存布局 */
void blur_filter_optimized(unsigned char *src, unsigned char *dst,
                           int width, int height) {
    /* 使用行缓冲区减少缓存未命中 */
    unsigned char row_buf[3][WIDTH];

    for (int by = 1; by < height - 1; by += BLOCK_SIZE) {
        int by_end = (by + BLOCK_SIZE < height - 1) ? by + BLOCK_SIZE : height - 1;

        for (int bx = 1; bx < width - 1; bx += BLOCK_SIZE) {
            int bx_end = (bx + BLOCK_SIZE < width - 1) ? bx + BLOCK_SIZE : width - 1;

            /* 预加载行到缓冲区 */
            memcpy(row_buf[0], &src[(by - 1) * width], width);
            memcpy(row_buf[1], &src[by * width], width);
            memcpy(row_buf[2], &src[(by + 1) * width], width);

            /* 处理块 */
            for (int y = by; y < by_end; y++) {
                for (int x = bx; x < bx_end; x++) {
                    /* 从缓冲区读取，提高缓存局部性 */
                    int sum = row_buf[0][x-1] + row_buf[0][x] + row_buf[0][x+1] +
                              row_buf[1][x-1] + row_buf[1][x] + row_buf[1][x+1] +
                              row_buf[2][x-1] + row_buf[2][x] + row_buf[2][x+1];
                    dst[y * width + x] = sum / 9;
                }

                /* 滚动缓冲区 */
                if (y + 1 < by_end) {
                    memcpy(row_buf[0], row_buf[1], width);
                    memcpy(row_buf[1], row_buf[2], width);
                    memcpy(row_buf[2], &src[(y + 2) * width], width);
                }
            }
        }
    }
}
