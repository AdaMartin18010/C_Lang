/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\README.md
 * Line: 248
 * Language: c
 * Block ID: 2cf1ae63
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Izhikevich 神经元模型
typedef struct {
    float v;           // 膜电位
    float u;           // 恢复变量
    float a, b, c, d;  // 模型参数
    int spiked;
} IzhikevichNeuron;

// 预定义神经元类型
void set_regular_spiking(IzhikevichNeuron *n) {
    n->a = 0.02f;
    n->b = 0.2f;
    n->c = -65.0f;
    n->d = 8.0f;
}

void set_intrinsically_bursting(IzhikevichNeuron *n) {
    n->a = 0.02f;
    n->b = 0.2f;
    n->c = -55.0f;
    n->d = 4.0f;
}

void update_izhikevich(IzhikevichNeuron *n, float i, float dt) {
    if (n->v >= 30.0f) {
        n->v = n->c;
        n->u += n->d;
        n->spiked = 1;
    } else {
        // dv/dt = 0.04v^2 + 5v + 140 - u + i
        float dv = 0.04f * n->v * n->v + 5.0f * n->v + 140.0f
                   - n->u + i;
        // du/dt = a(bv - u)
        float du = n->a * (n->b * n->v - n->u);

        n->v += dv * dt;
        n->u += du * dt;
        n->spiked = 0;
    }
}
