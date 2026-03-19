/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\README.md
 * Line: 375
 * Language: c
 * Block ID: 1ab4c842
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 定点数神经元（适合 MCU）
typedef struct {
    int16_t v;         // Q8.8 定点数
    int16_t v_th;
    int16_t leak;      // 衰减系数
    int16_t i;
} FixedLIFNeuron;

// 定点数更新
void update_fixed_lif(FixedLIFNeuron *n) {
    // v = v - (v >> leak_bits) + i
    n->v -= (n->v >> 4);  // 假设 leak_bits = 4
    n->v += n->i;

    if (n->v >= n->v_th) {
        n->v = 0;
        // 触发脉冲
    }
}
