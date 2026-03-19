/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\01_SNN_Simulation.md
 * Line: 64
 * Language: c
 * Block ID: 832969b5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Izhikevich神经元 (能重现多种放电模式)
typedef struct {
    float v;            // 膜电位
    float u;            // 恢复变量
    float a, b, c, d;   // 模型参数
} izhikevich_neuron_t;

// 参数设置示例
// 常规放电: a=0.02, b=0.2, c=-65, d=8
// 快速放电: a=0.1,  b=0.2, c=-65, d=2
// 爆发放电: a=0.02, b=0.2, c=-50, d=2

void izhikevich_update(izhikevich_neuron_t* n, float i, float dt) {
    // dv/dt = 0.04*v^2 + 5*v + 140 - u + I
    // du/dt = a*(b*v - u)

    float v = n->v;
    float u = n->u;

    n->v += (0.04f * v * v + 5.0f * v + 140.0f - u + i) * dt;
    n->u += n->a * (n->b * v - u) * dt;

    if (n->v >= 30.0f) {
        n->v = n->c;
        n->u += n->d;
    }
}
