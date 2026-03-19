/*
 * Auto-generated from: 16_Embedded_AI_Agents\09_Security_Reliability\01_AI_Safety_Standards.md
 * Line: 314
 * Language: c
 * Block ID: 0f3fe50b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// nn_formal_verification.h
// 神经网络的轻量级形式化验证

#ifndef NN_FORMAL_VERIFICATION_H
#define NN_FORMAL_VERIFICATION_H

#include <stdint.h>

// 区间抽象 (Interval Abstract Domain)
typedef struct {
    float lower;
    float upper;
} interval_t;

// 验证属性
typedef enum {
    PROP_LOCAL_ROBUSTNESS = 0,  // 局部鲁棒性
    PROP_OUTPUT_RANGE,          // 输出范围
    PROP_MONOTONICITY,          // 单调性
    PROP_LIPSCHITZ_BOUND        // Lipschitz约束
} verification_property_t;

// 验证结果
typedef struct {
    bool verified;
    float confidence;
    char counter_example[256];  // 反例 (如果验证失败)
} verification_result_t;

// 区间传播验证
verification_result_t verify_interval_propagation(
    const float* network_weights,
    const interval_t* input_intervals,
    int num_inputs,
    interval_t* output_intervals,
    int num_outputs
);

// 局部鲁棒性验证 (给定输入邻域内输出不变)
verification_result_t verify_local_robustness(
    const float* input,
    int input_size,
    float epsilon,              // L-infinity球半径
    int expected_class,
    float (*network_inference)(const float*, int)
);

#endif
