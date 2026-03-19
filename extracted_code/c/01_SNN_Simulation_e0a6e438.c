/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_SNN_Simulation.md
 * Line: 95
 * Language: c
 * Block ID: e0a6e438
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* snn_lif.h - LIF神经元模型头文件 */
#ifndef SNN_LIF_H
#define SNN_LIF_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/* LIF神经元参数 */
typedef struct {
    double tau_m;        /* 膜时间常数 (ms) */
    double v_rest;       /* 静息电位 (mV) */
    double v_reset;      /* 重置电位 (mV) */
    double v_thresh;     /* 发放阈值 (mV) */
    double r_membrane;   /* 膜电阻 (MΩ) */
    double tau_ref;      /* 不应期 (ms) */
} LIFParams;

/* LIF神经元状态 */
typedef struct {
    double v;            /* 当前膜电位 */
    double i_syn;        /* 突触电流 */
    double last_spike_time; /* 上次发放时间 */
    bool is_refractory;  /* 是否处于不应期 */
    uint32_t spike_count; /* 脉冲计数 */
} LIFNeuron;

void lif_init_params(LIFParams *params, double tau_m, double v_rest,
                      double v_reset, double v_thresh, double r_membrane,
                      double tau_ref);
void lif_neuron_init(LIFNeuron *neuron, const LIFParams *params);
bool lif_update(LIFNeuron *neuron, const LIFParams *params,
                double i_ext, double dt, double current_time);
void lif_receive_spike(LIFNeuron *neuron, double weight, double current_time);

#endif /* SNN_LIF_H */
