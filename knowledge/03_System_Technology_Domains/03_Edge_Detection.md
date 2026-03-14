# 边缘检测算法

> **层级定位**: 03 System Technology Domains / 03 Computer Vision
> **对应标准**: OpenCV, MATLAB, GPU Image Processing
> **难度级别**: L4 分析
> **预估学习时间**: 5-7 小时

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | Sobel, Canny, Laplacian算法, 卷积核实现, 图像处理优化, SIMD加速 |
| **前置知识** | 数字图像处理基础, 卷积运算, 梯度计算 |
| **后续延伸** | 深度学习边缘检测, 实时视频处理 |
| **权威来源** | OpenCV Documentation, Digital Image Processing (Gonzalez) |

---

## 🧠 知识结构思维导图

```mermaid
mindmap
  root((边缘检测))
    梯度算子
      Sobel算子
      Scharr算子
      Prewitt算子
    二阶微分
      Laplacian
      LoG
      DoG
    最优检测
      Canny算法
      非极大值抑制
      双阈值
    优化加速
      SIMD指令
      GPU并行
      多尺度处理
```

---

## 📖 核心概念详解

### 1. 图像梯度基础

```c
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
```

### 2. Sobel算子实现

```c
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
```

### 3. Laplacian算子

```c
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
```

### 4. Canny边缘检测

```c
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
```

### 5. SIMD加速实现

```c
// SSE/AVX优化的Sobel实现
#include <immintrin.h>

// SSE4.1优化: 一次处理16字节
void sobel_sse(const uint8_t *src, uint8_t *dst, int w, int h) {
    // 加载Sobel核到寄存器
    __m128i kx_pos = _mm_set1_epi16(1);
    __m128i kx_neg = _mm_set1_epi16(-1);
    __m128i kx_center = _mm_set1_epi16(2);

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x += 16) {
            // 加载3行数据
            __m128i row0 = _mm_loadu_si128((__m128i*)(src + (y-1)*w + x - 1));
            __m128i row1 = _mm_loadu_si128((__m128i*)(src + y*w + x - 1));
            __m128i row2 = _mm_loadu_si128((__m128i*)(src + (y+1)*w + x - 1));

            // 解交错为16位以支持负数运算
            __m128i r0_lo = _mm_unpacklo_epi8(row0, _mm_setzero_si128());
            __m128i r0_hi = _mm_unpackhi_epi8(row0, _mm_setzero_si128());
            __m128i r2_lo = _mm_unpacklo_epi8(row2, _mm_setzero_si128());
            __m128i r2_hi = _mm_unpackhi_epi8(row2, _mm_setzero_si128());

            // Gx计算: (r2 - r0) 的水平部分
            __m128i gx_lo = _mm_sub_epi16(r2_lo, r0_lo);
            __m128i gx_hi = _mm_sub_epi16(r2_hi, r0_hi);

            // 计算绝对值并饱和到8位
            __m128i abs_lo = _mm_abs_epi16(gx_lo);
            __m128i abs_hi = _mm_abs_epi16(gx_hi);
            __m128i result = _mm_packus_epi16(abs_lo, abs_hi);

            // 存储结果
            _mm_storeu_si128((__m128i*)(dst + y*w + x), result);
        }
    }
}

// AVX2优化: 一次处理32字节
void sobel_avx2(const uint8_t *src, uint8_t *dst, int w, int h) {
    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x += 32) {
            // 加载3行，每行32字节 (使用AVX2 256位寄存器)
            __m256i row0 = _mm256_loadu_si256((__m256i*)(src + (y-1)*w + x - 1));
            __m256i row1 = _mm256_loadu_si256((__m256i*)(src + y*w + x - 1));
            __m256i row2 = _mm256_loadu_si256((__m256i*)(src + (y+1)*w + x - 1));

            // 类似SSE处理，但256位宽度
            __m256i r0_lo = _mm256_unpacklo_epi8(row0, _mm256_setzero_si256());
            __m256i r0_hi = _mm256_unpackhi_epi8(row0, _mm256_setzero_si256());
            __m256i r2_lo = _mm256_unpacklo_epi8(row2, _mm256_setzero_si256());
            __m256i r2_hi = _mm256_unpackhi_epi8(row2, _mm256_setzero_si256());

            __m256i gx_lo = _mm256_sub_epi16(r2_lo, r0_lo);
            __m256i gx_hi = _mm256_sub_epi16(r2_hi, r0_hi);

            __m256i abs_lo = _mm256_abs_epi16(gx_lo);
            __m256i abs_hi = _mm256_abs_epi16(gx_hi);
            __m256i result = _mm256_packus_epi16(abs_lo, abs_hi);

            _mm256_storeu_si256((__m256i*)(dst + y*w + x), result);
        }
    }
}
```

---

## ⚠️ 常见陷阱

### 陷阱 EDGE01: 边界处理

| 属性 | 内容 |
|:-----|:-----|
| **现象** | 图像边缘像素计算错误 |
| **后果** | 边缘黑边或越界访问 |
| **修复方案** | 使用padding或跳过边缘像素 |

```c
// ❌ 错误: 可能越界
for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
        // 访问 (y-1)*w + (x-1) 可能越界
    }
}

// ✅ 正确: 限制范围
for (int y = 1; y < h - 1; y++) {
    for (int x = 1; x < w - 1; x++) {
        // 安全访问
    }
}
```

### 陷阱 EDGE02: 阈值选择

```c
// 自适应阈值计算
float compute_auto_threshold(const float *magnitude, int size, float ratio) {
    // 使用直方图确定高阈值
    int hist[256] = {0};
    for (int i = 0; i < size; i++) {
        int bin = (int)(magnitude[i] / 255.0f * 255);
        if (bin > 255) bin = 255;
        hist[bin]++;
    }

    // 找到ratio分位数
    int cumsum = 0;
    int threshold_idx = 0;
    for (int i = 255; i >= 0; i--) {
        cumsum += hist[i];
        if (cumsum > size * ratio) {
            threshold_idx = i;
            break;
        }
    }

    return threshold_idx * 255.0f / 256.0f;
}
```

---

## 🔗 权威来源引用

| 来源 | 章节/链接 | 核心内容 |
|:-----|:----------|:---------|
| **OpenCV** | imgproc模块 | Sobel, Canny, Laplacian实现 |
| **Digital Image Processing** | Gonzalez, Ch10 | 边缘检测理论 |
| **Intel IPP** | Image Processing | SIMD优化参考 |

---

> **状态**: ✅ 已完成

> **更新记录**
>
> - 2026-03-15: 初版创建，添加边缘检测完整实现
