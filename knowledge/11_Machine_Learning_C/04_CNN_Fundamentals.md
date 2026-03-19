# 卷积神经网络基础 (CNN Fundamentals)

> **难度**: L5 | **预估学习时间**: 5-6小时
> 包含：卷积层、池化层、前向/反向传播、完整CNN实现

---

## 1. 卷积神经网络概述

CNN专门用于处理具有**网格结构**的数据（如图像），通过**局部连接**和**权值共享**大幅减少参数数量。

### 1.1 核心组件

```text
输入图像 → [卷积层 → 激活 → 池化] × N → 全连接层 → 输出
```

| 组件 | 功能 | 参数 |
|:-----|:-----|:-----|
| **卷积层** | 特征提取 | 卷积核权重、偏置 |
| **激活函数** | 引入非线性 | 无参数 (ReLU) |
| **池化层** | 降维、平移不变 | 无参数 |
| **全连接层** | 分类 | 权重矩阵、偏置 |

---


---

## 📑 目录

- [卷积神经网络基础 (CNN Fundamentals)](#卷积神经网络基础-cnn-fundamentals)
  - [1. 卷积神经网络概述](#1-卷积神经网络概述)
    - [1.1 核心组件](#11-核心组件)
  - [📑 目录](#-目录)
  - [2. 卷积操作详解](#2-卷积操作详解)
    - [2.1 二维卷积](#21-二维卷积)
    - [2.2 C语言实现](#22-c语言实现)
    - [2.3 im2col优化](#23-im2col优化)
  - [3. 池化层](#3-池化层)
    - [3.1 最大池化](#31-最大池化)
    - [3.2 平均池化](#32-平均池化)
  - [4. 激活函数](#4-激活函数)
    - [4.1 ReLU与变体](#41-relu与变体)
    - [4.2 Softmax](#42-softmax)
  - [5. 反向传播](#5-反向传播)
    - [5.1 卷积层反向传播](#51-卷积层反向传播)
  - [6. 完整CNN示例：MNIST分类器](#6-完整cnn示例mnist分类器)
  - [7. 性能优化技巧](#7-性能优化技巧)
    - [7.1 内存布局](#71-内存布局)
    - [7.2 Winograd卷积](#72-winograd卷积)
  - [8. 参考文献](#8-参考文献)


---

## 2. 卷积操作详解

### 2.1 二维卷积

卷积核在输入上滑动，计算点积：

```text
输入: 5×5          卷积核: 3×3          输出: 3×3
┌─┬─┬─┬─┐        ┌─┬─┐
│1│2│3│4│5│       │1│0│-1│    计算: 1×1 + 2×0 + 3×(-1) +
├─┼─┼─┼─┤        ├─┼─┤         6×1 + 7×0 + 8×(-1) +
│6│7│8│9│10│  *  │1│0│-1│  =      11×1 + 12×0 + 13×(-1)
├─┼─┼─┼─┤        ├─┼─┤         = 1 - 3 + 6 - 8 + 11 - 13 = -6
│11│12│13│14│15│   │1│0│-1│
└─┴─┴─┴─┘        └─┴─┘
```

### 2.2 C语言实现

```c
typedef struct {
    int width, height, channels;
    float* data;  // 按CHW排列: data[c*H*W + h*W + w]
} Tensor;

typedef struct {
    int kernel_size;
    int in_channels, out_channels;
    float* weights;  // [out][in][kh][kw]
    float* bias;
    int stride, padding;
} Conv2D_Layer;

// 卷积前向传播
Tensor* conv2d_forward(Conv2D_Layer* layer, Tensor* input) {
    int k = layer->kernel_size;
    int out_h = (input->height + 2*layer->padding - k) / layer->stride + 1;
    int out_w = (input->width + 2*layer->padding - k) / layer->stride + 1;

    Tensor* output = tensor_create(out_w, out_h, layer->out_channels);

    for (int oc = 0; oc < layer->out_channels; oc++) {
        for (int oh = 0; oh < out_h; oh++) {
            for (int ow = 0; ow < out_w; ow++) {
                float sum = layer->bias[oc];

                for (int ic = 0; ic < layer->in_channels; ic++) {
                    for (int kh = 0; kh < k; kh++) {
                        for (int kw = 0; kw < k; kw++) {
                            int ih = oh * layer->stride + kh - layer->padding;
                            int iw = ow * layer->stride + kw - layer->padding;

                            if (ih >= 0 && ih < input->height &&
                                iw >= 0 && iw < input->width) {
                                float input_val = tensor_get(input, iw, ih, ic);
                                float weight = layer->weights[((oc * layer->in_channels + ic) * k + kh) * k + kw];
                                sum += input_val * weight;
                            }
                        }
                    }
                }
                tensor_set(output, ow, oh, oc, sum);
            }
        }
    }
    return output;
}
```

### 2.3 im2col优化

将卷积转换为矩阵乘法以利用BLAS：

```c
// im2col: 将输入图像块展开为列
void im2col(const float* data_im, int channels, int height, int width,
            int kernel_h, int kernel_w, int stride, int pad,
            float* data_col) {
    int height_col = (height + 2*pad - kernel_h) / stride + 1;
    int width_col = (width + 2*pad - kernel_w) / stride + 1;
    int channels_col = channels * kernel_h * kernel_w;

    for (int c = 0; c < channels_col; c++) {
        int w_offset = c % kernel_w;
        int h_offset = (c / kernel_w) % kernel_h;
        int c_im = c / kernel_h / kernel_w;

        for (int h = 0; h < height_col; h++) {
            for (int w = 0; w < width_col; w++) {
                int im_row = h_offset + h * stride;
                int im_col = w_offset + w * stride;
                int col_index = (c * height_col + h) * width_col + w;

                if (im_row - pad >= 0 && im_row - pad < height &&
                    im_col - pad >= 0 && im_col - pad < width)
                    data_col[col_index] = data_im[(c_im * height + (im_row - pad)) * width + (im_col - pad)];
                else
                    data_col[col_index] = 0;
            }
        }
    }
}

// 使用BLAS的卷积
void conv2d_blas(Conv2D_Layer* layer, Tensor* input, Tensor* output) {
    int k = layer->kernel_size;
    int out_h = output->height;
    int out_w = output->width;

    // 分配im2col缓冲区
    int col_size = layer->in_channels * k * k * out_h * out_w;
    float* col_buffer = malloc(col_size * sizeof(float));

    // im2col转换
    im2col(input->data, layer->in_channels, input->height, input->width,
           k, k, layer->stride, layer->padding, col_buffer);

    // 矩阵乘法: output = weights * col_buffer
    // 使用OpenBLAS cblas_sgemm
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                layer->out_channels, out_h * out_w, layer->in_channels * k * k,
                1.0f, layer->weights, layer->in_channels * k * k,
                col_buffer, out_h * out_w,
                0.0f, output->data, out_h * out_w);

    // 添加偏置
    for (int oc = 0; oc < layer->out_channels; oc++) {
        for (int i = 0; i < out_h * out_w; i++) {
            output->data[oc * out_h * out_w + i] += layer->bias[oc];
        }
    }

    free(col_buffer);
}
```

---

## 3. 池化层

### 3.1 最大池化

```c
typedef struct {
    int pool_size;
    int stride;
    int* max_indices;  // 用于反向传播
} MaxPool2D;

Tensor* maxpool2d_forward(MaxPool2D* pool, Tensor* input, Tensor* output) {
    int p = pool->pool_size;
    int out_h = output->height;
    int out_w = output->width;

    pool->max_indices = malloc(output->channels * out_h * out_w * sizeof(int));

    for (int c = 0; c < input->channels; c++) {
        for (int oh = 0; oh < out_h; oh++) {
            for (int ow = 0; ow < out_w; ow++) {
                float max_val = -INFINITY;
                int max_idx = 0;

                for (int ph = 0; ph < p; ph++) {
                    for (int pw = 0; pw < p; pw++) {
                        int ih = oh * pool->stride + ph;
                        int iw = ow * pool->stride + pw;
                        float val = tensor_get(input, iw, ih, c);

                        if (val > max_val) {
                            max_val = val;
                            max_idx = ph * p + pw;
                        }
                    }
                }
                tensor_set(output, ow, oh, c, max_val);
                pool->max_indices[(c * out_h + oh) * out_w + ow] = max_idx;
            }
        }
    }
    return output;
}
```

### 3.2 平均池化

```c
Tensor* avgpool2d_forward(Tensor* input, int pool_size, int stride) {
    int out_h = (input->height - pool_size) / stride + 1;
    int out_w = (input->width - pool_size) / stride + 1;
    Tensor* output = tensor_create(out_w, out_h, input->channels);

    for (int c = 0; c < input->channels; c++) {
        for (int oh = 0; oh < out_h; oh++) {
            for (int ow = 0; ow < out_w; ow++) {
                float sum = 0;
                for (int ph = 0; ph < pool_size; ph++) {
                    for (int pw = 0; pw < pool_size; pw++) {
                        int ih = oh * stride + ph;
                        int iw = ow * stride + pw;
                        sum += tensor_get(input, iw, ih, c);
                    }
                }
                tensor_set(output, ow, oh, c, sum / (pool_size * pool_size));
            }
        }
    }
    return output;
}
```

---

## 4. 激活函数

### 4.1 ReLU与变体

```c
// ReLU: max(0, x)
void relu_forward(float* data, int n) {
    for (int i = 0; i < n; i++) {
        if (data[i] < 0) data[i] = 0;
    }
}

void relu_backward(const float* grad_output, const float* input,
                   float* grad_input, int n) {
    for (int i = 0; i < n; i++) {
        grad_input[i] = (input[i] > 0) ? grad_output[i] : 0;
    }
}

// Leaky ReLU: max(alpha*x, x)
void leaky_relu_forward(float* data, int n, float alpha) {
    for (int i = 0; i < n; i++) {
        data[i] = (data[i] > 0) ? data[i] : alpha * data[i];
    }
}
```

### 4.2 Softmax

```c
void softmax(const float* input, float* output, int n) {
    // 数值稳定性：减去最大值
    float max_val = input[0];
    for (int i = 1; i < n; i++) {
        if (input[i] > max_val) max_val = input[i];
    }

    float sum = 0;
    for (int i = 0; i < n; i++) {
        output[i] = expf(input[i] - max_val);
        sum += output[i];
    }

    for (int i = 0; i < n; i++) {
        output[i] /= sum;
    }
}
```

---

## 5. 反向传播

### 5.1 卷积层反向传播

```c
typedef struct {
    Tensor* grad_weights;
    Tensor* grad_bias;
    Tensor* grad_input;
} Conv2D_Grads;

void conv2d_backward(Conv2D_Layer* layer, Tensor* input, Tensor* grad_output,
                     Conv2D_Grads* grads) {
    int k = layer->kernel_size;
    int out_h = grad_output->height;
    int out_w = grad_output->width;

    // 初始化梯度
    memset(grads->grad_weights->data, 0, grads->grad_weights->size * sizeof(float));
    memset(grads->grad_bias->data, 0, grads->grad_bias->size * sizeof(float));
    memset(grads->grad_input->data, 0, grads->grad_input->size * sizeof(float));

    for (int oc = 0; oc < layer->out_channels; oc++) {
        // 偏置梯度
        float bias_grad = 0;
        for (int oh = 0; oh < out_h; oh++) {
            for (int ow = 0; ow < out_w; ow++) {
                bias_grad += tensor_get(grad_output, ow, oh, oc);
            }
        }
        grads->grad_bias->data[oc] = bias_grad;

        for (int ic = 0; ic < layer->in_channels; ic++) {
            for (int kh = 0; kh < k; kh++) {
                for (int kw = 0; kw < k; kw++) {
                    float weight_grad = 0;

                    for (int oh = 0; oh < out_h; oh++) {
                        for (int ow = 0; ow < out_w; ow++) {
                            int ih = oh * layer->stride + kh - layer->padding;
                            int iw = ow * layer->stride + kw - layer->padding;

                            if (ih >= 0 && ih < input->height &&
                                iw >= 0 && iw < input->width) {
                                float g_out = tensor_get(grad_output, ow, oh, oc);
                                float inp = tensor_get(input, iw, ih, ic);
                                weight_grad += g_out * inp;

                                // 输入梯度
                                float w = layer->weights[((oc * layer->in_channels + ic) * k + kh) * k + kw];
                                float* g_in = &grads->grad_input->data[(ic * input->height + ih) * input->width + iw];
                                *g_in += g_out * w;
                            }
                        }
                    }
                    grads->grad_weights->data[((oc * layer->in_channels + ic) * k + kh) * k + kw] = weight_grad;
                }
            }
        }
    }
}
```

---

## 6. 完整CNN示例：MNIST分类器

```c
// 简化的LeNet-5风格网络
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// 网络结构: Conv(1->6, 5x5) -> Pool(2x2) -> Conv(6->16, 5x5) -> Pool(2x2) -> FC

typedef struct {
    Conv2D_Layer conv1;  // 1x28x28 -> 6x24x24
    MaxPool2D pool1;     // 6x24x24 -> 6x12x12
    Conv2D_Layer conv2;  // 6x12x12 -> 16x8x8
    MaxPool2D pool2;     // 16x8x8 -> 16x4x4 = 256
    float fc1_weights[120][256];  // 256 -> 120
    float fc1_bias[120];
    float fc2_weights[84][120];   // 120 -> 84
    float fc2_bias[84];
    float fc3_weights[10][84];    // 84 -> 10
    float fc3_bias[10];
} LeNet5;

void lenet5_init(LeNet5* net) {
    // Xavier初始化
    float scale = sqrtf(2.0f / (1 * 5 * 5));
    for (int i = 0; i < 6 * 1 * 5 * 5; i++) {
        net->conv1.weights[i] = ((float)rand() / RAND_MAX - 0.5f) * 2 * scale;
    }
    memset(net->conv1.bias, 0, sizeof(net->conv1.bias));
    // ... 类似初始化其他层
}

void lenet5_forward(LeNet5* net, float* input, float* output) {
    Tensor in = {28, 28, 1, input};

    // Conv1 + ReLU + Pool1
    Tensor* t1 = conv2d_forward(&net->conv1, &in);
    relu_forward(t1->data, t1->channels * t1->height * t1->width);
    Tensor* t2 = tensor_create(12, 12, 6);
    maxpool2d_forward(&net->pool1, t1, t2);

    // Conv2 + ReLU + Pool2
    Tensor* t3 = conv2d_forward(&net->conv2, t2);
    relu_forward(t3->data, t3->channels * t3->height * t3->width);
    Tensor* t4 = tensor_create(4, 4, 16);
    maxpool2d_forward(&net->pool2, t3, t4);

    // Flatten: 16x4x4 = 256
    float flat[256];
    memcpy(flat, t4->data, 256 * sizeof(float));

    // FC layers with ReLU
    float fc1_out[120], fc2_out[84];
    // ... 矩阵乘法 + ReLU

    // Output softmax
    softmax(fc2_out, output, 10);

    // 清理
    free_tensor(t1); free_tensor(t2);
    free_tensor(t3); free_tensor(t4);
}

// 交叉熵损失
float cross_entropy_loss(const float* pred, int label) {
    return -logf(pred[label] + 1e-8f);
}
```

---

## 7. 性能优化技巧

### 7.1 内存布局

```c
// NCHW vs NHWC
// NCHW (Batch, Channel, Height, Width) - cuDNN默认
// NHWC - TensorFlow默认，CPU缓存友好

// 使用SIMD向量化
#include <immintrin.h>

void relu_avx(float* data, int n) {
    __m256 zero = _mm256_setzero_ps();
    int i = 0;
    for (; i <= n - 8; i += 8) {
        __m256 x = _mm256_loadu_ps(&data[i]);
        x = _mm256_max_ps(x, zero);
        _mm256_storeu_ps(&data[i], x);
    }
    // 处理剩余元素
    for (; i < n; i++) {
        if (data[i] < 0) data[i] = 0;
    }
}
```

### 7.2 Winograd卷积

对于小卷积核(3x3)，Winograd算法减少乘法次数：

```text
标准卷积: 3x3 = 9次乘法/输出
Winograd F(2x2, 3x3): 4次乘法/输出 (2.25x加速)
```

---

## 8. 参考文献

1. **LeCun, Y.** et al. (1998). "Gradient-based learning applied to document recognition". *Proceedings of the IEEE*.
2. **Krizhevsky, A.** et al. (2012). "ImageNet Classification with Deep Convolutional Neural Networks". *NIPS*.
3. **Chetlur, S.** et al. (2014). "cuDNN: Efficient primitives for deep learning". arXiv:1410.0759.
4. **Lavin, A. & Gray, S.** (2016). "Fast Algorithms for Convolutional Neural Networks". *CVPR*.

---

> **关联文档**: [神经网络基础](./01_Neural_Network_Basics.md) | [梯度下降优化器](./03_Gradient_Descent_Optimizers.md) | [RNN与LSTM](./05_RNN_LSTM_Basics.md)


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
