/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\README.md
 * Line: 328
 * Language: c
 * Block ID: a60ac98f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 基于事件的 SNN 仿真器
typedef struct {
    int num_neurons;
    LIFNeuron *neurons;
    float **weights;

    // 事件队列
    SpikeEvent *event_queue;
    int queue_head;
    int queue_tail;
} SNN;

void simulate_step(SNN *net, float dt) {
    // 处理当前时刻的所有事件
    while (net->queue_head < net->queue_tail) {
        SpikeEvent *evt = &net->event_queue[net->queue_head];
        // 传播到连接的目标神经元
        for (int i = 0; i < net->num_neurons; i++) {
            if (net->weights[evt->neuron_id][i] > 0) {
                net->neurons[i].i += evt->amplitude *
                                     net->weights[evt->neuron_id][i];
            }
        }
        net->queue_head++;
    }

    // 更新所有神经元
    for (int i = 0; i < net->num_neurons; i++) {
        update_lif(&net->neurons[i], dt);
        if (net->neurons[i].spiked) {
            // 生成新事件
            SpikeEvent new_evt = {
                .timestamp = current_time,
                .neuron_id = i,
                .amplitude = 1.0f
            };
            net->event_queue[net->queue_tail++] = new_evt;
        }
    }
}
