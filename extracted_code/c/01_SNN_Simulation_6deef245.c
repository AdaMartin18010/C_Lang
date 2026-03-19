/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_SNN_Simulation.md
 * Line: 134
 * Language: c
 * Block ID: 6deef245
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* snn_lif.c - LIF模型实现 */
#include "snn_lif.h"

void lif_init_params(LIFParams *params, double tau_m, double v_rest,
                      double v_reset, double v_thresh, double r_membrane,
                      double tau_ref)
{
    params->tau_m = tau_m;
    params->v_rest = v_rest;
    params->v_reset = v_reset;
    params->v_thresh = v_thresh;
    params->r_membrane = r_membrane;
    params->tau_ref = tau_ref;
}

void lif_neuron_init(LIFNeuron *neuron, const LIFParams *params)
{
    neuron->v = params->v_rest;
    neuron->i_syn = 0.0;
    neuron->last_spike_time = -1e10;
    neuron->is_refractory = false;
    neuron->spike_count = 0;
}

void lif_receive_spike(LIFNeuron *neuron, double weight, double current_time)
{
    (void)current_time;
    neuron->i_syn += weight;
}

bool lif_update(LIFNeuron *neuron, const LIFParams *params,
                double i_ext, double dt, double current_time)
{
    if (neuron->is_refractory) {
        if (current_time - neuron->last_spike_time >= params->tau_ref) {
            neuron->is_refractory = false;
        } else {
            return false;
        }
    }

    double i_total = neuron->i_syn + i_ext;
    double dv = (-(neuron->v - params->v_rest) +
                  params->r_membrane * i_total) / params->tau_m;
    neuron->v += dv * dt;

    neuron->i_syn *= 0.9;

    if (neuron->v >= params->v_thresh) {
        neuron->v = params->v_reset;
        neuron->last_spike_time = current_time;
        neuron->is_refractory = true;
        neuron->spike_count++;
        return true;
    }

    return false;
}
