/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\README.md
 * Line: 438
 * Language: c
 * Block ID: 7d60f3a0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 基于 DVS（动态视觉传感器）的手势识别
void process_dvs_event(int x, int y, int polarity,
                       uint64_t timestamp) {
    // 映射到输入层
    int input_id = y * DVS_WIDTH + x;

    // 生成脉冲事件
    SpikeEvent evt = {
        .timestamp = timestamp,
        .neuron_id = input_id,
        .amplitude = polarity ? 1.0f : -1.0f
    };

    inject_event(&snn, evt);
}

// 读出类
int classify_gesture(SNN *net) {
    int spike_counts[OUTPUT_CLASSES] = {0};

    // 统计各类输出神经元的脉冲数
    for (int i = 0; i < OUTPUT_CLASSES; i++) {
        spike_counts[i] = count_spikes(&net->neurons[OUTPUT_START + i]);
    }

    // 选择发放最多的类别
    int max_class = 0;
    for (int i = 1; i < OUTPUT_CLASSES; i++) {
        if (spike_counts[i] > spike_counts[max_class]) {
            max_class = i;
        }
    }
    return max_class;
}
