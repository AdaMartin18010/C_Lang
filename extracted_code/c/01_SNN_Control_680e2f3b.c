/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\01_SNN_Control.md
 * Line: 465
 * Language: c
 * Block ID: 680e2f3b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// SNN机器人控制器
// 传感器输入 -> SNN -> 电机输出
// ============================================================================

// 传感器接口
typedef struct {
    float distance_left;    // 左侧距离传感器 (m)
    float distance_right;   // 右侧距离传感器
    float distance_front;   // 前方距离传感器
    float light_level;      // 光强
} SensorInput;

// 电机输出
typedef struct {
    float left_motor;       // -1.0 到 1.0
    float right_motor;      // -1.0 到 1.0
} MotorOutput;

// SNN控制器
typedef struct {
    SNNNetwork net;
    float input_min[4];
    float input_max[4];
    uint16_t input_neurons[4];  // 每传感器的神经元起始索引
    uint16_t output_neurons[2]; // 输出神经元索引
    uint32_t step_count;
} SNNController;

// 初始化避障控制器
void controller_init_obstacle_avoidance(SNNController *ctrl) {
    snn_init(&ctrl->net);

    // 输入层: 4个传感器，每个用4个神经元编码
    LIFParams input_params = LIF_DEFAULT;
    input_params.v_thresh = -50.0f;  // 更易激发

    for (int i = 0; i < 4; i++) {
        ctrl->input_neurons[i] = snn_add_layer(&ctrl->net, LAYER_INPUT, 4, &input_params);
    }

    // 隐藏层
    LIFParams hidden_params = LIF_DEFAULT;
    uint16_t hidden = snn_add_layer(&ctrl->net, LAYER_HIDDEN, 16, &hidden_params);

    // 输出层: 2个电机
    LIFParams output_params = LIF_DEFAULT;
    output_params.v_thresh = -45.0f;
    uint16_t output = snn_add_layer(&ctrl->net, LAYER_OUTPUT, 2, &output_params);

    // 连接输入到隐藏层
    for (int i = 0; i < 4; i++) {
        snn_connect(&ctrl->net, i, hidden / 4, 0.5f, 0.2f);
    }

    // 连接隐藏层到输出层
    snn_connect(&ctrl->net, hidden / 4, output / 2, 0.8f, 0.3f);

    // 设置输入范围
    float defaults_min[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float defaults_max[4] = {2.0f, 2.0f, 2.0f, 1000.0f};
    memcpy(ctrl->input_min, defaults_min, sizeof(defaults_min));
    memcpy(ctrl->input_max, defaults_max, sizeof(defaults_max));
}

// 运行控制器
void controller_step(SNNController *ctrl, const SensorInput *sensors,
                     MotorOutput *motors, float dt_ms) {

    // 1. 编码传感器输入为脉冲
    float sensor_values[4] = {
        sensors->distance_left,
        sensors->distance_right,
        sensors->distance_front,
        sensors->light_level
    };

    for (int s = 0; s < 4; s++) {
        float rate = rate_encode(sensor_values[s],
                                  ctrl->input_min[s], ctrl->input_max[s],
                                  0.0f, 100.0f);  // 0-100 Hz

        // 分配到群体神经元
        float rates[4];
        population_encode(sensor_values[s], rates, 4,
                         ctrl->input_min[s], ctrl->input_max[s]);

        for (int n = 0; n < 4; n++) {
            // 以概率注入脉冲
            float spike_prob = rates[n] * dt_ms / 1000.0f;
            if ((float)rand() / RAND_MAX < spike_prob) {
                snn_input_spike(&ctrl->net, ctrl->input_neurons[s] + n, 1.0f);
            }
        }
    }

    // 2. 更新SNN
    snn_step(&ctrl->net, dt_ms);

    // 3. 解码输出
    SNNLayer *output_layer = NULL;
    for (int l = 0; l < ctrl->net.num_layers; l++) {
        if (ctrl->net.layers[l].type == LAYER_OUTPUT) {
            output_layer = &ctrl->net.layers[l];
            break;
        }
    }

    if (output_layer) {
        // 基于脉冲计数解码
        uint32_t left_spikes = ctrl->net.neurons[output_layer->start_idx].spike_count;
        uint32_t right_spikes = ctrl->net.neurons[output_layer->start_idx + 1].spike_count;

        // 转换为电机输出 (-1 到 1)
        motors->left_motor = fminf(1.0f, left_spikes / 5.0f) * 2.0f - 1.0f;
        motors->right_motor = fminf(1.0f, right_spikes / 5.0f) * 2.0f - 1.0f;
    }

    ctrl->step_count++;
}
