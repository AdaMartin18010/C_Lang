/*
 * Auto-generated from: 16_Embedded_AI_Agents\06_C_Frameworks\01_CMSIS_NN.md
 * Line: 169
 * Language: c
 * Block ID: 1dd0a44d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 示例: 实现一个量化卷积层
#include "arm_nnfunctions.h"

// 卷积层上下文
typedef struct {
    q7_t* weights;           // 量化权重 [out_channels][kernel_h][kernel_w][in_channels]
    q7_t* bias;              // 量化偏置 [out_channels]
    q15_t* buffer_a;         // 临时缓冲区
    uint16_t input_offset;   // 输入零点
    uint16_t output_offset;  // 输出零点
    int32_t out_activation_min;
    int32_t out_activation_max;
} conv_layer_ctx_t;

// 初始化卷积层
int conv_layer_init(conv_layer_ctx_t* ctx,
                   const q7_t* weights,
                   const q7_t* bias,
                   uint16_t input_offset,
                   uint16_t output_offset) {
    ctx->weights = weights;
    ctx->bias = bias;
    ctx->input_offset = input_offset;
    ctx->output_offset = output_offset;
    ctx->out_activation_min = -128;
    ctx->out_activation_max = 127;

    // 分配临时缓冲区 (用于Im2Col转换)
    ctx->buffer_a = (q15_t*)malloc(2 * 3 * 3 * 3 * sizeof(q15_t));
    if (!ctx->buffer_a) return -1;

    return 0;
}

// 执行卷积推理
int conv_layer_inference(conv_layer_ctx_t* ctx,
                        const q7_t* input,
                        q7_t* output,
                        const uint16_t input_dims[4],
                        const uint16_t filter_dims[4],
                        const uint16_t bias_dims[1],
                        const uint16_t output_dims[4],
                        const uint16_t strides[2],
                        const uint16_t padding[4],
                        const int32_t quant_params[2]) {

    cmsis_nn_conv_params conv_params;
    cmsis_nn_per_channel_quant_params per_channel_quant_params;
    cmsis_nn_dims input_dims_cmsis, filter_dims_cmsis, bias_dims_cmsis, output_dims_cmsis;

    // 填充参数结构
    conv_params.input_offset = ctx->input_offset;
    conv_params.output_offset = ctx->output_offset;
    conv_params.stride.h = strides[0];
    conv_params.stride.w = strides[1];
    conv_params.padding.h = padding[0];
    conv_params.padding.w = padding[2];
    conv_params.activation.min = ctx->out_activation_min;
    conv_params.activation.max = ctx->out_activation_max;

    // 维度信息
    input_dims_cmsis.n = input_dims[0];
    input_dims_cmsis.h = input_dims[1];
    input_dims_cmsis.w = input_dims[2];
    input_dims_cmsis.c = input_dims[3];

    filter_dims_cmsis.n = filter_dims[0];  // output channels
    filter_dims_cmsis.h = filter_dims[1];  // kernel height
    filter_dims_cmsis.w = filter_dims[2];  // kernel width
    filter_dims_cmsis.c = filter_dims[3];  // input channels

    bias_dims_cmsis.n = bias_dims[0];

    output_dims_cmsis.n = output_dims[0];
    output_dims_cmsis.h = output_dims[1];
    output_dims_cmsis.w = output_dims[2];
    output_dims_cmsis.c = output_dims[3];

    // 量化参数 (每个输出通道)
    per_channel_quant_params.multiplier = quant_params;
    per_channel_quant_params.shift = quant_params + output_dims[3];

    // 调用CMSIS-NN卷积函数
    arm_status status = arm_convolve_s8(
        &conv_params,
        &per_channel_quant_params,
        &input_dims_cmsis,
        input,
        &filter_dims_cmsis,
        ctx->weights,
        &bias_dims_cmsis,
        ctx->bias,
        &output_dims_cmsis,
        output
    );

    return (status == ARM_MATH_SUCCESS) ? 0 : -1;
}

// 清理资源
void conv_layer_deinit(conv_layer_ctx_t* ctx) {
    if (ctx->buffer_a) {
        free(ctx->buffer_a);
        ctx->buffer_a = NULL;
    }
}
