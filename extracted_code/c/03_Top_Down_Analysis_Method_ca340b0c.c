/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\03_Top_Down_Analysis_Method.md
 * Line: 1365
 * Language: c
 * Block ID: ca340b0c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* image_filter_original.c - 原始版本 */
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 4096
#define HEIGHT 4096

void blur_filter_original(unsigned char *src, unsigned char *dst,
                          int width, int height) {
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            /* 3x3模糊核 */
            int sum = 0;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    sum += src[(y + ky) * width + (x + kx)];
                }
            }
            dst[y * width + x] = sum / 9;
        }
    }
}
