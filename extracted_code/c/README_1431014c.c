/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\README.md
 * Line: 44
 * Language: c
 * Block ID: 1431014c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 事件驱动神经元模型
typedef struct {
    float membrane_potential;
    float threshold;
    float leak_rate;
    uint64_t last_spike_time;
    int refractory_period;
} Neuron;

// 处理输入事件
void process_event(Neuron *neuron, float input_current,
                   uint64_t current_time) {
    // 检查不应期
    if (current_time - neuron->last_spike_time <
        neuron->refractory_period) {
        return;
    }

    // 膜电位衰减（漏电流）
    uint64_t dt = current_time - neuron->last_spike_time;
    neuron->membrane_potential *= exp(-dt * neuron->leak_rate);

    // 累加输入电流
    neuron->membrane_potential += input_current;

    // 检查是否触发脉冲
    if (neuron->membrane_potential >= neuron->threshold) {
        // 发射脉冲
        emit_spike(neuron);
        neuron->membrane_potential = 0.0f;
        neuron->last_spike_time = current_time;
    }
}
