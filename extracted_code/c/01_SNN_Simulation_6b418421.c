/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_SNN_Simulation.md
 * Line: 240
 * Language: c
 * Block ID: 6b418421
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* snn_izhikevich.c */
#include "snn_izhikevich.h"

const IzhikevichParams IZ_REGULAR_SPIKING = {0.02, 0.2, -65.0, 8.0};
const IzhikevichParams IZ_INTRINSICALLY_BURSTING = {0.02, 0.2, -55.0, 4.0};
const IzhikevichParams IZ_CHATTERING = {0.02, 0.2, -50.0, 2.0};

void izhikevich_neuron_init(IzhikevichNeuron *neuron,
                             const IzhikevichParams *params)
{
    neuron->v = -65.0;
    neuron->u = params->b * neuron->v;
    neuron->spike_count = 0;
}

bool izhikevich_update(IzhikevichNeuron *neuron,
                        const IzhikevichParams *params,
                        double i_ext, double dt)
{
    double dv = (0.04 * neuron->v * neuron->v + 5.0 * neuron->v +
                 140.0 - neuron->u + i_ext) * dt;
    double du = (params->a * (params->b * neuron->v - neuron->u)) * dt;

    neuron->v += dv;
    neuron->u += du;

    if (neuron->v >= 30.0) {
        neuron->v = params->c;
        neuron->u += params->d;
        neuron->spike_count++;
        return true;
    }
    return false;
}
