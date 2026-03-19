/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\01_SNN_Control.md
 * Line: 74
 * Language: c
 * Block ID: 7a8c5f74
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// Leaky Integrate-and-Fire (LIF) 神经元模型
// 事件驱动实现
// ============================================================================

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

// 神经元参数
typedef struct {
    float v_rest;       // 静息电位 (mV)
    float v_reset;      // 复位电位 (mV)
    float v_thresh;     // 阈值电位 (mV)
    float tau_m;        // 膜时间常数 (ms)
    float r_mem;        // 膜电阻 (MΩ)
    float tau_ref;      // 不应期 (ms)
} LIFParams;

// 默认参数
const LIFParams LIF_DEFAULT = {
    .v_rest = -70.0f,
    .v_reset = -70.0f,
    .v_thresh = -55.0f,
    .tau_m = 20.0f,
    .r_mem = 1.0f,
    .tau_ref = 2.0f
};

// 神经元状态
typedef struct {
    float v_mem;        // 膜电位 (mV)
    float i_syn;        // 突触电流 (nA)
    float ref_count;    // 不应期计数 (ms)
    uint64_t last_spike_time;  // 上次脉冲时间 (us)
    uint32_t spike_count;      // 脉冲计数
    bool is_refactory;         // 是否处于不应期
} LIFNeuron;

// 初始化神经元
void lif_init(LIFNeuron *neuron, const LIFParams *params) {
    neuron->v_mem = params->v_rest;
    neuron->i_syn = 0.0f;
    neuron->ref_count = 0.0f;
    neuron->last_spike_time = 0;
    neuron->spike_count = 0;
    neuron->is_refactory = false;
}

// 突触输入 (电流注入)
void lif_input_current(LIFNeuron *neuron, float i_input) {
    neuron->i_syn += i_input;
}

// 突触输入 (脉冲事件)
void lif_input_spike(LIFNeuron *neuron, float weight) {
    // 转换为电流
    neuron->i_syn += weight;
}

// 神经元更新 (固定时间步长)
bool lif_update(LIFNeuron *neuron, const LIFParams *params, float dt_ms) {
    bool spiked = false;

    // 不应期处理
    if (neuron->is_refactory) {
        neuron->ref_count -= dt_ms;
        if (neuron->ref_count <= 0.0f) {
            neuron->is_refactory = false;
            neuron->v_mem = params->v_reset;
        }
        return false;  // 不应期内不发放
    }

    // 泄漏积分
    float dv = (-(neuron->v_mem - params->v_rest) +
                params->r_mem * neuron->i_syn) / params->tau_m;
    neuron->v_mem += dv * dt_ms;

    // 突触电流衰减
    neuron->i_syn *= expf(-dt_ms / params->tau_m);

    // 检查脉冲发放
    if (neuron->v_mem >= params->v_thresh) {
        spiked = true;
        neuron->spike_count++;
        neuron->is_refactory = true;
        neuron->ref_count = params->tau_ref;
        // 膜电位保持或复位在不应期内处理
    }

    return spiked;
}

// 事件驱动更新 (精确时间)
float lif_next_event_time(const LIFNeuron *neuron, const LIFParams *params) {
    if (neuron->i_syn <= 0.0f) {
        return INFINITY;  // 不会发放
    }

    // 解析求解脉冲时间
    // tau_m * dv/dt = -(v - v_rest) + R * I
    // 解: v(t) = v_rest + R*I + (v0 - v_rest - R*I) * exp(-t/tau_m)
    // 当v(t) = v_thresh时:
    float v_inf = params->v_rest + params->r_mem * neuron->i_syn;

    if (v_inf <= params->v_thresh) {
        return INFINITY;  // 稳态不会达到阈值
    }

    float delta_v = v_inf - neuron->v_mem;
    float target_delta = v_inf - params->v_thresh;

    float t_spike = -params->tau_m * logf(target_delta / delta_v);

    return t_spike;
}
