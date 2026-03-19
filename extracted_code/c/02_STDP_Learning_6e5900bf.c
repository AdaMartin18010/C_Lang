/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\02_STDP_Learning.md
 * Line: 259
 * Language: c
 * Block ID: 6e5900bf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// STDP学习网络
// ============================================================================

#define MAX_STDP_NEURONS    256
#define MAX_STDP_SYNAPSES   1024

// STDP神经元 (带学习能力的LIF)
typedef struct {
    // LIF参数
    float v_mem;
    float v_rest;
    float v_thresh;
    float tau_m;

    // 状态
    bool spiked;
    uint64_t last_spike_time;
    uint32_t spike_count;

    // 输入突触索引列表
    uint16_t input_synapses[32];
    uint8_t num_input_syn;

    // 输出突触索引列表
    uint16_t output_synapses[32];
    uint8_t num_output_syn;
} STDPNeuron;

// STDP网络
typedef struct {
    STDPNeuron neurons[MAX_STDP_NEURONS];
    uint16_t num_neurons;

    STDPSynapse synapses[MAX_STDP_SYNAPSES];
    uint16_t num_synapses;

    // 学习开关
    bool learning_enabled;
    float current_time_ms;
} STDPNetwork;

// 初始化网络
void stdp_network_init(STDPNetwork *net) {
    memset(net, 0, sizeof(STDPNetwork));
    net->learning_enabled = true;
    net->current_time_ms = 0.0f;
}

// 添加神经元
int stdp_add_neuron(STDPNetwork *net, float v_rest, float v_thresh, float tau_m) {
    if (net->num_neurons >= MAX_STDP_NEURONS) return -1;

    STDPNeuron *n = &net->neurons[net->num_neurons];
    n->v_mem = v_rest;
    n->v_rest = v_rest;
    n->v_thresh = v_thresh;
    n->tau_m = tau_m;
    n->spiked = false;
    n->last_spike_time = 0;

    return net->num_neurons++;
}

// 添加STDP连接
int stdp_connect(STDPNetwork *net, int pre_idx, int post_idx,
                 float initial_weight, const STDPParams *params) {
    if (net->num_synapses >= MAX_STDP_SYNAPSES) return -1;

    int syn_idx = net->num_synapses;
    STDPSynapse *syn = &net->synapses[syn_idx];

    stdp_synapse_init(syn, initial_weight, params);

    // 注册到神经元
    STDPNeuron *pre = &net->neurons[pre_idx];
    STDPNeuron *post = &net->neurons[post_idx];

    pre->output_synapses[pre->num_output_syn++] = syn_idx;
    post->input_synapses[post->num_input_syn++] = syn_idx;

    net->num_synapses++;
    return syn_idx;
}

// 网络更新
void stdp_network_step(STDPNetwork *net, float dt_ms) {
    net->current_time_ms += dt_ms;
    uint64_t current_time_us = (uint64_t)(net->current_time_ms * 1000);

    // 1. 收集突触输入并更新膜电位
    for (int n = 0; n < net->num_neurons; n++) {
        STDPNeuron *neuron = &net->neurons[n];

        // 收集所有输入突触的电流
        float i_syn = 0.0f;
        for (int s = 0; s < neuron->num_input_syn; s++) {
            int syn_idx = neuron->input_synapses[s];
            STDPSynapse *syn = &net->synapses[syn_idx];

            // 这里简化处理，实际应有脉冲队列
            i_syn += syn->weight;
        }

        // 更新LIF
        float dv = (-(neuron->v_mem - neuron->v_rest) + i_syn) / neuron->tau_m;
        neuron->v_mem += dv * dt_ms;
        neuron->spiked = (neuron->v_mem >= neuron->v_thresh);

        if (neuron->spiked) {
            neuron->v_mem = neuron->v_rest;
            neuron->last_spike_time = current_time_us;
            neuron->spike_count++;
        }
    }

    // 2. STDP学习
    if (net->learning_enabled) {
        for (int n = 0; n < net->num_neurons; n++) {
            STDPNeuron *neuron = &net->neurons[n];

            if (neuron->spiked) {
                // 后突触脉冲: 触发LTP (输入突触增强)
                for (int s = 0; s < neuron->num_input_syn; s++) {
                    int syn_idx = neuron->input_synapses[s];
                    stdp_post_spike(&net->synapses[syn_idx], current_time_us);
                }

                // 触发LTD (输出突触抑制)
                for (int s = 0; s < neuron->num_output_syn; s++) {
                    int syn_idx = neuron->output_synapses[s];
                    stdp_pre_spike(&net->synapses[syn_idx], current_time_us);
                }
            }
        }
    }
}
