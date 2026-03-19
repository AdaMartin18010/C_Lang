/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_SNN_Simulation.md
 * Line: 423
 * Language: c
 * Block ID: 794aa224
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* snn_network.h - SNN网络定义 */
# ifndef SNN_NETWORK_H
# define SNN_NETWORK_H

# include "snn_lif.h"
# include "snn_stdp.h"
# include <stdint.h>
# include <stdbool.h>

# define MAX_NEURONS 10000
# define MAX_SYNAPSES 100000

typedef enum {
    LAYER_INPUT,
    LAYER_HIDDEN,
    LAYER_OUTPUT
} LayerType;

/* 网络层 */
typedef struct {
    LayerType type;
    uint32_t start_idx;
    uint32_t num_neurons;
    LIFParams neuron_params;
} Layer;

/* SNN网络 */
typedef struct {
    /* 神经元 */
    LIFNeuron neurons[MAX_NEURONS];
    uint32_t num_neurons;

    /* 层 */
    Layer layers[10];
    uint32_t num_layers;

    /* 突触连接 */
    Synapse synapses[MAX_SYNAPSES];
    STDPtrace traces[MAX_SYNAPSES];
    uint32_t num_synapses;

    /* 网络状态 */
    bool spikes[MAX_NEURONS];
    double current_time;
    double dt;

    /* STDP参数 */
    STDPParams stdp_params;
    bool enable_stdp;
} SNNNetwork;

void snn_init(SNNNetwork *net, double dt);
int snn_add_layer(SNNNetwork *net, LayerType type, uint32_t num_neurons,
                   const LIFParams *params);
void snn_connect_layers(SNNNetwork *net, uint32_t pre_layer, uint32_t post_layer,
                        double connection_prob, double weight_mean, double weight_std);
void snn_update(SNNNetwork *net);
void snn_apply_input(SNNNetwork *net, uint32_t layer_idx,
                      const double *input_currents);
void snn_get_output_spikes(SNNNetwork *net, uint32_t layer_idx,
                            bool *output_spikes);
void snn_get_output_rates(SNNNetwork *net, uint32_t layer_idx,
                           double *rates, double window_ms);

# endif /* SNN_NETWORK_H */
