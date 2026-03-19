/*
 * Auto-generated from: 03_System_Technology_Domains\03_Edge_Detection.md
 * Line: 270
 * Language: c
 * Block ID: 87590951
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Canny边缘检测完整实现
// 步骤: 1. 高斯平滑 2. 梯度计算 3. 非极大值抑制 4. 双阈值 5. 边缘连接

// 高斯平滑核 (5x5, sigma=1.4)
static const float gaussian_5x5[5][5] = {
    {2/159.0f, 4/159.0f, 5/159.0f, 4/159.0f, 2/159.0f},
    {4/159.0f, 9/159.0f, 12/159.0f, 9/159.0f, 4/159.0f},
    {5/159.0f, 12/159.0f, 15/159.0f, 12/159.0f, 5/159.0f},
    {4/159.0f, 9/159.0f, 12/159.0f, 9/159.0f, 4/159.0f},
    {2/159.0f, 4/159.0f, 5/159.0f, 4/159.0f, 2/159.0f}
};

void gaussian_blur(const Image *src, float *dst) {
    int w = src->width;
    int h = src->height;

    for (int y = 2; y < h - 2; y++) {
        for (int x = 2; x < w - 2; x++) {
            float sum = 0;
            for (int ky = -2; ky <= 2; ky++) {
                for (int kx = -2; kx <= 2; kx++) {
                    sum += src->data[(y + ky) * w + (x + kx)] * gaussian_5x5[ky + 2][kx + 2];
                }
            }
            dst[y * w + x] = sum;
        }
    }
}

// 非极大值抑制
void non_max_suppression(const float *magnitude, const float *direction,
                         int w, int h, float *output) {
    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            int idx = y * w + x;
            float mag = magnitude[idx];
            float dir = direction[idx];

            // 将方向归一化到0-180度 (4个方向)
            // 0度: 水平边缘, 45度: 对角线, 90度: 垂直边缘, 135度: 对角线
            float angle = dir * 180.0f / 3.14159f;
            if (angle < 0) angle += 180;

            float neighbor1 = 0, neighbor2 = 0;

            // 确定比较的两个相邻像素
            if ((angle >= 0 && angle < 22.5) || (angle >= 157.5)) {
                // 水平边缘 (0度) - 比较左右
                neighbor1 = magnitude[idx - 1];
                neighbor2 = magnitude[idx + 1];
            } else if (angle >= 22.5 && angle < 67.5) {
                // 对角线 (45度) - 比较右上和左下
                neighbor1 = magnitude[(y - 1) * w + (x + 1)];
                neighbor2 = magnitude[(y + 1) * w + (x - 1)];
            } else if (angle >= 67.5 && angle < 112.5) {
                // 垂直边缘 (90度) - 比较上下
                neighbor1 = magnitude[(y - 1) * w + x];
                neighbor2 = magnitude[(y + 1) * w + x];
            } else {
                // 对角线 (135度) - 比较左上和右下
                neighbor1 = magnitude[(y - 1) * w + (x - 1)];
                neighbor2 = magnitude[(y + 1) * w + (x + 1)];
            }

            // 抑制非极大值
            output[idx] = (mag >= neighbor1 && mag >= neighbor2) ? mag : 0;
        }
    }
}

// 双阈值和边缘连接
#define STRONG_EDGE 255
#define WEAK_EDGE 128
#define NO_EDGE 0

void double_threshold(float *nms, int w, int h,
                      float low_thresh, float high_thresh,
                      uint8_t *edges) {
    for (int i = 0; i < w * h; i++) {
        if (nms[i] >= high_thresh) {
            edges[i] = STRONG_EDGE;
        } else if (nms[i] >= low_thresh) {
            edges[i] = WEAK_EDGE;
        } else {
            edges[i] = NO_EDGE;
        }
    }
}

// 滞后阈值: 连接边缘
void hysteresis(uint8_t *edges, int w, int h) {
    // 8连通域搜索
    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            int idx = y * w + x;

            if (edges[idx] == WEAK_EDGE) {
                // 检查8邻域是否有强边缘
                bool has_strong_neighbor = false;
                for (int dy = -1; dy <= 1 && !has_strong_neighbor; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (edges[(y + dy) * w + (x + dx)] == STRONG_EDGE) {
                            has_strong_neighbor = true;
                            break;
                        }
                    }
                }

                edges[idx] = has_strong_neighbor ? STRONG_EDGE : NO_EDGE;
            }
        }
    }
}

// 完整Canny算法
void canny_edge_detect(const Image *src, Image *dst,
                       float low_thresh, float high_thresh) {
    int w = src->width;
    int h = src->height;
    int size = w * h;

    // 分配临时缓冲区
    float *blurred = malloc(size * sizeof(float));
    float *grad_x = malloc(size * sizeof(float));
    float *grad_y = malloc(size * sizeof(float));
    float *magnitude = malloc(size * sizeof(float));
    float *direction = malloc(size * sizeof(float));
    float *nms = malloc(size * sizeof(float));

    // 1. 高斯平滑
    gaussian_blur(src, blurred);

    // 2. 计算梯度 (使用Sobel)
    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            float gx = 0, gy = 0;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    float pixel = blurred[(y + ky) * w + (x + kx)];
                    gx += pixel * sobel_x[ky + 1][kx + 1];
                    gy += pixel * sobel_y[ky + 1][kx + 1];
                }
            }
            int idx = y * w + x;
            grad_x[idx] = gx;
            grad_y[idx] = gy;
            magnitude[idx] = sqrtf(gx * gx + gy * gy);
            direction[idx] = atan2f(gy, gx);
        }
    }

    // 3. 非极大值抑制
    non_max_suppression(magnitude, direction, w, h, nms);

    // 4. 双阈值
    double_threshold(nms, w, h, low_thresh, high_thresh, dst->data);

    // 5. 边缘连接
    hysteresis(dst->data, w, h);

    // 清理
    free(blurred);
    free(grad_x);
    free(grad_y);
    free(magnitude);
    free(direction);
    free(nms);
}
