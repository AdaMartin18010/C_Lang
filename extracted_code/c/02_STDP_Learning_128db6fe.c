/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\02_STDP_Learning.md
 * Line: 153
 * Language: c
 * Block ID: 128db6fe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 基于轨迹的STDP实现 (高效)
// ============================================================================

// 突触轨迹状态
typedef struct {
    float a_pre;            // 前突触活动轨迹
    float a_post;           // 后突触活动轨迹
    uint64_t last_pre_time; // 上次前脉冲时间 (us)
    uint64_t last_post_time;// 上次后脉冲时间 (us)
} STDPTrajectory;

// 带STDP的突触
typedef struct {
    float weight;
    float delay;
    STDPTrajectory traj;
    STDPParams params;
} STDPSynapse;

// 初始化STDP突触
void stdp_synapse_init(STDPSynapse *syn, float initial_weight,
                       const STDPParams *params) {
    syn->weight = initial_weight;
    syn->delay = 1.0f;
    syn->traj.a_pre = 0.0f;
    syn->traj.a_post = 0.0f;
    syn->traj.last_pre_time = 0;
    syn->traj.last_post_time = 0;
    syn->params = *params;
}

// 前突触脉冲到达
void stdp_pre_spike(STDPSynapse *syn, uint64_t current_time_us) {
    // 1. 更新前突触轨迹
    float dt_ms = (current_time_us - syn->traj.last_pre_time) / 1000.0f;
    if (syn->traj.last_pre_time > 0) {
        syn->traj.a_pre *= expf(-dt_ms / syn->params.tau_plus);
    }
    syn->traj.a_pre += 1.0f;  // 脉冲贡献
    syn->traj.last_pre_time = current_time_us;

    // 2. 基于后突触轨迹更新权重 (LTD)
    // 如果后神经元最近发放过，则抑制
    if (syn->traj.last_post_time > 0) {
        float dt_post_ms = (current_time_us - syn->traj.last_post_time) / 1000.0f;
        if (dt_post_ms < 5.0f * syn->params.tau_minus) {
            float dw = syn->params.A_minus * syn->traj.a_post;

            // 权重依赖
            if (syn->params.weight_dependence == 1) {
                // 乘性: 接近w_min时变化小
                dw *= (syn->weight - syn->params.w_min);
            } else if (syn->params.weight_dependence == 2) {
                // 幂律
                dw *= powf(syn->weight, syn->params.mu);
            }

            syn->weight += syn->params.learning_rate * dw;
        }
    }
}

// 后突触脉冲到达
void stdp_post_spike(STDPSynapse *syn, uint64_t current_time_us) {
    // 1. 更新后突触轨迹
    float dt_ms = (current_time_us - syn->traj.last_post_time) / 1000.0f;
    if (syn->traj.last_post_time > 0) {
        syn->traj.a_post *= expf(-dt_ms / syn->params.tau_minus);
    }
    syn->traj.a_post += 1.0f;
    syn->traj.last_post_time = current_time_us;

    // 2. 基于前突触轨迹更新权重 (LTP)
    if (syn->traj.last_pre_time > 0) {
        float dt_pre_ms = (current_time_us - syn->traj.last_pre_time) / 1000.0f;
        if (dt_pre_ms < 5.0f * syn->params.tau_plus) {
            float dw = syn->params.A_plus * syn->traj.a_pre;

            // 权重依赖
            if (syn->params.weight_dependence == 1) {
                // 乘性: 接近w_max时变化小
                dw *= (syn->params.w_max - syn->weight);
            } else if (syn->params.weight_dependence == 2) {
                dw *= powf(syn->params.w_max - syn->weight, syn->params.mu);
            }

            syn->weight += syn->params.learning_rate * dw;
        }
    }

    // 3. 限制权重范围
    if (syn->weight > syn->params.w_max) syn->weight = syn->params.w_max;
    if (syn->weight < syn->params.w_min) syn->weight = syn->params.w_min;
}

// 轨迹衰减 (定期调用)
void stdp_decay_trajectory(STDPSynapse *syn, float dt_ms) {
    syn->traj.a_pre *= expf(-dt_ms / syn->params.tau_plus);
    syn->traj.a_post *= expf(-dt_ms / syn->params.tau_minus);
}
