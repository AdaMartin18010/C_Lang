/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\01_SNN_Simulation.md
 * Line: 26
 * Language: c
 * Block ID: 3c6fc853
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// LIF神经元参数
typedef struct {
    float v;            // 膜电位 (mV)
    float v_rest;       // 静息电位 (-70mV)
    float v_thresh;     // 阈值 (-55mV)
    float v_reset;      // 重置电位 (-75mV)
    float tau_m;        // 膜时间常数 (20ms)
    float r;            // 膜电阻 (10MΩ)
    float i_offset;     // 背景电流
    float refractory;   // 不应期 (2ms)
    float refractory_countdown; // 不应期倒计时
} lif_neuron_t;

// LIF神经元更新
// tau_m * dv/dt = -(v - v_rest) + R * I
void lif_update(lif_neuron_t* n, float i_syn, float dt) {
    if (n->refractory_countdown > 0) {
        n->refractory_countdown -= dt;
        n->v = n->v_reset;
        return;
    }

    // 欧拉法积分
    float dv = (-(n->v - n->v_rest) + n->r * (i_syn + n->i_offset)) / n->tau_m;
    n->v += dv * dt;

    // 脉冲产生
    if (n->v >= n->v_thresh) {
        n->v = n->v_reset;
        n->refractory_countdown = n->refractory;
        // 发送脉冲...
    }
}
