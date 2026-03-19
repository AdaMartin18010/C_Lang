/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\README.md
 * Line: 97
 * Language: c
 * Block ID: 4cef73f9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* nr_phy.h - 5G NR物理层定义 */
#ifndef NR_PHY_H
#define NR_PHY_H

#include <stdint.h>
#include <stdbool.h>
#include <complex.h>

/* 复数类型定义 */
typedef float complex cf_t;
typedef double complex cd_t;

/* 参数集 (Numerology) */
typedef enum {
    MU_0 = 0,   /* SCS = 15 kHz */
    MU_1 = 1,   /* SCS = 30 kHz */
    MU_2 = 2,   /* SCS = 60 kHz */
    MU_3 = 3,   /* SCS = 120 kHz */
    MU_4 = 4    /* SCS = 240 kHz */
} numerology_t;

/* 子载波间隔 (Hz) */
#define SUBCARRIER_SPACING(mu) (15000 << (mu))

/* 时隙配置 */
typedef struct {
    numerology_t mu;
    uint32_t slot_number;
    uint32_t symbol_number;
    uint32_t subframe_number;
    uint32_t frame_number;
} slot_config_t;

/* 资源网格 */
typedef struct {
    uint32_t num_rb;            /* 资源块数 */
    uint32_t num_symbols;       /* 每时隙符号数 */
    uint32_t num_subcarriers;   /* 每RB子载频数 (12) */
    cf_t *data;                 /* 复数数据 [符号][子载频] */
    bool *allocated;            /* 资源分配掩码 */
} resource_grid_t;

/* 信道估计结果 */
typedef struct {
    cf_t *channel_response;     /* 信道响应 H */
    float *noise_variance;      /* 噪声方差 */
    uint32_t num_pilots;        /* 导频数量 */
} channel_estimate_t;

/* 调制配置 */
typedef enum {
    MOD_QPSK = 2,
    MOD_16QAM = 4,
    MOD_64QAM = 6,
    MOD_256QAM = 8
} modulation_t;

/* 物理层配置 */
typedef struct {
    numerology_t mu;
    uint32_t carrier_bandwidth_hz;
    uint32_t num_tx_antennas;
    uint32_t num_rx_antennas;
    modulation_t max_modulation;
    bool enable_mimo;
    uint32_t num_layers;
} phy_config_t;

/* 物理层处理函数 */
int nr_phy_init(phy_config_t *config);
void nr_phy_deinit(void);

/* 调制解调 */
int nr_modulate(const uint8_t *bits, uint32_t num_bits,
                modulation_t mod, cf_t *symbols);
int nr_demodulate(const cf_t *symbols, uint32_t num_symbols,
                  modulation_t mod, float *llr);

/* OFDM处理 */
int nr_ofdm_modulate(const resource_grid_t *grid, cf_t *time_signal);
int nr_ofdm_demodulate(const cf_t *time_signal, resource_grid_t *grid);

/* 信道估计与均衡 */
int nr_channel_estimate(const resource_grid_t *rx_grid,
                        const cf_t *reference_signals,
                        channel_estimate_t *est);
int nr_channel_equalize(const resource_grid_t *rx_grid,
                        const channel_estimate_t *est,
                        resource_grid_t *eq_grid);

/* MIMO处理 */
int nr_mimo_precoding(cf_t *layers, uint32_t num_layers,
                      cf_t *antenna_ports, uint32_t num_ports);
int nr_mimo_detection(const cf_t *rx_signals,
                      const channel_estimate_t *ch_est,
                      cf_t *detected_layers);

#endif /* NR_PHY_H */
