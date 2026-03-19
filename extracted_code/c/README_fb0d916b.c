/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Cryogenic_Superconducting\README.md
 * Line: 317
 * Language: c
 * Block ID: fb0d916b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 量子比特控制脉冲参数
typedef struct {
    float frequency;         // 驱动频率 (GHz)
    float amplitude;         // 脉冲幅度
    float duration;          // 脉冲宽度 (ns)
    float phase;             // 相位
    int pulse_shape;         // 脉冲形状：高斯、DRAG 等
} QubitPulse;

// 量子比特控制板接口
typedef struct {
    int num_qubits;
    float sample_rate;       // DAC 采样率
    int resolution;          // DAC 分辨率
    float output_range;      // 输出电压范围
    float line_delay;        // 线缆延迟
} QubitControlBoard;

// 脉冲序列生成
void generate_pulse_sequence(QubitPulse *pulses, int num_pulses,
                             float *dac_samples, int num_samples) {
    float dt = 1.0f / CONTROL_BOARD.sample_rate;

    for (int i = 0; i < num_samples; i++) {
        float t = i * dt;
        float sample = 0;

        // 叠加所有脉冲
        for (int p = 0; p < num_pulses; p++) {
            QubitPulse *pulse = &pulses[p];
            if (t >= pulse->start_time &&
                t < pulse->start_time + pulse->duration) {

                float envelope = gaussian_envelope(t - pulse->start_time,
                                                    pulse->duration);
                sample += pulse->amplitude * envelope *
                          cosf(2 * M_PI * pulse->frequency * t + pulse->phase);
            }
        }

        // 量化
        dac_samples[i] = quantize(sample, CONTROL_BOARD.resolution);
    }
}

// 读取信号解调
float demodulate_readout(float *adc_samples, int num_samples,
                         float if_frequency, float sample_rate) {
    float i_sum = 0, q_sum = 0;
    float dt = 1.0f / sample_rate;

    for (int i = 0; i < num_samples; i++) {
        float t = i * dt;
        float signal = adc_samples[i];

        // 混频
        i_sum += signal * cosf(2 * M_PI * if_frequency * t);
        q_sum += signal * sinf(2 * M_PI * if_frequency * t);
    }

    // 计算幅度
    return sqrtf(i_sum*i_sum + q_sum*q_sum);
}
