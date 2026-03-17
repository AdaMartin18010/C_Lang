# Canny边缘检测算法C语言实现


---

## 📑 目录

- [Canny边缘检测算法C语言实现](#canny边缘检测算法c语言实现)
  - [📑 目录](#-目录)
  - [1. 边缘检测理论基础](#1-边缘检测理论基础)
    - [1.1 边缘检测概述](#11-边缘检测概述)
    - [1.2 边缘检测算子对比](#12-边缘检测算子对比)
    - [1.3 Canny算法优势](#13-canny算法优势)
  - [2. Canny算法四步骤详解](#2-canny算法四步骤详解)
    - [2.1 高斯滤波(Gaussian Smoothing)](#21-高斯滤波gaussian-smoothing)
    - [2.2 梯度计算(Gradient Computation)](#22-梯度计算gradient-computation)
    - [2.3 非极大值抑制(Non-Maximum Suppression)](#23-非极大值抑制non-maximum-suppression)
    - [2.4 双阈值检测(Double Threshold)](#24-双阈值检测double-threshold)
  - [3. 完整C语言实现](#3-完整c语言实现)
    - [3.1 数据结构定义](#31-数据结构定义)
    - [3.2 PPM图像I/O实现](#32-ppm图像io实现)
    - [3.3 Canny算法核心实现](#33-canny算法核心实现)
    - [3.4 主程序和示例](#34-主程序和示例)
  - [4. 优化技巧与实时处理](#4-优化技巧与实时处理)
    - [4.1 性能优化策略](#41-性能优化策略)
    - [4.2 实时处理优化要点](#42-实时处理优化要点)
    - [4.3 固定点优化版本](#43-固定点优化版本)
  - [5. 总结](#5-总结)


---

## 1. 边缘检测理论基础

### 1.1 边缘检测概述

边缘检测是计算机视觉中的基础任务，旨在识别图像中亮度变化剧烈的区域。
边缘通常对应于物体的边界、表面方向变化或光照变化。

**边缘类型：**

- **阶跃边缘(Step Edge)**：灰度值发生突然跳变
- **屋顶边缘(Roof Edge)**：灰度值先增加后减少，形成峰值
- **线条边缘(Line Edge)**：灰度值在短距离内发生变化

### 1.2 边缘检测算子对比

| 算子 | 特点 | 优点 | 缺点 |
|------|------|------|------|
| Sobel | 一阶微分 | 计算简单，噪声平滑 | 边缘较粗，定位精度低 |
| Prewitt | 一阶微分 | 实现简单 | 对噪声敏感 |
| Roberts | 一阶微分 | 边缘定位较准 | 对噪声敏感 |
| Laplacian | 二阶微分 | 边缘定位准确 | 对噪声敏感 |
| Canny | 多阶段优化 | 信噪比高，定位准 | 计算复杂 |

### 1.3 Canny算法优势

Canny边缘检测算法由John F. Canny于1986年提出，满足以下三个最优准则：

1. **信噪比准则**：最大化检测到的真实边缘，最小化误检
2. **定位精度准则**：检测到的边缘应尽可能接近真实边缘位置
3. **单边缘响应准则**：对每个真实边缘只产生一个响应

---

## 2. Canny算法四步骤详解

### 2.1 高斯滤波(Gaussian Smoothing)

高斯滤波用于平滑图像，抑制噪声，是Canny算法的第一步。

**高斯函数公式：**

$$G(x, y) = \frac{1}{2\pi\sigma^2}e^{-\frac{x^2 + y^2}{2\sigma^2}}$$

**离散高斯核生成：**

```c
/* 生成高斯滤波核 */
static void generate_gaussian_kernel(double *kernel, int size, double sigma)
{
    int half_size = size / 2;
    double sum = 0.0;

    for (int i = -half_size; i <= half_size; i++) {
        for (int j = -half_size; j <= half_size; j++) {
            double value = (1.0 / (2.0 * M_PI * sigma * sigma)) *
                          exp(-(i * i + j * j) / (2.0 * sigma * sigma));
            kernel[(i + half_size) * size + (j + half_size)] = value;
            sum += value;
        }
    }

    /* 归一化 */
    for (int i = 0; i < size * size; i++) {
        kernel[i] /= sum;
    }
}
```

### 2.2 梯度计算(Gradient Computation)

使用Sobel算子计算图像梯度的幅值和方向。

**Sobel算子：**

$$S_x = \begin{bmatrix} -1 & 0 & 1 \\ -2 & 0 & 2 \\ -1 & 0 & 1 \end{bmatrix}, \quad S_y = \begin{bmatrix} -1 & -2 & -1 \\ 0 & 0 & 0 \\ 1 & 2 & 1 \end{bmatrix}$$

**梯度计算：**

- 梯度幅值：$G = \sqrt{G_x^2 + G_y^2}$
- 梯度方向：$\theta = \arctan(\frac{G_y}{G_x})$

### 2.3 非极大值抑制(Non-Maximum Suppression)

NMS用于细化边缘，将宽边缘变为单像素宽的边缘。

**原理：**

- 将梯度方向量化为4个方向(0°、45°、90°、135°)
- 沿梯度方向比较相邻像素的梯度幅值
- 仅保留局部最大值点

### 2.4 双阈值检测(Double Threshold)

通过高、低两个阈值区分强边缘、弱边缘和非边缘。

**分类规则：**

- 强边缘：梯度幅值 > 高阈值
- 弱边缘：低阈值 ≤ 梯度幅值 ≤ 高阈值
- 非边缘：梯度幅值 < 低阈值

**边缘连接：**

- 强边缘点一定是边缘
- 弱边缘点如果与强边缘相连，则保留为边缘

---

## 3. 完整C语言实现

### 3.1 数据结构定义

```c
/* canny_edge_detector.h */
#ifndef CANNY_EDGE_DETECTOR_H
#define CANNY_EDGE_DETECTOR_H

#include <stdint.h>
#include <stdbool.h>

/* 图像数据结构 */
typedef struct {
    uint8_t *data;      /* 灰度图像数据 */
    int width;          /* 图像宽度 */
    int height;         /* 图像高度 */
} Image;

/* Canny检测参数 */
typedef struct {
    double sigma;       /* 高斯滤波标准差 */
    double low_thresh;  /* 低阈值 (0.0-1.0) */
    double high_thresh; /* 高阈值 (0.0-1.0) */
} CannyParams;

/* 默认参数 */
#define DEFAULT_SIGMA       1.0
#define DEFAULT_LOW_THRESH  0.05
#define DEFAULT_HIGH_THRESH 0.15
#define GAUSSIAN_KERNEL_SIZE 5

/* 函数声明 */
Image* image_create(int width, int height);
void image_free(Image *img);
Image* image_load_ppm(const char *filename);
bool image_save_ppm(const Image *img, const char *filename);
Image* canny_edge_detect(const Image *src, const CannyParams *params);

#endif /* CANNY_EDGE_DETECTOR_H */
```

### 3.2 PPM图像I/O实现

```c
/* ppm_io.c - PPM格式图像读写 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "canny_edge_detector.h"

/* 跳过PPM文件中的注释 */
static void skip_comments(FILE *fp)
{
    int c;
    while ((c = fgetc(fp)) == '#') {
        while ((c = fgetc(fp)) != '\n' && c != EOF);
    }
    ungetc(c, fp);
}

/* 加载PPM图像(P6格式) */
Image* image_load_ppm(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "无法打开文件: %s\n", filename);
        return NULL;
    }

    /* 读取魔数 */
    char magic[3];
    if (fscanf(fp, "%2s", magic) != 1 || magic[0] != 'P' ||
        (magic[1] != '6' && magic[1] != '5')) {
        fprintf(stderr, "不支持的PPM格式: %s\n", magic);
        fclose(fp);
        return NULL;
    }

    /* 跳过空白和注释 */
    skip_comments(fp);

    /* 读取宽度、高度 */
    int width, height, maxval;
    if (fscanf(fp, "%d %d", &width, &height) != 2) {
        fprintf(stderr, "读取图像尺寸失败\n");
        fclose(fp);
        return NULL;
    }

    skip_comments(fp);

    /* 读取最大像素值 */
    if (fscanf(fp, "%d", &maxval) != 1 || maxval > 255) {
        fprintf(stderr, "不支持的像素值范围\n");
        fclose(fp);
        return NULL;
    }

    /* 跳到图像数据 */
    fgetc(fp);

    /* 创建图像 */
    Image *img = image_create(width, height);
    if (!img) {
        fclose(fp);
        return NULL;
    }

    /* 读取像素数据 */
    if (magic[1] == '6') {
        /* P6: RGB格式，转换为灰度 */
        uint8_t *rgb = malloc(width * height * 3);
        if (!rgb || fread(rgb, 3, width * height, fp) != (size_t)(width * height)) {
            fprintf(stderr, "读取RGB数据失败\n");
            free(rgb);
            image_free(img);
            fclose(fp);
            return NULL;
        }
        /* RGB转灰度 */
        for (int i = 0; i < width * height; i++) {
            img->data[i] = (uint8_t)(0.299 * rgb[i * 3] +
                                     0.587 * rgb[i * 3 + 1] +
                                     0.114 * rgb[i * 3 + 2]);
        }
        free(rgb);
    } else {
        /* P5: 灰度格式 */
        if (fread(img->data, 1, width * height, fp) != (size_t)(width * height)) {
            fprintf(stderr, "读取灰度数据失败\n");
            image_free(img);
            fclose(fp);
            return NULL;
        }
    }

    fclose(fp);
    return img;
}

/* 保存PPM图像 */
bool image_save_ppm(const Image *img, const char *filename)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "无法创建文件: %s\n", filename);
        return false;
    }

    /* 写入P5格式(灰度) */
    fprintf(fp, "P5\n%d %d\n255\n", img->width, img->height);
    fwrite(img->data, 1, img->width * img->height, fp);

    fclose(fp);
    return true;
}

/* 创建图像 */
Image* image_create(int width, int height)
{
    Image *img = malloc(sizeof(Image));
    if (!img) return NULL;

    img->width = width;
    img->height = height;
    img->data = calloc((size_t)width * height, sizeof(uint8_t));

    if (!img->data) {
        free(img);
        return NULL;
    }
    return img;
}

/* 释放图像 */
void image_free(Image *img)
{
    if (img) {
        free(img->data);
        free(img);
    }
}
```

### 3.3 Canny算法核心实现

```c
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
```

### 3.4 主程序和示例

```c
/* main.c - 示例程序 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include "canny_edge_detector.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("用法: %s <输入PPM图像> [sigma] [low_thresh] [high_thresh]\n", argv[0]);
        printf("  sigma: 高斯滤波标准差 (默认: 1.0)\n");
        printf("  low_thresh: 低阈值 (默认: 0.05)\n");
        printf("  high_thresh: 高阈值 (默认: 0.15)\n");
        return 1;
    }

    /* 解析参数 */
    CannyParams params = {
        .sigma = (argc > 2) ? atof(argv[2]) : DEFAULT_SIGMA,
        .low_thresh = (argc > 3) ? atof(argv[3]) : DEFAULT_LOW_THRESH,
        .high_thresh = (argc > 4) ? atof(argv[4]) : DEFAULT_HIGH_THRESH
    };

    printf("Canny边缘检测参数:\n");
    printf("  Sigma: %.2f\n", params.sigma);
    printf("  低阈值: %.3f\n", params.low_thresh);
    printf("  高阈值: %.3f\n", params.high_thresh);

    /* 加载图像 */
    printf("\n加载图像: %s\n", argv[1]);
    clock_t start = clock();

    Image *src = image_load_ppm(argv[1]);
    if (!src) {
        fprintf(stderr, "图像加载失败\n");
        return 1;
    }

    printf("图像尺寸: %d x %d\n", src->width, src->height);

    /* 执行Canny边缘检测 */
    printf("\n执行Canny边缘检测...\n");
    Image *edges = canny_edge_detect(src, &params);

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    if (!edges) {
        fprintf(stderr, "边缘检测失败\n");
        image_free(src);
        return 1;
    }

    /* 保存结果 */
    const char *output_name = "edges_output.ppm";
    if (image_save_ppm(edges, output_name)) {
        printf("边缘图像已保存: %s\n", output_name);
    }

    printf("处理时间: %.3f秒\n", elapsed);
    printf("处理速度: %.2f MP/s\n",
           (src->width * src->height / 1000000.0) / elapsed);

    /* 清理 */
    image_free(src);
    image_free(edges);

    return 0;
}
```

---

## 4. 优化技巧与实时处理

### 4.1 性能优化策略

```c
/* optimized_canny.c - 优化版本 */
#include <immintrin.h>  /* SIMD指令 */

/* SSE优化的Sobel梯度计算 */
static void sobel_gradient_sse(const uint8_t *src, float *grad_x,
                                float *grad_y, int width, int height)
{
    /* 使用SIMD并行处理4个像素 */
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x += 4) {
            /* 加载3x3邻域到SSE寄存器 */
            __m128i r0, r1, r2;
            r0 = _mm_loadu_si128((__m128i*)(src + (y-1)*width + x - 1));
            r1 = _mm_loadu_si128((__m128i*)(src + y*width + x - 1));
            r2 = _mm_loadu_si128((__m128i*)(src + (y+1)*width + x - 1));

            /* 计算Gx = (-1)*p00 + 1*p02 + (-2)*p10 + 2*p12 + (-1)*p20 + 1*p22 */
            /* 这里需要解包和转换... */
        }
    }
}

/* 多线程并行处理 */
#include <pthread.h>

typedef struct {
    uint8_t *src;
    double *dst;
    int start_row;
    int end_row;
    int width;
    int height;
    double sigma;
} ThreadArgs;

static void* gaussian_thread(void *arg)
{
    ThreadArgs *args = (ThreadArgs*)arg;
    /* 处理指定行范围 */
    for (int y = args->start_row; y < args->end_row; y++) {
        /* 行处理逻辑 */
    }
    return NULL;
}
```

### 4.2 实时处理优化要点

| 优化技术 | 描述 | 性能提升 |
|---------|------|---------|
| **积分图** | 预计算累积和，加速平均滤波 | 2-3x |
| **查表法** | 预计算高斯核值，避免重复exp计算 | 1.5x |
| **SIMD并行** | SSE/AVX并行处理多个像素 | 4-8x |
| **GPU加速** | 使用OpenCL/CUDA | 10-100x |
| **下采样** | 先缩小图像处理，再映射回原图 | 2-4x |

### 4.3 固定点优化版本

```c
/* fixed_point_canny.c - 无浮点运算版本，适合嵌入式 */
#define FIXED_SHIFT 8
#define FIXED_SCALE (1 << FIXED_SHIFT)
#define TO_FIXED(x) ((int)((x) * FIXED_SCALE))
#define FROM_FIXED(x) ((x) >> FIXED_SHIFT)

/* 预计算的整数高斯核 (sigma=1.0, 3x3) */
static const int gaussian_kernel_3x3[9] = {
    16, 32, 16,
    32, 64, 32,
    16, 32, 16
}; /* 和为256 = 2^8，便于移位除法 */

static void gaussian_smooth_fixed(const uint8_t *src, int16_t *dst,
                                   int width, int height)
{
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sum = 0;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    sum += src[(y+ky)*width + (x+kx)] *
                           gaussian_kernel_3x3[(ky+1)*3 + (kx+1)];
                }
            }
            dst[y*width + x] = sum >> 8;  /* 除以256 */
        }
    }
}
```

---

## 5. 总结

Canny边缘检测算法是计算机视觉领域最广泛使用的边缘检测方法。通过本文的实现，我们学习了：

1. **理论基础**：边缘检测的数学原理和Canny三准则
2. **算法流程**：四步骤的详细实现
3. **工程实践**：完整的C语言代码，包括图像I/O和参数调优
4. **性能优化**：SIMD、多线程、固定点等优化技术

**调试建议：**

- 高斯sigma值过大可能导致边缘模糊
- 低/高阈值比例建议在1:2到1:3之间
- 对于噪声较多的图像，可适当增大sigma值
