/*
 * Auto-generated from: 04_Industrial_Scenarios\10_Deep_Sea\01_Acoustic_Modem.md
 * Line: 107
 * Language: c
 * Block ID: 0b95b485
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// OFDM水声调制解调器
// 针对多径信道优化
// ============================================================================

#include <stdint.h>
#include <stdbool.h>
#include <complex.h>
#include <math.h>
#include <string.h>

// OFDM参数
typedef struct {
    uint16_t num_subcarriers;       // 子载波数
    uint16_t num_data_subcarriers;  // 数据子载波数
    uint16_t cp_length;             // 循环前缀长度 (samples)
    uint16_t fft_size;              // FFT大小
    float subcarrier_spacing;       // 子载波间隔 (Hz)
    float sampling_rate;            // 采样率 (Hz)
    float bandwidth;                // 信号带宽 (Hz)
    float guard_band;               // 保护频带
} OFDMParams;

// 水声OFDM典型配置
const OFDMParams OFDM_UWA_CONFIG = {
    .num_subcarriers = 1024,
    .num_data_subcarriers = 832,
    .cp_length = 256,           // 约17ms @ 15kHz采样
    .fft_size = 1024,
    .subcarrier_spacing = 14.65f,  // Hz
    .sampling_rate = 15000.0f,     // Hz
    .bandwidth = 9000.0f,          // Hz (6-15kHz)
    .guard_band = 1000.0f          // Hz
};

// 导频位置
typedef struct {
    uint16_t *pilot_indices;
    uint16_t num_pilots;
    float complex *pilot_symbols;
} PilotConfig;

// OFDM帧结构
typedef struct {
    uint16_t num_symbols;
    uint16_t symbol_counter;
    float complex **frequency_data;  // [symbol][subcarrier]
    float complex *time_signal;
    uint32_t signal_length;
} OFDMFrame;

// ============================================================================
// IFFT调制
// ============================================================================

void ofdm_modulate(const OFDMParams *params, const OFDMFrame *frame,
                   float complex *time_domain) {
    uint32_t sample_idx = 0;

    for (int sym = 0; sym < frame->num_symbols; sym++) {
        float complex fft_in[1024];
        float complex fft_out[1024];

        // 构建频域数据 (零填充到FFT大小)
        memset(fft_in, 0, sizeof(fft_in));

        // 映射数据子载波
        int data_start = (params->fft_size - params->num_data_subcarriers) / 2;
        for (int i = 0; i < params->num_data_subcarriers; i++) {
            fft_in[data_start + i] = frame->frequency_data[sym][i];
        }

        // IFFT
        // 实际使用FFTW或其他库
        // ifft(fft_in, fft_out, params->fft_size);

        // 添加循环前缀
        memcpy(time_domain + sample_idx,
               fft_out + params->fft_size - params->cp_length,
               params->cp_length * sizeof(float complex));

        memcpy(time_domain + sample_idx + params->cp_length,
               fft_out,
               params->fft_size * sizeof(float complex));

        sample_idx += params->fft_size + params->cp_length;
    }

    // 上变频到载波频率 (由DAC完成)
}

// ============================================================================
// 信道估计与均衡
// ============================================================================

// 导频辅助信道估计
void channel_estimate_ls(const OFDMParams *params,
                         const PilotConfig *pilots,
                         const float complex *received_pilots,
                         float complex *channel_estimate) {
    // 最小二乘信道估计
    for (int p = 0; p < pilots->num_pilots; p++) {
        uint16_t idx = pilots->pilot_indices[p];
        // H_ls = Y_p / X_p
        channel_estimate[idx] = received_pilots[p] / pilots->pilot_symbols[p];
    }

    // 插值到所有子载波 (线性插值或DFT插值)
    for (int i = 0; i < params->num_subcarriers; i++) {
        // 找到最近的两个导频
        // 线性插值
        // ...
    }
}

// MMSE均衡
void equalize_mmse(const float complex *received,
                   const float complex *channel,
                   float snr,
                   float complex *equalized,
                   int length) {
    float snr_linear = powf(10.0f, snr / 10.0f);

    for (int i = 0; i < length; i++) {
        float complex H = channel[i];
        float H_mag_sq = crealf(H * conjf(H));

        // MMSE均衡系数
        float complex W = conjf(H) / (H_mag_sq + 1.0f / snr_linear);

        equalized[i] = received[i] * W;
    }
}

// ============================================================================
// 多普勒补偿
// ============================================================================

// 多普勒因子估计
typedef struct {
    float doppler_factor;       // (1 + v/c)
    float doppler_shift;        // Hz
    float residual_cfo;         // 残余频偏
} DopplerEstimate;

void estimate_doppler(const float complex *preamble_rx,
                      const float complex *preamble_tx,
                      int length,
                      float sampling_rate,
                      DopplerEstimate *est) {
    // 使用双前导码结构
    // 频率压缩/扩展因子估计

    // 1. 粗估计 (整数倍采样间隔)
    // 相关峰位置偏移

    // 2. 细估计 (分数倍)
    // 相位变化率

    // 简化: 使用已知前导码的自相关特性
    float complex corr = 0;
    for (int i = 0; i < length - 1; i++) {
        corr += preamble_rx[i + 1] * conjf(preamble_rx[i]);
    }

    float phase_diff = atan2f(cimagf(corr), crealf(corr));
    float freq_offset = phase_diff * sampling_rate / (2.0f * M_PI);

    est->doppler_shift = freq_offset;
    est->doppler_factor = 1.0f + freq_offset / 12000.0f;  // 假设载波12kHz
}

// 重采样补偿多普勒
void doppler_compensate(float complex *signal, int length,
                        float doppler_factor,
                        float complex *compensated) {
    // 使用 sinc 插值重采样
    // 简化: 线性插值

    for (int i = 0; i < length; i++) {
        float src_idx = i / doppler_factor;
        int idx_low = (int)floorf(src_idx);
        int idx_high = (int)ceilf(src_idx);
        float frac = src_idx - idx_low;

        if (idx_high >= length) idx_high = length - 1;

        compensated[i] = signal[idx_low] * (1.0f - frac) +
                         signal[idx_high] * frac;
    }
}
