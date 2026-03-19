/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\README.md
 * Line: 374
 * Language: c
 * Block ID: e83bc997
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* nr_channel.c - 信道估计与均衡 */
#include "nr_phy.h"
#include <math.h>

/* LS信道估计 */
void channel_estimate_ls(const cf_t *rx_pilots,
                         const cf_t *tx_pilots,
                         cf_t *channel_est,
                         uint32_t num_pilots) {
    /* H_est = Y / X (逐元素除法) */
    for (uint32_t i = 0; i < num_pilots; i++) {
        channel_est[i] = rx_pilots[i] / tx_pilots[i];
    }
}

/* 线性插值扩展到整个资源网格 */
void channel_interpolate_linear(const cf_t *pilot_est,
                                const uint32_t *pilot_sc,
                                uint32_t num_pilots,
                                cf_t *full_grid,
                                uint32_t num_sc) {
    uint32_t pilot_idx = 0;

    for (uint32_t sc = 0; sc < num_sc; sc++) {
        if (pilot_idx < num_pilots - 1 && sc >= pilot_sc[pilot_idx + 1]) {
            pilot_idx++;
        }

        if (pilot_idx == 0 && sc < pilot_sc[0]) {
            /* 第一个导频之前的点 */
            full_grid[sc] = pilot_est[0];
        } else if (pilot_idx >= num_pilots - 1) {
            /* 最后一个导频之后的点 */
            full_grid[sc] = pilot_est[num_pilots - 1];
        } else {
            /* 线性插值 */
            uint32_t sc1 = pilot_sc[pilot_idx];
            uint32_t sc2 = pilot_sc[pilot_idx + 1];
            float alpha = (float)(sc - sc1) / (sc2 - sc1);

            full_grid[sc] = (1.0f - alpha) * pilot_est[pilot_idx] +
                            alpha * pilot_est[pilot_idx + 1];
        }
    }
}

/* MMSE均衡 */
void equalize_mmse(const cf_t *rx_signal,
                   const cf_t *channel,
                   float noise_var,
                   cf_t *eq_signal,
                   uint32_t num_samples) {
    for (uint32_t i = 0; i < num_samples; i++) {
        cf_t h = channel[i];
        float h2 = crealf(h) * crealf(h) + cimagf(h) * cimagf(h);

        /* MMSE系数: H* / (|H|^2 + sigma^2) */
        cf_t mmse_coef = conjf(h) / (h2 + noise_var);
        eq_signal[i] = rx_signal[i] * mmse_coef;
    }
}
