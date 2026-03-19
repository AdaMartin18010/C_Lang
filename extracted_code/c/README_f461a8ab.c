/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\README.md
 * Line: 273
 * Language: c
 * Block ID: f461a8ab
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* nr_ofdm.c - OFDM调制实现 */
#include "nr_dsp.h"
#include <string.h>
#include <math.h>

/* OFDM配置 */
#define N_FFT_MAX       4096
#define CP_NORMAL_LEN   144  /* 常规CP长度 (30kHz SCS) */
#define CP_EXTENDED_LEN 512  /* 扩展CP长度 */

/* IFFT实现 - 使用蝶形运算 */
static void butterfly(cf_t *a, cf_t *b, cf_t w) {
    cf_t temp = *b * w;
    *b = *a - temp;
    *a = *a + temp;
}

/* 基-2 DIT FFT */
void fft_radix2(cf_t *data, uint32_t n, bool inverse) {
    /* 位反转重排 */
    uint32_t j = 0;
    for (uint32_t i = 1; i < n; i++) {
        uint32_t bit = n >> 1;
        for (; j & bit; bit >>= 1) {
            j ^= bit;
        }
        j ^= bit;

        if (i < j) {
            cf_t temp = data[i];
            data[i] = data[j];
            data[j] = temp;
        }
    }

    /* FFT蝶形运算 */
    for (uint32_t len = 2; len <= n; len <<= 1) {
        float ang = 2 * M_PI / len * (inverse ? -1 : 1);
        cf_t wlen = cosf(ang) + I * sinf(ang);

        for (uint32_t i = 0; i < n; i += len) {
            cf_t w = 1.0f + 0.0f * I;
            for (uint32_t j = 0; j < len / 2; j++) {
                butterfly(&data[i + j], &data[i + j + len / 2], w);
                w *= wlen;
            }
        }
    }

    /* 归一化 */
    if (inverse) {
        float scale = 1.0f / n;
        for (uint32_t i = 0; i < n; i++) {
            data[i] *= scale;
        }
    }
}

/* OFDM调制 - IFFT + CP添加 */
int nr_ofdm_modulate(const resource_grid_t *grid, cf_t *time_signal) {
    uint32_t n_fft = 2048;  /* 根据带宽配置 */
    uint32_t cp_len = CP_NORMAL_LEN;

    cf_t fft_input[N_FFT_MAX];
    cf_t fft_output[N_FFT_MAX];

    for (uint32_t sym = 0; sym < grid->num_symbols; sym++) {
        /* 构建FFT输入 (频域 -> 时域) */
        memset(fft_input, 0, n_fft * sizeof(cf_t));

        /* 将资源网格数据映射到子载波 */
        uint32_t sc_offset = n_fft / 2 - grid->num_rb * 6; /* 中心对齐 */
        for (uint32_t rb = 0; rb < grid->num_rb; rb++) {
            for (uint32_t sc = 0; sc < 12; sc++) {
                uint32_t grid_idx = sym * grid->num_rb * 12 + rb * 12 + sc;
                uint32_t fft_idx = sc_offset + rb * 12 + sc;
                fft_input[fft_idx] = grid->data[grid_idx];
            }
        }

        /* IFFT */
        memcpy(fft_output, fft_input, n_fft * sizeof(cf_t));
        fft_radix2(fft_output, n_fft, true);

        /* 添加循环前缀 */
        uint32_t ts_offset = sym * (n_fft + cp_len);
        for (uint32_t i = 0; i < cp_len; i++) {
            time_signal[ts_offset + i] = fft_output[n_fft - cp_len + i];
        }
        for (uint32_t i = 0; i < n_fft; i++) {
            time_signal[ts_offset + cp_len + i] = fft_output[i];
        }
    }

    return 0;
}
