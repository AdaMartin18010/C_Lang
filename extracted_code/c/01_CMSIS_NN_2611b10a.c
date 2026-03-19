/*
 * Auto-generated from: 16_Embedded_AI_Agents\06_C_Frameworks\01_CMSIS_NN.md
 * Line: 148
 * Language: c
 * Block ID: 2611b10a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// CMSIS-NN核心数据结构
#include "arm_nnfunctions.h"
#include "arm_nn_types.h"

// 张量信息结构
arm_nn_tensor_info input_info = {
    .num_dims = 4,
    .shape = {1, 32, 32, 3},      // [batch, height, width, channels]
    .data_type = ARM_NN_TYPE_INT8,  // 8位量化
};

// 量化参数
arm_nn_quant_params quant_params = {
    .shift = {-3},                  // 移位量
    .multiplier = {1877991215},     // 乘法因子 (Q31格式)
};
