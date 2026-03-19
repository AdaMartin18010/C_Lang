/*
 * Auto-generated from: 04_Industrial_Scenarios\10_Deep_Sea\README.md
 * Line: 139
 * Language: c
 * Block ID: 8508b76b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 蓝绿激光通信（短距离高速）
#define OPTICAL_WAVELENGTH 450e-9  // 450nm 蓝光
#define PULSE_DURATION 1e-9        // 1ns 脉冲

typedef struct {
    float wavelength;
    float beam_divergence;      // 发散角
    float tx_power;
    float receiver_aperture;
    float filter_bandwidth;
} OpticalLinkConfig;

// 光信道建模
float calculate_optical_loss(float distance, float attenuation_coef,
                             float scattering_coef) {
    // Beer-Lambert 定律
    float absorption_loss = exp(-attenuation_coef * distance);

    // 散射损耗（简化模型）
    float scattering_loss = exp(-scattering_coef * distance);

    return absorption_loss * scattering_loss;
}

// 光通信接收机
void optical_receiver(float *adc_samples, size_t num_samples,
                      uint8_t *output_bits) {
    // 带通滤波
    float filtered[1024];
    bandpass_filter(adc_samples, filtered, num_samples,
                    400e6, 500e6, 1e9);  // 400-500MHz 带通

    // 包络检测
    float envelope[1024];
    for (size_t i = 0; i < num_samples; i++) {
        envelope[i] = sqrtf(filtered[i] * filtered[i]);
    }

    // 阈值判决
    float threshold = calculate_adaptive_threshold(envelope, num_samples);

    size_t bit_idx = 0;
    for (size_t i = 0; i < num_samples; i += SAMPLES_PER_BIT) {
        float energy = 0;
        for (size_t j = 0; j < SAMPLES_PER_BIT && i + j < num_samples; j++) {
            energy += envelope[i + j];
        }
        output_bits[bit_idx++] = (energy > threshold) ? 1 : 0;
    }
}
