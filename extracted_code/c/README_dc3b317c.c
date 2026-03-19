/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\README.md
 * Line: 204
 * Language: c
 * Block ID: dc3b317c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* nr_dsp.h - DSP核心算法 */
#ifndef NR_DSP_H
#define NR_DSP_H

#include "nr_phy.h"

/* FFT/IFFT配置 */
typedef struct {
    uint32_t size;
    bool inverse;
    void *twiddle_factors;
    void *plan;                 /* FFTW计划或自定义 */
} fft_config_t;

/* 卷积编码 */
typedef struct {
    uint32_t constraint_length;
    uint32_t code_rate_num;
    uint32_t code_rate_den;
    uint32_t *generator_polynomials;
} conv_code_t;

/* 矩阵运算 */
typedef struct {
    uint32_t rows;
    uint32_t cols;
    cf_t *data;                 /* 行优先存储 */
} matrix_cf_t;

/* FFT/IFFT */
int fft_init(fft_config_t *cfg, uint32_t size, bool inverse);
void fft_execute(fft_config_t *cfg, cf_t *input, cf_t *output);
void fft_deinit(fft_config_t *cfg);

/* FIR滤波器 */
typedef struct {
    float *coeffs;
    uint32_t num_taps;
    float *state;
    uint32_t state_index;
} fir_filter_t;

void fir_init(fir_filter_t *filter, const float *coeffs, uint32_t num_taps);
void fir_execute(fir_filter_t *filter, const float *input,
                 float *output, uint32_t num_samples);
void fir_deinit(fir_filter_t *filter);

/* 相关运算 */
void xcorr(const cf_t *x, const cf_t *y, uint32_t len,
           cf_t *result, int32_t min_lag, int32_t max_lag);

/* 线性代数 */
int matrix_multiply(const matrix_cf_t *a, const matrix_cf_t *b,
                    matrix_cf_t *c);
int matrix_inverse(const matrix_cf_t *a, matrix_cf_t *inv);
int matrix_cholesky(const matrix_cf_t *a, matrix_cf_t *l);

/* 插值 */
void interpolate_linear(const cf_t *input, uint32_t in_len,
                        cf_t *output, uint32_t out_len);
void interpolate_sinc(const cf_t *input, uint32_t in_len,
                      cf_t *output, uint32_t out_len);

#endif /* NR_DSP_H */
