/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\README.md
 * Line: 84
 * Language: c
 * Block ID: 62d81f69
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 脉冲编码示例
typedef struct {
    uint64_t timestamp;
    int neuron_id;
    float amplitude;
} SpikeEvent;

// 率编码转脉冲序列
void rate_to_spikes(float rate, int duration_ms,
                    SpikeEvent *spikes, int *num_spikes) {
    float lambda = rate / 1000.0f; // 转换为每毫秒
    *num_spikes = 0;

    for (int t = 0; t < duration_ms; t++) {
        // 泊松过程生成脉冲
        float p = (float)rand() / RAND_MAX;
        if (p < lambda) {
            spikes[*num_spikes].timestamp = t;
            spikes[*num_spikes].neuron_id = 0;
            spikes[*num_spikes].amplitude = 1.0f;
            (*num_spikes)++;
        }
    }
}
