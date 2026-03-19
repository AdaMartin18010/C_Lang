/*
 * Auto-generated from: 10_WebAssembly_C\02_Emscripten_Advanced_Usage.md
 * Line: 2967
 * Language: c
 * Block ID: 0a00be40
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// audio_processor.c
#include <emscripten/emscripten.h>
#include <math.h>

#define SAMPLE_RATE 44100
#define MAX_CHANNELS 2

// 音频缓冲区
typedef struct {
    float* data;
    int length;
    int channels;
} AudioBuffer;

// 生成正弦波
EMSCRIPTEN_KEEPALIVE
void generate_sine_wave(float* buffer, int length, float frequency, float amplitude) {
    for (int i = 0; i < length; i++) {
        float t = (float)i / SAMPLE_RATE;
        buffer[i] = amplitude * sinf(2.0f * 3.14159265f * frequency * t);
    }
}

// 应用增益
EMSCRIPTEN_KEEPALIVE
void apply_gain(float* buffer, int length, float gain) {
    for (int i = 0; i < length; i++) {
        buffer[i] *= gain;
        // 限幅
        if (buffer[i] > 1.0f) buffer[i] = 1.0f;
        if (buffer[i] < -1.0f) buffer[i] = -1.0f;
    }
}

// 简单混响效果
EMSCRIPTEN_KEEPALIVE
void apply_reverb(float* input, float* output, int length, float mix, float decay) {
    static float delay_buffer[44100] = {0};  // 1秒延迟
    static int delay_pos = 0;

    for (int i = 0; i < length; i++) {
        float delayed = delay_buffer[delay_pos];
        delay_buffer[delay_pos] = input[i] + delayed * decay;
        output[i] = input[i] * (1.0f - mix) + delayed * mix;
        delay_pos = (delay_pos + 1) % 44100;
    }
}

// FFT 辅助函数（简化版）
EMSCRIPTEN_KEEPALIVE
void compute_spectrum(float* input, float* output, int length) {
    // 简化的频谱计算
    // 实际应用中应使用完整的 FFT 实现
    for (int i = 0; i < length / 2; i++) {
        float sum = 0;
        for (int j = i * 2; j < (i + 1) * 2 && j < length; j++) {
            sum += fabsf(input[j]);
        }
        output[i] = sum / 2.0f;
    }
}
