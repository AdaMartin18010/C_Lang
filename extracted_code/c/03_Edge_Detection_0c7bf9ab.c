/*
 * Auto-generated from: 03_System_Technology_Domains\03_Edge_Detection.md
 * Line: 122
 * Language: c
 * Block ID: 0c7bf9ab
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Sobel算子: 3x3卷积核
// Gx: 水平边缘检测
// [-1  0  1]
// [-2  0  2]
// [-1  0  1]
//
// Gy: 垂直边缘检测
// [-1 -2 -1]
// [ 0  0  0]
// [ 1  2  1]

static const int sobel_x[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
};

static const int sobel_y[3][3] = {
    {-1, -2, -1},
    { 0,  0,  0},
    { 1,  2,  1}
};

// 基础Sobel实现
void sobel_edge_detect(const Image *src, Image *dst) {
    int w = src->width;
    int h = src->height;

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            float gx = 0, gy = 0;

            // 3x3卷积
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    uint8_t pixel = src->data[(y + ky) * w + (x + kx)];
                    gx += pixel * sobel_x[ky + 1][kx + 1];
                    gy += pixel * sobel_y[ky + 1][kx + 1];
                }
            }

            // 计算梯度幅值 (近似: |gx| + |gy| 或精确: sqrt(gx^2 + gy^2))
            float mag = sqrtf(gx * gx + gy * gy);

            // 归一化到0-255
            dst->data[y * w + x] = (uint8_t)(mag > 255 ? 255 : mag);
        }
    }
}

// Scharr算子 (更精确的旋转不变性)
// Gx:
// [-3  0  3]
// [-10 0 10]
// [-3  0  3]

static const int scharr_x[3][3] = {
    {-3, 0,  3},
    {-10, 0, 10},
    {-3, 0,  3}
};

void scharr_edge_detect(const Image *src, Image *dst) {
    // 实现与Sobel类似，使用Scharr核
    // 提供更好的角度分辨率
}
