/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_SNN_Simulation.md
 * Line: 491
 * Language: c
 * Block ID: 2b88f0f3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* snn_network.c - SNN网络实现 */
# include "snn_network.h"
# include <stdlib.h>
# include <math.h>
# include <string.h>

void snn_init(SNNNetwork *net, double dt)
{
    memset(net, 0, sizeof(SNNNetwork));
    net->dt = dt;
    net->current_time = 0.0;
    net->enable_stdp = false;
    stdp_init_params(&net->stdp_params);
}

int snn_add_layer(SNNNetwork *net, LayerType type, uint32_t num_neurons,
                   const LIFParams *params)
{
    if (net->num_layers >= 10) return -1;
    if (net->num_neurons + num_neurons > MAX_NEURONS) return -1;

    Layer *layer = &net->layers[net->num_layers];
    layer->type = type;
    layer->start_idx = net->num_neurons;
    layer->num_neurons = num_neurons;
    layer->neuron_params = *params;

    /* 初始化神经元 */
    for (uint32_t i = 0; i < num_neurons; i++) {
        lif_neuron_init(&net->neurons[net->num_neurons + i], params);
    }

    net->num_neurons += num_neurons;
    net->num_layers++;

    return (int)(net->num_layers - 1);
}

void snn_connect_layers(SNNNetwork *net, uint32_t pre_layer, uint32_t post_layer,
                        double connection_prob, double weight_mean, double weight_std)
{
    Layer *pre = &net->layers[pre_layer];
    Layer *post = &net->layers[post_layer];

    for (uint32_t i = 0; i < pre->num_neurons; i++) {
        for (uint32_t j = 0; j < post->num_neurons; j++) {
            if ((double)rand() / RAND_MAX < connection_prob) {
                if (net->num_synapses >= MAX_SYNAPSES) return;

                /* 生成高斯分布权重 */
                double u1 = (double)rand() / RAND_MAX;
                double u2 = (double)rand() / RAND_MAX;
                double z = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
                double weight = weight_mean + weight_std * z;

                stdp_init_synapse(&net->synapses[net->num_synapses],
                                  pre->start_idx + i,
                                  post->start_idx + j,
                                  weight);
                net->num_synapses++;
            }
        }
    }
}

void snn_update(SNNNetwork *net)
{
    /* 清除脉冲记录 */
    memset(net->spikes, 0, sizeof(bool) * net->num_neurons);

    /* 1. 更新所有神经元 */
    for (uint32_t i = 0; i < net->num_neurons; i++) {
        /* 找到神经元所在层 */
        Layer *layer = NULL;
        for (uint32_t l = 0; l < net->num_layers; l++) {
            if (i >= net->layers[l].start_idx &&
                i < net->layers[l].start_idx + net->layers[l].num_neurons) {
                layer = &net->layers[l];
                break;
            }
        }

        if (layer) {
            net->spikes[i] = lif_update(&net->neurons[i], &layer->neuron_params,
                                         0.0, net->dt, net->current_time);
        }
    }

    /* 2. 传递脉冲和处理STDP */
    for (uint32_t s = 0; s < net->num_synapses; s++) {
        Synapse *syn = &net->synapses[s];
        uint32_t pre_idx = syn->pre_neuron;
        uint32_t post_idx = syn->post_neuron;

        /* 突触前脉冲传递 */
        if (net->spikes[pre_idx]) {
            lif_receive_spike(&net->neurons[post_idx], syn->weight,
                               net->current_time);

            if (net->enable_stdp) {
                stdp_pre_spike(syn, &net->stdp_params,
                               &net->traces[s], net->current_time);
            }
        }

        /* 突触后脉冲STDP */
        if (net->enable_stdp && net->spikes[post_idx]) {
            stdp_post_spike(syn, &net->stdp_params,
                            &net->traces[s], net->current_time);
        }

        /* 更新迹 */
        if (net->enable_stdp) {
            stdp_update_traces(&net->traces[s], &net->stdp_params, net->dt);
        }
    }

    net->current_time += net->dt;
}

void snn_apply_input(SNNNetwork *net, uint32_t layer_idx,
                      const double *input_currents)
{
    Layer *layer = &net->layers[layer_idx];
    for (uint32_t i = 0; i < layer->num_neurons; i++) {
        net->neurons[layer->start_idx + i].i_syn += input_currents[i];
    }
}

void snn_get_output_spikes(SNNNetwork *net, uint32_t layer_idx,
                            bool *output_spikes)
{
    Layer *layer = &net->layers[layer_idx];
    for (uint32_t i = 0; i < layer->num_neurons; i++) {
        output_spikes[i] = net->spikes[layer->start_idx + i];
    }
}

void snn_get_output_rates(SNNNetwork *net, uint32_t layer_idx,
                           double *rates, double window_ms)
{
    Layer *layer = &net->layers[layer_idx];
    double window_factor = 1000.0 / window_ms; /* 转换为Hz */

    for (uint32_t i = 0; i < layer->num_neurons; i++) {
        rates[i] = net->neurons[layer->start_idx + i].spike_count * window_factor;
    }
}
