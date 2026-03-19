/*
 * Auto-generated from: 16_Embedded_AI_Agents\06_C_Frameworks\01_CMSIS_NN.md
 * Line: 528
 * Language: c
 * Block ID: 9ec58661
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 将CMSIS-NN作为TFLM的后端

// tflm_cmsis_nn_delegate.c
#include "tensorflow/lite/micro/micro_allocator.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/kernels/cmsis-nn/add.h"
#include "tensorflow/lite/micro/kernels/cmsis-nn/conv.h"
#include "tensorflow/lite/micro/kernels/cmsis-nn/depthwise_conv.h"
#include "tensorflow/lite/micro/kernels/cmsis-nn/fully_connected.h"

// 注册CMSIS-NN内核
void register_cmsis_nn_kernels(tflite::MicroMutableOpResolver* resolver) {
    resolver->AddConv2D(arm_convolve_s8);
    resolver->AddDepthwiseConv2D(arm_depthwise_conv_s8);
    resolver->AddFullyConnected(arm_fully_connected_s8);
    resolver->AddAdd(arm_elementwise_add_s8);
    resolver->AddAveragePool2D(arm_avgpool_s8);
    resolver->AddMaxPool2D(arm_max_pool_s8);
    resolver->AddSoftmax(arm_softmax_s8);
}

// 完整TFLM + CMSIS-NN推理
int tflm_cmsis_inference(const uint8_t* model_data,
                        const void* input_data,
                        void* output_data,
                        size_t input_size,
                        size_t output_size) {

    // 张量区域
    constexpr int kTensorArenaSize = 20 * 1024;
    uint8_t tensor_arena[kTensorArenaSize];

    // 加载模型
    const tflite::Model* model = tflite::GetModel(model_data);

    // 使用CMSIS-NN优化的解析器
    static tflite::MicroMutableOpResolver<10> resolver;
    register_cmsis_nn_kernels(&resolver);

    // 创建解释器
    tflite::MicroInterpreter interpreter(
        model, resolver, tensor_arena, kTensorArenaSize);

    TfLiteStatus allocate_status = interpreter.AllocateTensors();
    if (allocate_status != kTfLiteOk) return -1;

    // 复制输入
    TfLiteTensor* input = interpreter.input(0);
    memcpy(input->data.int8, input_data, input_size);

    // 推理 (使用CMSIS-NN优化内核)
    TfLiteStatus invoke_status = interpreter.Invoke();
    if (invoke_status != kTfLiteOk) return -1;

    // 复制输出
    TfLiteTensor* output = interpreter.output(0);
    memcpy(output_data, output->data.int8, output_size);

    return 0;
}
