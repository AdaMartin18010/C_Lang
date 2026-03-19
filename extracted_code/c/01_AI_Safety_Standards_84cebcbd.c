/*
 * Auto-generated from: 16_Embedded_AI_Agents\09_Security_Reliability\01_AI_Safety_Standards.md
 * Line: 367
 * Language: c
 * Block ID: 84cebcbd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 基于抽象解释的神经网络验证

// ReLU的区间传播
interval_t relu_interval(interval_t x) {
    return (interval_t){
        .lower = (x.lower > 0) ? x.lower : 0,
        .upper = (x.upper > 0) ? x.upper : 0
    };
}

// 线性层的区间传播
void linear_interval_propagation(const interval_t* inputs,
                                int num_inputs,
                                const float* weights,
                                const float* bias,
                                int num_outputs,
                                interval_t* outputs) {
    for (int i = 0; i < num_outputs; i++) {
        float lower = bias[i];
        float upper = bias[i];

        for (int j = 0; j < num_inputs; j++) {
            float w = weights[i * num_inputs + j];

            if (w >= 0) {
                lower += w * inputs[j].lower;
                upper += w * inputs[j].upper;
            } else {
                lower += w * inputs[j].upper;  // 负权重反转边界
                upper += w * inputs[j].lower;
            }
        }

        outputs[i] = (interval_t){lower, upper};
    }
}

// 验证输出范围
bool verify_output_range(const interval_t* output_intervals,
                        int num_outputs,
                        int expected_class,
                        float margin) {
    float expected_lower = output_intervals[expected_class].lower;

    for (int i = 0; i < num_outputs; i++) {
        if (i == expected_class) continue;

        // 检查预期类的下界是否大于其他类的上界+margin
        if (expected_lower <= output_intervals[i].upper + margin) {
            return false;  // 无法保证
        }
    }
    return true;
}

// 完整验证流程
verification_result_t verify_network_safety(
    const float* input,
    int input_size,
    float epsilon,
    int expected_class,
    const neural_network_t* network) {

    // 1. 构建输入区间
    interval_t input_intervals[input_size];
    for (int i = 0; i < input_size; i++) {
        input_intervals[i] = (interval_t){
            input[i] - epsilon,
            input[i] + epsilon
        };
    }

    // 2. 逐层区间传播
    interval_t current_intervals[256];  // 假设最大层大小
    memcpy(current_intervals, input_intervals,
           input_size * sizeof(interval_t));

    int current_size = input_size;

    for (int layer = 0; layer < network->num_layers; layer++) {
        interval_t next_intervals[256];

        // 线性变换
        linear_interval_propagation(
            current_intervals, current_size,
            network->layers[layer].weights,
            network->layers[layer].bias,
            network->layers[layer].num_outputs,
            next_intervals
        );

        // 激活函数
        if (network->layers[layer].activation == ACT_RELU) {
            for (int i = 0; i < network->layers[layer].num_outputs; i++) {
                next_intervals[i] = relu_interval(next_intervals[i]);
            }
        }

        memcpy(current_intervals, next_intervals,
               network->layers[layer].num_outputs * sizeof(interval_t));
        current_size = network->layers[layer].num_outputs;
    }

    // 3. 验证输出
    verification_result_t result = {0};
    result.verified = verify_output_range(
        current_intervals, current_size, expected_class, 0.1f
    );
    result.confidence = 1.0f;  // 抽象解释给出确定保证

    if (!result.verified) {
        snprintf(result.counter_example, sizeof(result.counter_example),
                "Output intervals overlap for class %d", expected_class);
    }

    return result;
}
