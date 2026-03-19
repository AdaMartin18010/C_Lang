/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\02_STDP_Learning.md
 * Line: 401
 * Language: c
 * Block ID: d174edd1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// STDP特征学习器
// ============================================================================

#define INPUT_SIZE      16
#define FEATURE_SIZE    8

// 竞争学习层
typedef struct {
    STDPNetwork net;
    int input_neurons[INPUT_SIZE];
    int feature_neurons[FEATURE_SIZE];
    float inhibition_strength;
} STDPFeatureLearner;

// 初始化特征学习器
void feature_learner_init(STDPFeatureLearner *learner) {
    stdp_network_init(&learner->net);

    STDPParams params = STDP_CLASSIC;
    params.w_max = 0.3f;
    params.A_plus = 0.02f;
    params.A_minus = -0.018f;

    // 创建输入神经元
    for (int i = 0; i < INPUT_SIZE; i++) {
        learner->input_neurons[i] = stdp_add_neuron(&learner->net, -70.0f, -55.0f, 20.0f);
    }

    // 创建特征神经元 (WTA竞争)
    for (int i = 0; i < FEATURE_SIZE; i++) {
        learner->feature_neurons[i] = stdp_add_neuron(&learner->net, -70.0f, -50.0f, 20.0f);

        // 连接到所有输入
        for (int j = 0; j < INPUT_SIZE; j++) {
            float w = 0.05f + ((float)rand() / RAND_MAX) * 0.05f;
            stdp_connect(&learner->net, learner->input_neurons[j],
                        learner->feature_neurons[i], w, &params);
        }
    }

    // 特征神经元间抑制 (WTA)
    STDPParams inh_params = {0};
    inh_params.w_min = -0.5f;
    inh_params.w_max = 0.0f;

    for (int i = 0; i < FEATURE_SIZE; i++) {
        for (int j = 0; j < FEATURE_SIZE; j++) {
            if (i != j) {
                stdp_connect(&learner->net, learner->feature_neurons[i],
                            learner->feature_neurons[j], -0.2f, &inh_params);
            }
        }
    }
}

// 训练步骤
void feature_learner_train(STDPFeatureLearner *learner,
                           const float *input_pattern, float dt_ms) {
    // 将输入模式转换为脉冲
    for (int i = 0; i < INPUT_SIZE; i++) {
        // 输入强度 -> 脉冲概率
        float spike_prob = input_pattern[i];
        if ((float)rand() / RAND_MAX < spike_prob) {
            STDPNeuron *n = &learner->net.neurons[learner->input_neurons[i]];
            n->v_mem = n->v_thresh + 1.0f;  // 强制发放
        }
    }

    // 网络更新
    stdp_network_step(&learner->net, dt_ms);
}

// 测试步骤
int feature_learner_infer(STDPFeatureLearner *learner,
                          const float *input_pattern, float dt_ms) {
    bool old_learning = learner->net.learning_enabled;
    learner->net.learning_enabled = false;

    // 重置脉冲计数
    for (int i = 0; i < FEATURE_SIZE; i++) {
        learner->net.neurons[learner->feature_neurons[i]].spike_count = 0;
    }

    // 运行一段时间
    for (int step = 0; step < 50; step++) {
        for (int i = 0; i < INPUT_SIZE; i++) {
            float spike_prob = input_pattern[i] * 0.3f;
            if ((float)rand() / RAND_MAX < spike_prob) {
                STDPNeuron *n = &learner->net.neurons[learner->input_neurons[i]];
                n->v_mem = n->v_thresh + 1.0f;
            }
        }
        stdp_network_step(&learner->net, dt_ms);
    }

    // 找发放最多的特征神经元
    int winner = 0;
    uint32_t max_spikes = 0;
    for (int i = 0; i < FEATURE_SIZE; i++) {
        uint32_t spikes = learner->net.neurons[learner->feature_neurons[i]].spike_count;
        if (spikes > max_spikes) {
            max_spikes = spikes;
            winner = i;
        }
    }

    learner->net.learning_enabled = old_learning;
    return winner;
}
