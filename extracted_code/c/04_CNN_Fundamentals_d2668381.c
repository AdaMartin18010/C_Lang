/*
 * Auto-generated from: 11_Machine_Learning_C\04_CNN_Fundamentals.md
 * Line: 387
 * Language: c
 * Block ID: d2668381
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
