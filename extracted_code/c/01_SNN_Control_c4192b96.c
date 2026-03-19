/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\01_SNN_Control.md
 * Line: 255
 * Language: c
 * Block ID: c4192b96
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 脉冲编码方案
// ============================================================================

// 速率编码: 将模拟值转换为脉冲频率
float rate_encode(float value, float min_val, float max_val,
                  float min_rate, float max_rate) {
    // 归一化
    float normalized = (value - min_val) / (max_val - min_val);
    if (normalized < 0.0f) normalized = 0.0f;
    if (normalized > 1.0f) normalized = 1.0f;

    // 线性映射到频率
    return min_rate + normalized * (max_rate - min_rate);
}

// 时间编码: 首次脉冲时间编码
float time_encode_first_spike(float value, float min_val, float max_val,
                               float t_min, float t_max) {
    float normalized = (value - min_val) / (max_val - min_val);

    // 值越大，脉冲越早
    return t_max - normalized * (t_max - t_min);
}

// 群体编码: 多个神经元的活动模式
void population_encode(float value, float *neuron_rates, int n_neurons,
                       float min_val, float max_val) {
    float center = (min_val + max_val) / 2.0f;
    float width = (max_val - min_val) / n_neurons;

    for (int i = 0; i < n_neurons; i++) {
        float neuron_center = min_val + (i + 0.5f) * width;
        float distance = fabsf(value - neuron_center);

        // 高斯调谐曲线
        neuron_rates[i] = expf(-(distance * distance) / (2.0f * width * width));
    }
}

// 解码脉冲计数 (速率解码)
float rate_decode(const uint32_t *spike_counts, int n_neurons, float dt_ms) {
    float total = 0.0f;
    float weighted_sum = 0.0f;

    for (int i = 0; i < n_neurons; i++) {
        float rate = spike_counts[i] / (dt_ms / 1000.0f);  // Hz
        total += rate;
        weighted_sum += rate * i;
    }

    if (total < 1.0f) return 0.0f;
    return weighted_sum / total;
}
