/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\README.md
 * Line: 217
 * Language: c
 * Block ID: 836ad60d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// LIF 神经元仿真
typedef struct {
    float v;           // 膜电位
    float v_rest;      // 静息电位
    float v_reset;     // 重置电位
    float v_th;        // 阈值电位
    float tau_m;       // 膜时间常数
    float r;           // 膜电阻
    float i;           // 输入电流
    int spiked;        // 是否发放脉冲
} LIFNeuron;

void update_lif(LIFNeuron *n, float dt) {
    // 欧拉法求解微分方程
    // dv/dt = (v_rest - v + r*i) / tau_m
    float dv = (n->v_rest - n->v + n->r * n->i) / n->tau_m;
    n->v += dv * dt;

    // 检查脉冲
    if (n->v >= n->v_th) {
        n->spiked = 1;
        n->v = n->v_reset;
    } else {
        n->spiked = 0;
    }
}
