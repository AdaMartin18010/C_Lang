/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\02_STDP_Learning.md
 * Line: 520
 * Language: c
 * Block ID: ed40b8e1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 问题: 权重持续单向漂移，导致饱和
// LTP和LTD不平衡

// ✅ 解决: 平衡A_plus和A_minus
STDPParams balanced = {
    .A_plus = 0.01f,
    .A_minus = -0.009f,  // 略小，防止过度抑制
    // ...
};

// 或添加突触缩放
void synaptic_scaling(STDPNetwork *net, float target_rate) {
    for (int n = 0; n < net->num_neurons; n++) {
        float actual_rate = net->neurons[n].spike_count / simulation_time;
        float scale_factor = target_rate / actual_rate;

        for (int s = 0; s < net->neurons[n].num_input_syn; s++) {
            int syn_idx = net->neurons[n].input_synapses[s];
            net->synapses[syn_idx].weight *= scale_factor;
        }
    }
}
