/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\02_STDP_Learning.md
 * Line: 72
 * Language: c
 * Block ID: b5c37347
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// STDP (Spike-Timing-Dependent Plasticity) 实现
// 生物启发的突触可塑性规则
// ============================================================================

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

// STDP参数
typedef struct {
    // 增强窗口 (pre before post)
    float A_plus;           // 最大增强量
    float tau_plus;         // 增强时间常数 (ms)

    // 抑制窗口 (post before pre)
    float A_minus;          // 最大抑制量
    float tau_minus;        // 抑制时间常数 (ms)

    // 权重约束
    float w_min;            // 最小权重
    float w_max;            // 最大权重

    // 学习率
    float learning_rate;

    // 权重依赖类型
    // 0 = 加性, 1 = 乘性, 2 = 幂律
    int weight_dependence;
    float mu;               // 幂律指数 (0=加性, 1=乘性)
} STDPParams;

// 经典STDP参数 (Bi & Poo 1998)
const STDPParams STDP_CLASSIC = {
    .A_plus = 0.01f,
    .tau_plus = 20.0f,
    .A_minus = -0.0105f,
    .tau_minus = 20.0f,
    .w_min = 0.0f,
    .w_max = 1.0f,
    .learning_rate = 1.0f,
    .weight_dependence = 0,
    .mu = 0.0f
};

// 对称STDP参数
const STDPParams STDP_SYMMETRIC = {
    .A_plus = 0.01f,
    .tau_plus = 20.0f,
    .A_minus = -0.01f,
    .tau_minus = 20.0f,
    .w_min = 0.0f,
    .w_max = 1.0f,
    .learning_rate = 1.0f,
    .weight_dependence = 1,
    .mu = 0.5f
};

// 计算STDP窗口函数
delta_w = {
    A_plus  * exp(-delta_t / tau_plus)   if delta_t > 0 (pre before post)
    A_minus * exp( delta_t / tau_minus)  if delta_t < 0 (post before pre)
}

// STDP权重变化计算
float stdp_weight_change(float delta_t_ms, const STDPParams *params) {
    if (delta_t_ms > 0.0f) {
        // pre在post之前: 增强 (LTP)
        return params->A_plus * expf(-delta_t_ms / params->tau_plus);
    } else if (delta_t_ms < 0.0f) {
        // post在pre之前: 抑制 (LTD)
        return params->A_minus * expf(delta_t_ms / params->tau_minus);
    }
    return 0.0f;
}
