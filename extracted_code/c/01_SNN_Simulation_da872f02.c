/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\01_SNN_Simulation.md
 * Line: 96
 * Language: c
 * Block ID: da872f02
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Spike-Timing Dependent Plasticity
// 突触前脉冲先于突触后: 长时程增强 (LTP)
// 突触后脉冲先于突触前: 长时程抑制 (LTD)

typedef struct {
    float w;            // 突触权重
    float w_max, w_min; // 权重边界
    float a_plus;       // LTP学习率
    float a_minus;      // LTD学习率
    float tau_plus;     // LTP时间常数
    float tau_minus;    // LTD时间常数
} stdp_synapse_t;

// STDP窗口函数
float stdp_window(float delta_t) {
    // delta_t = t_post - t_pre
    if (delta_t > 0) {
        // LTP
        return A_PLUS * expf(-delta_t / TAU_PLUS);
    } else {
        // LTD
        return -A_MINUS * expf(delta_t / TAU_MINUS);
    }
}

void stdp_update(stdp_synapse_t* syn, float delta_t) {
    float dw = stdp_window(delta_t);
    syn->w += dw;
    syn->w = fmaxf(fminf(syn->w, syn->w_max), syn->w_min);
}
