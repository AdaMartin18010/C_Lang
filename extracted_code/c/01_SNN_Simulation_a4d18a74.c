/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_SNN_Simulation.md
 * Line: 207
 * Language: c
 * Block ID: a4d18a74
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* snn_izhikevich.h - Izhikevich模型 */
#ifndef SNN_IZHIKEVICH_H
#define SNN_IZHIKEVICH_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    double a;
    double b;
    double c;
    double d;
} IzhikevichParams;

typedef struct {
    double v;
    double u;
    uint32_t spike_count;
} IzhikevichNeuron;

extern const IzhikevichParams IZ_REGULAR_SPIKING;
extern const IzhikevichParams IZ_INTRINSICALLY_BURSTING;
extern const IzhikevichParams IZ_CHATTERING;

void izhikevich_neuron_init(IzhikevichNeuron *neuron,
                             const IzhikevichParams *params);
bool izhikevich_update(IzhikevichNeuron *neuron,
                        const IzhikevichParams *params,
                        double i_ext, double dt);
#endif
