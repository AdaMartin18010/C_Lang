/*
 * Auto-generated from: 16_Embedded_AI_Agents\06_C_Frameworks\01_CMSIS_NN.md
 * Line: 351
 * Language: c
 * Block ID: f9100fdc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// mnist_cmsis_nn.c
#include "arm_nnfunctions.h"
#include "model_weights.h"  // 模型权重 (从TensorFlow导出)

#define INPUT_SIZE (28 * 28)
#define HIDDEN_SIZE 128
#define OUTPUT_SIZE 10

// 缓冲区
static q7_t input_buffer[INPUT_SIZE];
static q7_t hidden_buffer[HIDDEN_SIZE];
static q7_t output_buffer[OUTPUT_SIZE];

// 临时缓冲区 (用于中间计算)
#define SCRATCH_BUFFER_SIZE (2 * 28 * 28)  // 2 * input_size for q15
static q15_t scratch_buffer[SCRATCH_BUFFER_SIZE];

// 量化参数 (从训练获得)
static const int32_t conv1_quant_params[2] = {1877991215, -3};  // multiplier, shift
static const int32_t fc1_quant_params[2] = {1531234567, -2};
static const int32_t fc2_quant_params[2] = {1987654321, -4};

int mnist_inference(const uint8_t* input_image, float* output_probs) {
    // 1. 预处理: 归一化并量化为int8
    // input: uint8 [0, 255] -> int8 [-128, 127]
    for (int i = 0; i < INPUT_SIZE; i++) {
        input_buffer[i] = (q7_t)((int)input_image[i] - 128);
    }

    // 2. 第一层: Conv2D (1x28x28 -> 32x26x26)
    // 为简化示例，这里假设已经展开了卷积
    // 实际使用 arm_convolve_s8

    // 3. MaxPooling (26x26 -> 13x13)
    cmsis_nn_pool_params pool_params = {
        .stride = {2, 2},
        .padding = {0, 0, 0, 0},
        .activation = {-128, 127}
    };
    cmsis_nn_dims pool_input_dims = {32, 26, 26, 1};  // 示例
    cmsis_nn_dims pool_filter_dims = {1, 2, 2, 1};
    cmsis_nn_dims pool_output_dims = {32, 13, 13, 1};

    arm_max_pool_s8(
        &pool_params,
        &pool_input_dims,
        hidden_buffer,  // 输入
        &pool_filter_dims,
        &pool_output_dims,
        hidden_buffer   // 输出 (原地)
    );

    // 4. Flatten + FC1 (5408 -> 128)
    cmsis_nn_fc_params fc1_params = {
        .input_offset = 128,
        .output_offset = -128,
        .filter_offset = 0,
        .activation = {-128, 127}
    };
    // ... 调用 arm_fully_connected_s8

    // 5. ReLU激活 (通过activation参数自动处理)

    // 6. FC2 (128 -> 10)
    cmsis_nn_fc_params fc2_params = {
        .input_offset = 128,
        .output_offset = 0,  // Softmax前不需要偏移
        .filter_offset = 0,
        .activation = {-128, 127}
    };
    // ... 调用 arm_fully_connected_s8

    // 7. Softmax (转换为概率)
    arm_softmax_s8(output_buffer, 1, 10, 1, 1, output_probs);

    // 找到最大值
    int max_idx = 0;
    float max_prob = output_probs[0];
    for (int i = 1; i < OUTPUT_SIZE; i++) {
        if (output_probs[i] > max_prob) {
            max_prob = output_probs[i];
            max_idx = i;
        }
    }

    return max_idx;  // 返回预测的数字
}
