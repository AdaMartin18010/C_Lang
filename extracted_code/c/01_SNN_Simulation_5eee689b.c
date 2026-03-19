/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_SNN_Simulation.md
 * Line: 869
 * Language: c
 * Block ID: 5eee689b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* temporal_coding.c - 时间编码示例 */
# include <stdio.h>
# include <math.h>

/* 首次脉冲时间编码 */
void encode_time_to_first_spike(double value, double min_val, double max_val,
                                 double *spike_time, double max_time)
{
    /* 归一化到0-1 */
    double normalized = (value - min_val) / (max_val - min_val);
    /* 反转：越大越早发放 */
    *spike_time = max_time * (1.0 - normalized);
}

/* 延迟学习 (Tempotron) */
void update_delays_tempotron(double *delays, const double *spike_times,
                              uint32_t num_inputs, double target_time,
                              double learning_rate, double threshold)
{
    /* 计算膜电位峰值时间 */
    double v_max = 0.0;
    double t_max = 0.0;

    for (uint32_t i = 0; i < num_inputs; i++) {
        if (spike_times[i] > 0 && spike_times[i] < target_time) {
            double v = 0.0;
            for (uint32_t j = 0; j < num_inputs; j++) {
                /* 简化PSP计算 */
                double dt = spike_times[i] - spike_times[j] - delays[j];
                if (dt > 0) {
                    v += exp(-dt / 10.0);
                }
            }
            if (v > v_max) {
                v_max = v;
                t_max = spike_times[i];
            }
        }
    }

    /* 更新延迟 */
    if (v_max >= threshold) {
        /* 应该发放但没有 - LTD */
        for (uint32_t i = 0; i < num_inputs; i++) {
            if (spike_times[i] > 0 && spike_times[i] < t_max) {
                delays[i] -= learning_rate;
            }
        }
    } else {
        /* 不应该发放但发放了 - LTP */
        for (uint32_t i = 0; i < num_inputs; i++) {
            if (spike_times[i] > 0 && spike_times[i] < target_time) {
                delays[i] += learning_rate;
            }
        }
    }
}
