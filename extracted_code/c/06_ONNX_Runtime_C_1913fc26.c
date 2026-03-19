/*
 * Auto-generated from: 11_Machine_Learning_C\06_ONNX_Runtime_C.md
 * Line: 341
 * Language: c
 * Block ID: 1913fc26
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#include "onnx_inference.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <model.onnx>\n", argv[0]);
        return 1;
    }

    // 初始化
    onnx_init();

    // 创建运行时
    ONNX_Runtime* rt = onnx_create_runtime(argv[1]);

    // 打印模型信息
    print_model_info(rt);

    // 准备输入 (1x1x28x28 MNIST图像)
    float input_data[1 * 1 * 28 * 28];
    // 填充数据 (例如从文件读取图像)
    for (int i = 0; i < 784; i++) {
        input_data[i] = 0.5f;  // 示例数据
    }

    int64_t input_dims[] = {1, 1, 28, 28};
    OrtValue* input_tensor = create_input_tensor(rt, input_data,
                                                   input_dims, 4);

    // 执行推理
    OrtValue* inputs[] = {input_tensor};
    Tensor_Output* output = onnx_inference(rt, inputs, 1);

    // 处理输出 (10类概率)
    printf("Output probabilities:\n");
    for (int i = 0; i < 10; i++) {
        printf("  Digit %d: %.4f\n", i, output->data[i]);
    }

    // 找出最大概率
    int max_idx = 0;
    float max_prob = output->data[0];
    for (int i = 1; i < 10; i++) {
        if (output->data[i] > max_prob) {
            max_prob = output->data[i];
            max_idx = i;
        }
    }
    printf("Predicted digit: %d (confidence: %.2f%%)\n",
           max_idx, max_prob * 100);

    // 清理
    free_tensor_output(output);
    g_ort->ReleaseValue(input_tensor);
    onnx_destroy_runtime(rt);

    return 0;
}
