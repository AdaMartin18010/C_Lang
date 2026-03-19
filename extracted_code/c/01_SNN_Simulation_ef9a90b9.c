/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_SNN_Simulation.md
 * Line: 339
 * Language: c
 * Block ID: ef9a90b9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* snn_stdp.c - STDP实现 */
# include "snn_stdp.h"
# include <math.h>

void stdp_init_params(STDPParams *params)
{
    params->A_plus = 0.1;
    params->A_minus = 0.1;
    params->tau_plus = 20.0;   /* ms */
    params->tau_minus = 20.0;  /* ms */
    params->w_min = 0.0;
    params->w_max = 1.0;
    params->learning_rate = 0.01;
}

void stdp_init_synapse(Synapse *syn, uint32_t pre, uint32_t post, double w)
{
    syn->pre_neuron = pre;
    syn->post_neuron = post;
    syn->weight = w;
    syn->last_pre_time = -1e10;
    syn->last_post_time = -1e10;
}

void stdp_pre_spike(Synapse *syn, STDPParams *params,
                     STDPtrace *trace, double current_time)
{
    syn->last_pre_time = current_time;

    /* 计算时间差 */
    double dt = current_time - syn->last_post_time;

    if (dt > 0 && dt < 5 * params->tau_minus) {
        /* LTD: 突触后先于突触前发放 */
        double dw = -params->A_minus * exp(-dt / params->tau_minus);
        dw *= trace->post_trace;
        syn->weight += params->learning_rate * dw;
    }

    /* 边界限制 */
    if (syn->weight < params->w_min) syn->weight = params->w_min;
    if (syn->weight > params->w_max) syn->weight = params->w_max;

    /* 更新突触前迹 */
    trace->pre_trace += 1.0;
}

void stdp_post_spike(Synapse *syn, STDPParams *params,
                      STDPtrace *trace, double current_time)
{
    syn->last_post_time = current_time;

    /* 计算时间差 */
    double dt = current_time - syn->last_pre_time;

    if (dt > 0 && dt < 5 * params->tau_plus) {
        /* LTP: 突触前先于突触后发放 */
        double dw = params->A_plus * exp(-dt / params->tau_plus);
        dw *= trace->pre_trace;
        syn->weight += params->learning_rate * dw;
    }

    /* 边界限制 */
    if (syn->weight < params->w_min) syn->weight = params->w_min;
    if (syn->weight > params->w_max) syn->weight = params->w_max;

    /* 更新突触后迹 */
    trace->post_trace += 1.0;
}

void stdp_update_traces(STDPtrace *trace, STDPParams *params, double dt)
{
    trace->pre_trace *= exp(-dt / params->tau_plus);
    trace->post_trace *= exp(-dt / params->tau_minus);
}
