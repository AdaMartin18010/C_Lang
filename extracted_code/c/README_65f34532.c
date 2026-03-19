/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\README.md
 * Line: 293
 * Language: c
 * Block ID: 65f34532
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// STDP (Spike-Timing-Dependent Plasticity)
typedef struct {
    float weight;
    float a_plus;      // 增强系数
    float a_minus;     // 抑制系数
    float tau_plus;    // 增强时间常数
    float tau_minus;   // 抑制时间常数
    float w_max;       // 最大权重
    float w_min;       // 最小权重
} STDPSynapse;

void apply_stdp(STDPSynapse *syn,
                int64_t pre_time, int64_t post_time) {
    int64_t dt = post_time - pre_time;
    float dw;

    if (dt > 0) {
        // 后神经元在预神经元之后发放 - 增强
        dw = syn->a_plus * exp(-dt / syn->tau_plus);
    } else {
        // 抑制
        dw = -syn->a_minus * exp(dt / syn->tau_minus);
    }

    syn->weight += dw;

    // 边界约束
    if (syn->weight > syn->w_max) syn->weight = syn->w_max;
    if (syn->weight < syn->w_min) syn->weight = syn->w_min;
}
