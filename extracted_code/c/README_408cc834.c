/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\README.md
 * Line: 117
 * Language: c
 * Block ID: 408cc834
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Loihi 神经元配置示例
typedef struct {
    // 膜电位参数
    int v_decay;           // 膜电位衰减
    int v_th_mant;         // 阈值尾数
    int v_th_exp;          // 阈值指数

    // 脉冲延迟
    int ds_offset;         // 树突延迟偏移
    int dm_offset;         // 轴突延迟偏移

    // 不应期
    int refract_delay;

    // 突触连接
    int num_axons;
    int num_synapses;
} LoihiNeuronConfig;

// 配置 LIF 神经元
void configure_lif_neuron(LoihiNeuronConfig *cfg) {
    cfg->v_decay = 128;      // 衰减因子
    cfg->v_th_mant = 100;    // 阈值 = 100 << 6
    cfg->v_th_exp = 6;
    cfg->refract_delay = 2;
}
