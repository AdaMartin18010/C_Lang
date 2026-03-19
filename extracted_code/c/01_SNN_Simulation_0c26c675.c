/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_SNN_Simulation.md
 * Line: 294
 * Language: c
 * Block ID: 0c26c675
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* snn_stdp.h - STDP学习规则 */
# ifndef SNN_STDP_H
# define SNN_STDP_H

# include <stdint.h>
# include <stdbool.h>

/* STDP参数 */
typedef struct {
    double A_plus;       /* LTP幅度 */
    double A_minus;      /* LTD幅度 */
    double tau_plus;     /* LTP时间常数 */
    double tau_minus;    /* LTD时间常数 */
    double w_min;        /* 最小权重 */
    double w_max;        /* 最大权重 */
    double learning_rate;/* 学习率 */
} STDPParams;

/* 突触连接 */
typedef struct {
    uint32_t pre_neuron;   /* 突触前神经元索引 */
    uint32_t post_neuron;  /* 突触后神经元索引 */
    double weight;         /* 突触权重 */
    double last_pre_time;  /* 上次突触前脉冲时间 */
    double last_post_time; /* 上次突触后脉冲时间 */
} Synapse;

/* STDP迹 */
typedef struct {
    double pre_trace;   /* 突触前迹 */
    double post_trace;  /* 突触后迹 */
} STDPtrace;

void stdp_init_params(STDPParams *params);
void stdp_init_synapse(Synapse *syn, uint32_t pre, uint32_t post, double w);
void stdp_pre_spike(Synapse *syn, STDPParams *params,
                     STDPtrace *trace, double current_time);
void stdp_post_spike(Synapse *syn, STDPParams *params,
                      STDPtrace *trace, double current_time);
void stdp_update_traces(STDPtrace *trace, STDPParams *params, double dt);

# endif /* SNN_STDP_H */
