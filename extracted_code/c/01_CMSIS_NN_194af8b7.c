/*
 * Auto-generated from: 16_Embedded_AI_Agents\06_C_Frameworks\01_CMSIS_NN.md
 * Line: 280
 * Language: c
 * Block ID: 194af8b7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 全连接层 (Fully Connected / Dense)
typedef struct {
    q7_t* weights;           // [output_units][input_units]
    q7_t* bias;              // [output_units]
    uint16_t input_offset;
    uint16_t output_offset;
} fc_layer_ctx_t;

int fc_layer_inference(const fc_layer_ctx_t* ctx,
                      const q7_t* input,
                      q7_t* output,
                      uint16_t input_len,
                      uint16_t output_len,
                      int32_t quant_multiplier,
                      int32_t quant_shift) {

    cmsis_nn_fc_params fc_params;
    cmsis_nn_per_tensor_quant_params quant_params;
    cmsis_nn_dims input_dims, filter_dims, bias_dims, output_dims;

    fc_params.input_offset = ctx->input_offset;
    fc_params.output_offset = ctx->output_offset;
    fc_params.filter_offset = 0;
    fc_params.activation.min = -128;
    fc_params.activation.max = 127;

    input_dims.n = 1;
    input_dims.h = 1;
    input_dims.w = 1;
    input_dims.c = input_len;

    filter_dims.n = output_len;
    filter_dims.h = 1;
    filter_dims.w = 1;
    filter_dims.c = input_len;

    bias_dims.n = output_len;

    output_dims.n = 1;
    output_dims.h = 1;
    output_dims.w = 1;
    output_dims.c = output_len;

    quant_params.multiplier = quant_multiplier;
    quant_params.shift = quant_shift;

    // 使用优化的全连接函数
    arm_status status = arm_fully_connected_s8(
        &fc_params,
        &quant_params,
        &input_dims,
        input,
        &filter_dims,
        ctx->weights,
        &bias_dims,
        ctx->bias,
        &output_dims,
        output
    );

    return (status == ARM_MATH_SUCCESS) ? 0 : -1;
}
