/*
 * Auto-generated from: 03_System_Technology_Domains\03_Edge_Detection.md
 * Line: 193
 * Language: c
 * Block ID: e7b217db
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Laplacian: 二阶微分算子
// 对噪声敏感，通常先进行高斯平滑

// 基础Laplacian核
// [ 0  1  0]
// [ 1 -4  1]
// [ 0  1  0]

// 扩展Laplacian核 (考虑对角线)
// [ 1  1  1]
// [ 1 -8  1]
// [ 1  1  1]

static const int laplacian_kernel[3][3] = {
    {0,  1, 0},
    {1, -4, 1},
    {0,  1, 0}
};

void laplacian_edge_detect(const Image *src, Image *dst) {
    int w = src->width;
    int h = src->height;

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            int sum = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    uint8_t pixel = src->data[(y + ky) * w + (x + kx)];
                    sum += pixel * laplacian_kernel[ky + 1][kx + 1];
                }
            }

            // Laplacian结果可能有正负，取绝对值
            int edge = abs(sum);
            dst->data[y * w + x] = edge > 255 ? 255 : (uint8_t)edge;
        }
    }
}

// LoG (Laplacian of Gaussian): 先平滑再检测
// 5x5 LoG核近似
static const float log_kernel[5][5] = {
    {0, 0, 1, 0, 0},
    {0, 1, 2, 1, 0},
    {1, 2, -16, 2, 1},
    {0, 1, 2, 1, 0},
    {0, 0, 1, 0, 0}
};

void log_edge_detect(const Image *src, Image *dst) {
    // 5x5卷积实现
    int w = src->width;
    int h = src->height;

    for (int y = 2; y < h - 2; y++) {
        for (int x = 2; x < w - 2; x++) {
            float sum = 0;

            for (int ky = -2; ky <= 2; ky++) {
                for (int kx = -2; kx <= 2; kx++) {
                    uint8_t pixel = src->data[(y + ky) * w + (x + kx)];
                    sum += pixel * log_kernel[ky + 2][kx + 2];
                }
            }

            int edge = (int)fabsf(sum);
            dst->data[y * w + x] = edge > 255 ? 255 : (uint8_t)edge;
        }
    }
}
