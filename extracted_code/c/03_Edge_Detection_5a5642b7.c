/*
 * Auto-generated from: 03_System_Technology_Domains\03_Computer_Vision\03_Edge_Detection.md
 * Line: 329
 * Language: c
 * Block ID: 5a5642b7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* canny_core.c - Canny算法核心实现 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "canny_edge_detector.h"

/* 高斯滤波 */
static void gaussian_smooth(const uint8_t *src, double *dst,
                            int width, int height, double sigma)
{
    int kernel_size = (int)(6 * sigma) | 1;  /* 奇数大小 */
    if (kernel_size < 3) kernel_size = 3;
    if (kernel_size > 15) kernel_size = 15;

    int half = kernel_size / 2;
    double *kernel = malloc(kernel_size * kernel_size * sizeof(double));

    /* 生成高斯核 */
    double sum = 0.0;
    for (int i = -half; i <= half; i++) {
        for (int j = -half; j <= half; j++) {
            double g = exp(-(i*i + j*j) / (2.0 * sigma * sigma));
            kernel[(i + half) * kernel_size + (j + half)] = g;
            sum += g;
        }
    }

    /* 归一化 */
    for (int i = 0; i < kernel_size * kernel_size; i++) {
        kernel[i] /= sum;
    }

    /* 卷积操作 */
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double val = 0.0;
            for (int ky = -half; ky <= half; ky++) {
                for (int kx = -half; kx <= half; kx++) {
                    int px = x + kx;
                    int py = y + ky;
                    /* 边界处理：镜像 */
                    if (px < 0) px = -px;
                    if (px >= width) px = 2 * width - px - 2;
                    if (py < 0) py = -py;
                    if (py >= height) py = 2 * height - py - 2;

                    val += src[py * width + px] *
                           kernel[(ky + half) * kernel_size + (kx + half)];
                }
            }
            dst[y * width + x] = val;
        }
    }

    free(kernel);
}

/* 计算梯度 */
static void compute_gradient(const double *src, double *magnitude,
                              double *direction, int width, int height)
{
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            /* Sobel算子 */
            double gx = -src[(y-1)*width + (x-1)] + src[(y-1)*width + (x+1)]
                      - 2*src[y*width + (x-1)]     + 2*src[y*width + (x+1)]
                      - src[(y+1)*width + (x-1)] + src[(y+1)*width + (x+1)];

            double gy =  src[(y-1)*width + (x-1)] + 2*src[(y-1)*width + x] +
                       src[(y-1)*width + (x+1)]
                      - src[(y+1)*width + (x-1)] - 2*src[(y+1)*width + x] -
                       src[(y+1)*width + (x+1)];

            int idx = y * width + x;
            magnitude[idx] = sqrt(gx * gx + gy * gy);
            direction[idx] = atan2(gy, gx);
        }
    }
}

/* 非极大值抑制 */
static void non_maximum_suppression(const double *magnitude,
                                     const double *direction,
                                     double *output, int width, int height)
{
    /* 方向量化：0°、45°、90°、135° */
    const int dx[] = {1, 1, 0, -1};
    const int dy[] = {0, 1, 1, 1};

    memcpy(output, magnitude, (size_t)width * height * sizeof(double));

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int idx = y * width + x;
            double angle = direction[idx];

            /* 归一化到0-π范围 */
            if (angle < 0) angle += M_PI;

            /* 确定方向扇区 */
            int sector = (int)((angle + M_PI/8) / (M_PI/4)) % 4;

            double mag1 = magnitude[(y + dy[sector]) * width + (x + dx[sector])];
            double mag2 = magnitude[(y - dy[sector]) * width + (x - dx[sector])];

            if (magnitude[idx] < mag1 || magnitude[idx] < mag2) {
                output[idx] = 0;
            }
        }
    }
}

/* 双阈值检测和边缘连接 */
static void hysteresis_thresholding(double *gradient, uint8_t *edges,
                                     int width, int height,
                                     double low, double high)
{
    /* 查找最大梯度值用于阈值归一化 */
    double max_grad = 0.0;
    for (int i = 0; i < width * height; i++) {
        if (gradient[i] > max_grad) max_grad = gradient[i];
    }

    double high_thresh = high * max_grad;
    double low_thresh = low * max_grad;

    /* 初始化边缘图 */
    memset(edges, 0, (size_t)width * height);

    /* 标记强边缘 */
    for (int i = 0; i < width * height; i++) {
        if (gradient[i] >= high_thresh) {
            edges[i] = 255;
        }
    }

    /* 边缘连接：使用8连通 */
    bool changed;
    do {
        changed = false;
        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                int idx = y * width + x;
                if (gradient[idx] >= low_thresh && gradient[idx] < high_thresh && edges[idx] == 0) {
                    /* 检查8邻域是否有强边缘 */
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dx = -1; dx <= 1; dx++) {
                            if (dx == 0 && dy == 0) continue;
                            if (edges[(y + dy) * width + (x + dx)] == 255) {
                                edges[idx] = 255;
                                changed = true;
                                break;
                            }
                        }
                        if (edges[idx] == 255) break;
                    }
                }
            }
        }
    } while (changed);
}

/* 主Canny检测函数 */
Image* canny_edge_detect(const Image *src, const CannyParams *params)
{
    if (!src || !params) return NULL;

    int width = src->width;
    int height = src->height;
    int size = width * height;

    /* 分配中间缓冲区 */
    double *smoothed = malloc(size * sizeof(double));
    double *magnitude = malloc(size * sizeof(double));
    double *direction = malloc(size * sizeof(double));
    double *suppressed = malloc(size * sizeof(double));

    if (!smoothed || !magnitude || !direction || !suppressed) {
        free(smoothed); free(magnitude);
        free(direction); free(suppressed);
        return NULL;
    }

    /* 步骤1: 高斯滤波 */
    gaussian_smooth(src->data, smoothed, width, height, params->sigma);

    /* 步骤2: 计算梯度 */
    compute_gradient(smoothed, magnitude, direction, width, height);

    /* 步骤3: 非极大值抑制 */
    non_maximum_suppression(magnitude, direction, suppressed, width, height);

    /* 步骤4: 双阈值检测和边缘连接 */
    Image *edges = image_create(width, height);
    if (edges) {
        hysteresis_thresholding(suppressed, edges->data, width, height,
                               params->low_thresh, params->high_thresh);
    }

    free(smoothed);
    free(magnitude);
    free(direction);
    free(suppressed);

    return edges;
}
