/*
 * Auto-generated from: 04_Industrial_Scenarios\10_Deep_Sea\README.md
 * Line: 39
 * Language: c
 * Block ID: 93a0734e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 水声通信物理层参数
typedef struct {
    float carrier_freq;      // 载波频率 (Hz)
    float bandwidth;         // 带宽 (Hz)
    float symbol_rate;       // 符号率 (baud)
    int modulation_type;     // 调制方式
    float tx_power;          // 发射功率 (W)
} AcousticConfig;

// 调制方式枚举
typedef enum {
    MOD_FSK,        // 频移键控
    MOD_PSK,        // 相移键控
    MOD_OFDM,       // 正交频分复用
    MOD_DSSS        // 直接序列扩频
} ModulationType;

// OFDM 水声通信实现
#define OFDM_SUBCARRIERS 64
#define OFDM_CP_LEN 16      // 循环前缀长度
#define OFDM_SYMBOL_LEN (OFDM_SUBCARRIERS + OFDM_CP_LEN)

typedef struct {
    float real;
    float imag;
} complex_float;

// IFFT 实现（基2时域抽取）
void ifft(complex_float *in, complex_float *out, int n) {
    // 位反转排列
    for (int i = 0; i < n; i++) {
        int j = bit_reverse(i, log2(n));
        out[j] = in[i];
    }

    // 蝶形运算
    for (int s = 1; s <= log2(n); s++) {
        int m = 1 << s;
        complex_float wm = {
            cosf(2 * M_PI / m),
            -sinf(2 * M_PI / m)
        };

        for (int k = 0; k < n; k += m) {
            complex_float w = {1.0f, 0.0f};
            for (int j = 0; j < m/2; j++) {
                complex_float t = complex_mul(w, out[k + j + m/2]);
                complex_float u = out[k + j];
                out[k + j] = complex_add(u, t);
                out[k + j + m/2] = complex_sub(u, t);
                w = complex_mul(w, wm);
            }
        }
    }
}

// OFDM 调制器
void ofdm_modulate(const uint8_t *data, size_t len,
                   float *output, const AcousticConfig *cfg) {
    complex_float freq_domain[OFDM_SUBCARRIERS];
    complex_float time_domain[OFDM_SUBCARRIERS];

    size_t bits_per_symbol = 2;  // QPSK
    size_t symbols = (len * 8 + bits_per_symbol - 1) / bits_per_symbol;

    for (size_t s = 0; s < symbols; s++) {
        // 将比特映射到星座点
        for (int k = 0; k < OFDM_SUBCARRIERS; k++) {
            int bit_idx = s * OFDM_SUBCARRIERS * bits_per_symbol +
                          k * bits_per_symbol;
            if (bit_idx / 8 < len) {
                int bits = (data[bit_idx / 8] >> (bit_idx % 8)) & 0x3;
                // QPSK 映射
                freq_domain[k].real = (bits & 1) ? 0.707f : -0.707f;
                freq_domain[k].imag = (bits & 2) ? 0.707f : -0.707f;
            } else {
                freq_domain[k].real = 0;
                freq_domain[k].imag = 0;
            }
        }

        // IFFT
        ifft(freq_domain, time_domain, OFDM_SUBCARRIERS);

        // 添加循环前缀
        int out_idx = s * OFDM_SYMBOL_LEN;
        for (int i = 0; i < OFDM_CP_LEN; i++) {
            output[out_idx + i] = time_domain[OFDM_SUBCARRIERS -
                                              OFDM_CP_LEN + i].real;
        }
        for (int i = 0; i < OFDM_SUBCARRIERS; i++) {
            output[out_idx + OFDM_CP_LEN + i] = time_domain[i].real;
        }
    }
}
