/*
 * Auto-generated from: 03_System_Technology_Domains\03_Edge_Detection.md
 * Line: 73
 * Language: c
 * Block ID: f24b6712
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 图像梯度计算基础
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

// 图像结构
typedef struct {
    uint8_t *data;
    int width;
    int height;
    int channels;
} Image;

// 梯度结构
typedef struct {
    float *magnitude;   // 梯度幅值
    float *direction;   // 梯度方向 (弧度)
} Gradient;

// 基础梯度计算: 中心差分
void compute_gradient_basic(const Image *src, Gradient *grad) {
    int w = src->width;
    int h = src->height;

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            // 灰度值
            uint8_t *p = src->data + y * w + x;

            // x方向梯度: 右 - 左
            float gx = *(p + 1) - *(p - 1);

            // y方向梯度: 下 - 上
            float gy = *(p + w) - *(p - w);

            // 梯度幅值
            int idx = y * w + x;
            grad->magnitude[idx] = sqrtf(gx * gx + gy * gy);

            // 梯度方向
            grad->direction[idx] = atan2f(gy, gx);
        }
    }
}
