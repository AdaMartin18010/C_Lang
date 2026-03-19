/*
 * Auto-generated from: 16_Embedded_AI_Agents\01_TinyML_Fundamentals\01_Introduction_to_TinyML.md
 * Line: 212
 * Language: c
 * Block ID: 3bc4d3a1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// main.c: TinyML推理示例
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "model_data.h"  // 生成的模型数组

// 内存配置
constexpr int kTensorArenaSize = 10 * 1024;  // 10KB张量区域
alignas(16) uint8_t tensor_arena[kTensorArenaSize];

// 模型输入/输出维度
constexpr int kInputSize = 1960;   // 49帧 x 40梅尔频带
constexpr int kOutputSize = 4;      // 4个关键词类别

void setup_tflm() {
    // 加载模型
    const tflite::Model* model = tflite::GetModel(g_keyword_model_data);

    // 验证模型版本
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        MicroPrintf("Model version mismatch!");
        return;
    }

    // 配置操作解析器 (仅包含需要的操作)
    static tflite::MicroMutableOpResolver<5> resolver;
    resolver.AddDepthwiseConv2D();
    resolver.AddConv2D();
    resolver.AddMaxPool2D();
    resolver.AddFullyConnected();
    resolver.AddSoftmax();

    // 创建解释器
    static tflite::MicroInterpreter interpreter(
        model, resolver, tensor_arena, kTensorArenaSize);

    TfLiteStatus allocate_status = interpreter.AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        MicroPrintf("AllocateTensors() failed");
        return;
    }

    MicroPrintf("TinyML model loaded successfully!");
}

int run_inference(const float* audio_features) {
    // 获取输入张量
    TfLiteTensor* input = interpreter.input(0);

    // 复制输入数据 (量化到INT8)
    for (int i = 0; i < kInputSize; i++) {
        input->data.int8[i] = static_cast<int8_t>(
            (audio_features[i] / input->params.scale) + input->params.zero_point
        );
    }

    // 运行推理
    TfLiteStatus invoke_status = interpreter.Invoke();
    if (invoke_status != kTfLiteOk) {
        MicroPrintf("Invoke failed");
        return -1;
    }

    // 获取输出
    TfLiteTensor* output = interpreter.output(0);

    // 找到最大概率的类别
    int max_index = 0;
    int8_t max_value = output->data.int8[0];
    for (int i = 1; i < kOutputSize; i++) {
        if (output->data.int8[i] > max_value) {
            max_value = output->data.int8[i];
            max_index = i;
        }
    }

    // 反量化得到概率
    float probability = (max_value - output->params.zero_point) * output->params.scale;

    if (probability > 0.8f) {
        return max_index;  // 检测到关键词
    }

    return -1;  // 无关键词
}

int main() {
    setup_tflm();

    while (1) {
        // 从麦克风获取音频特征
        float audio_features[kInputSize];
        extract_mel_spectrogram(audio_features);

        // 运行推理
        int keyword = run_inference(audio_features);

        if (keyword >= 0) {
            const char* keywords[] = {"Yes", "No", "Up", "Down"};
            printf("Detected: %s\n", keywords[keyword]);

            // 触发相应动作
            execute_command(keyword);
        }

        delay_ms(100);  // 100ms检测间隔
    }

    return 0;
}
