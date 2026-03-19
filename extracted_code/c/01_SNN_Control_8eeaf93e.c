/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\01_SNN_Control.md
 * Line: 196
 * Language: c
 * Block ID: 8eeaf93e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 突触模型 - 电流型和电导型
// ============================================================================

// 突触类型
typedef enum {
    SYNAPSE_CURRENT,    // 电流型 (简单)
    SYNAPSE_EXP,        // 指数衰减型
    SYNAPSE_ALPHA,      // Alpha函数型
    SYNAPSE_NMDA        // NMDA型 (电压依赖)
} SynapseType;

// 突触参数
typedef struct {
    SynapseType type;
    float weight;       // 突触权重
    float delay;        // 突触延迟 (ms)
    float tau_syn;      // 突触时间常数 (ms)
} SynapseParams;

// 突触状态
typedef struct {
    float g;            // 电导 (nS)
    float i;            // 电流 (nA)
    uint64_t last_pre_spike;
    uint64_t last_post_spike;
} SynapseState;

// 指数衰减突触更新
void synapse_exp_update(SynapseState *syn, const SynapseParams *params,
                        float dt_ms, float v_post) {
    // 指数衰减
    syn->g *= expf(-dt_ms / params->tau_syn);

    // 计算电流
    syn->i = syn->g * v_post;
}

// Alpha函数突触
void synapse_alpha_spike(SynapseState *syn, const SynapseParams *params) {
    // Alpha函数: g(t) = (t/tau) * exp(1 - t/tau)
    // 峰值在 t = tau 时，g_max = weight
    syn->g += params->weight;
}

// NMDA突触 (电压依赖)
void synapse_nmda_update(SynapseState *syn, const SynapseParams *params,
                         float dt_ms, float v_post) {
    // NMDA电导的镁阻断
    float mg_block = 1.0f / (1.0f + 0.28f * expf(-0.062f * v_post) * (v_post / 3.57f));

    syn->g *= expf(-dt_ms / params->tau_syn);
    syn->i = syn->g * v_post * mg_block;
}
