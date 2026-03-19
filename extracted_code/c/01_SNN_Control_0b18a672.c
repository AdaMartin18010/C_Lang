/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\01_SNN_Control.md
 * Line: 314
 * Language: c
 * Block ID: 0b18a672
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// SNN网络控制器
// ============================================================================

#define MAX_NEURONS     1024
#define MAX_SYNAPSES    4096
#define MAX_LAYERS      10

// 层类型
typedef enum {
    LAYER_INPUT,
    LAYER_HIDDEN,
    LAYER_OUTPUT
} LayerType;

// 网络层
typedef struct {
    LayerType type;
    uint16_t start_idx;     // 起始神经元索引
    uint16_t num_neurons;
    LIFParams neuron_params;
} SNNLayer;

// 突触连接
typedef struct {
    uint16_t pre_idx;       // 前神经元索引
    uint16_t post_idx;      // 后神经元索引
    float weight;
    float delay;
    SynapseParams params;
} Connection;

// SNN网络
typedef struct {
    // 神经元
    LIFNeuron neurons[MAX_NEURONS];
    uint16_t num_neurons;

    // 层
    SNNLayer layers[MAX_LAYERS];
    uint16_t num_layers;

    // 连接
    Connection connections[MAX_SYNAPSES];
    uint16_t num_connections;

    // 事件队列 (按时间排序的脉冲)
    typedef struct {
        uint64_t time_us;
        uint16_t neuron_idx;
    } SpikeEvent;

    SpikeEvent event_queue[1024];
    uint16_t queue_head;
    uint16_t queue_tail;

    // 统计
    uint64_t total_spikes;
    uint64_t simulation_time_us;
} SNNNetwork;

// 初始化网络
void snn_init(SNNNetwork *net) {
    memset(net, 0, sizeof(SNNNetwork));
    net->queue_head = 0;
    net->queue_tail = 0;
}

// 添加层
int snn_add_layer(SNNNetwork *net, LayerType type, uint16_t num_neurons,
                  const LIFParams *params) {
    if (net->num_layers >= MAX_LAYERS) return -1;
    if (net->num_neurons + num_neurons > MAX_NEURONS) return -1;

    SNNLayer *layer = &net->layers[net->num_layers++];
    layer->type = type;
    layer->start_idx = net->num_neurons;
    layer->num_neurons = num_neurons;
    layer->neuron_params = *params;

    // 初始化神经元
    for (int i = 0; i < num_neurons; i++) {
        lif_init(&net->neurons[net->num_neurons + i], params);
    }

    net->num_neurons += num_neurons;
    return layer->start_idx;
}

// 添加连接
int snn_connect(SNNNetwork *net, uint16_t pre_layer, uint16_t post_layer,
                float weight_mean, float weight_std) {
    SNNLayer *pre = &net->layers[pre_layer];
    SNNLayer *post = &net->layers[post_layer];

    for (int i = 0; i < pre->num_neurons; i++) {
        for (int j = 0; j < post->num_neurons; j++) {
            if (net->num_connections >= MAX_SYNAPSES) return -1;

            Connection *conn = &net->connections[net->num_connections++];
            conn->pre_idx = pre->start_idx + i;
            conn->post_idx = post->start_idx + j;
            conn->weight = weight_mean + ((float)rand() / RAND_MAX - 0.5f) * 2.0f * weight_std;
            conn->delay = 1.0f;
        }
    }

    return 0;
}

// 注入输入脉冲
void snn_input_spike(SNNNetwork *net, uint16_t neuron_idx, float weight) {
    lif_input_spike(&net->neurons[neuron_idx], weight);
}

// 网络单步更新
void snn_step(SNNNetwork *net, float dt_ms) {
    // 1. 更新所有神经元
    bool spiked[MAX_NEURONS] = {false};

    for (int i = 0; i < net->num_neurons; i++) {
        // 收集突触输入
        for (int c = 0; c < net->num_connections; c++) {
            if (net->connections[c].post_idx == i) {
                // 这里简化处理，实际应有突触延迟
                lif_input_spike(&net->neurons[i], net->connections[c].weight);
            }
        }

        // 更新神经元
        spiked[i] = lif_update(&net->neurons[i],
                               &net->layers[0].neuron_params, dt_ms);

        if (spiked[i]) {
            net->total_spikes++;
        }
    }

    // 2. 传播脉冲 (简化)
    for (int c = 0; c < net->num_connections; c++) {
        if (spiked[net->connections[c].pre_idx]) {
            // 脉冲传播到突触后神经元
            // 实际应有延迟队列
        }
    }
}
