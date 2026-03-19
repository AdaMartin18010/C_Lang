/*
 * Auto-generated from: 11_Machine_Learning_C\03_Embedded_AI_Deployment.md
 * Line: 843
 * Language: c
 * Block ID: 248ad3ea
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file dynamic_quantization.c
 * @brief 动态量化实现
 *
 * 动态量化特点:
 * - 权重静态量化 (离线)
 * - 激活动态量化 (运行时)
 * - 适合RNN/LSTM等时序网络
 */

#include "quantization_types.h"
#include <float.h>

/**
 * @brief 运行时动态计算量化参数
 * @param data 输入数据
 * @param size 数据大小
 * @param qparams 输出量化参数
 */
void dynamic_calc_quant_params(const float* data,
                                uint32_t size,
                                quantization_params_t* qparams)
{
    /* 1. 寻找最小最大值 */
    float rmin = FLT_MAX;
    float rmax = -FLT_MAX;

    for (uint32_t i = 0; i < size; i++) {
        if (data[i] < rmin) rmin = data[i];
        if (data[i] > rmax) rmax = data[i];
    }

    /* 2. 计算量化参数 */
    qparams->scheme = QUANT_INT8_SYMMETRIC;
    qparams->scale = int8_calc_scale(rmin, rmax);
    qparams->zero_point = 0;  /* 对称量化 */
    qparams->qmin = -128;
    qparams->qmax = 127;
}

/**
 * @brief 动态量化线性层
 * @param input 输入 (FP32)
 * @param weight 权重 (INT8, 已量化)
 * @param bias 偏置 (FP32)
 * @param output 输出 (FP32)
 * @param in_features 输入特征数
 * @param out_features 输出特征数
 * @param weight_scale 权重缩放因子
 */
void dynamic_quantized_linear(const float* input,
                               const int8_t* weight,
                               const float* bias,
                               float* output,
                               uint32_t in_features,
                               uint32_t out_features,
                               float weight_scale)
{
    /* 1. 动态量化输入 */
    quantization_params_t input_qparams;
    dynamic_calc_quant_params(input, in_features, &input_qparams);

    /* 分配临时INT8缓冲区 */
    int8_t* quantized_input = (int8_t*)malloc(in_features);
    if (!quantized_input) return;

    quantize_fp32_to_int8(input, quantized_input, in_features,
                          input_qparams.scale);

    /* 2. INT8矩阵乘法 */
    for (uint32_t o = 0; o < out_features; o++) {
        int32_t acc = 0;

        for (uint32_t i = 0; i < in_features; i++) {
            acc += (int32_t)quantized_input[i] *
                   (int32_t)weight[o * in_features + i];
        }

        /* 3. 反量化并加偏置 */
        float dequantized = (float)acc * input_qparams.scale * weight_scale;
        output[o] = dequantized + (bias ? bias[o] : 0.0f);
    }

    free(quantized_input);
}

/* ========== 逐通道量化 ========== */

typedef struct {
    float* scales;            /* 每个通道的scale */
    uint32_t num_channels;
} per_channel_quant_params_t;

/**
 * @brief 逐通道量化卷积核
 * 卷积核: [C_out x K_h x K_w x C_in]
 * 每个输出通道使用独立的scale
 */
void quantize_conv_kernel_per_channel(const float* kernel_fp32,
                                       int8_t* kernel_int8,
                                       per_channel_quant_params_t* pc_params,
                                       uint32_t C_out,
                                       uint32_t K_h,
                                       uint32_t K_w,
                                       uint32_t C_in)
{
    uint32_t kernel_size_per_channel = K_h * K_w * C_in;

    for (uint32_t co = 0; co < C_out; co++) {
        const float* channel_kernel = kernel_fp32 + co * kernel_size_per_channel;
        int8_t* channel_quant = kernel_int8 + co * kernel_size_per_channel;

        /* 计算该通道的min/max */
        float rmin = FLT_MAX, rmax = -FLT_MAX;
        for (uint32_t i = 0; i < kernel_size_per_channel; i++) {
            if (channel_kernel[i] < rmin) rmin = channel_kernel[i];
            if (channel_kernel[i] > rmax) rmax = channel_kernel[i];
        }

        /* 计算scale并量化 */
        pc_params->scales[co] = int8_calc_scale(rmin, rmax);
        quantize_fp32_to_int8(channel_kernel, channel_quant,
                              kernel_size_per_channel,
                              pc_params->scales[co]);
    }
}
