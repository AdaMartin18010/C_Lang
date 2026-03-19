/*
 * Auto-generated from: 16_Embedded_AI_Agents\07_Hardware_Acceleration\01_NPU_TPU_C_Interfaces.md
 * Line: 249
 * Language: c
 * Block ID: ae2c5d9a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// coral_tpu_inference.c
#include "edgetpu.h"
#include "tensorflow/lite/c/c_api.h"

// Edge TPU上下文
typedef struct {
    EdgeTpuContext* tpu_context;      // TPU设备上下文
    TfLiteInterpreter* interpreter;   // TFLite解释器
    TfLiteModel* model;               // 模型
    TfLiteDelegate* delegate;         // TPU委托
} coral_ctx_t;

// 初始化Edge TPU
coral_ctx_t* coral_init(const char* model_path) {
    coral_ctx_t* ctx = calloc(1, sizeof(coral_ctx_t));

    // 获取Edge TPU设备
    ctx->tpu_context = edgetpu_create_device(NULL);
    if (!ctx->tpu_context) {
        fprintf(stderr, "Failed to create Edge TPU device\n");
        free(ctx);
        return NULL;
    }

    // 加载模型
    ctx->model = TfLiteModelCreateFromFile(model_path);
    if (!ctx->model) {
        fprintf(stderr, "Failed to load model\n");
        edgetpu_free_device(ctx->tpu_context);
        free(ctx);
        return NULL;
    }

    // 创建TPU委托
    ctx->delegate = edgetpu_create_delegate(ctx->tpu_context);
    if (!ctx->delegate) {
        fprintf(stderr, "Failed to create TPU delegate\n");
        TfLiteModelDelete(ctx->model);
        edgetpu_free_device(ctx->tpu_context);
        free(ctx);
        return NULL;
    }

    // 创建解释器选项
    TfLiteInterpreterOptions* options = TfLiteInterpreterOptionsCreate();
    TfLiteInterpreterOptionsAddDelegate(options, ctx->delegate);
    TfLiteInterpreterOptionsSetNumThreads(options, 1);  // Edge TPU不需要多线程

    // 创建解释器
    ctx->interpreter = TfLiteInterpreterCreate(ctx->model, options);
    TfLiteInterpreterOptionsDelete(options);

    if (!ctx->interpreter) {
        fprintf(stderr, "Failed to create interpreter\n");
        coral_deinit(ctx);
        return NULL;
    }

    // 分配张量
    if (TfLiteInterpreterAllocateTensors(ctx->interpreter) != kTfLiteOk) {
        fprintf(stderr, "Failed to allocate tensors\n");
        coral_deinit(ctx);
        return NULL;
    }

    printf("Edge TPU initialized\n");
    printf("  Model: %s\n", model_path);
    printf("  Input size: %zu bytes\n",
           TfLiteTensorByteSize(TfLiteInterpreterGetInputTensor(ctx->interpreter, 0)));

    return ctx;
}

// 执行推理
int coral_inference(coral_ctx_t* ctx,
                   const uint8_t* input_data,
                   size_t input_size,
                   float* output_data,
                   size_t output_size) {

    // 获取输入张量
    TfLiteTensor* input_tensor = TfLiteInterpreterGetInputTensor(ctx->interpreter, 0);

    // 复制输入数据
    if (TfLiteTensorCopyFromBuffer(input_tensor, input_data, input_size) != kTfLiteOk) {
        fprintf(stderr, "Failed to copy input\n");
        return -1;
    }

    // 执行推理 (大部分在TPU上执行)
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    if (TfLiteInterpreterInvoke(ctx->interpreter) != kTfLiteOk) {
        fprintf(stderr, "Inference failed\n");
        return -1;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    float inference_ms = (end.tv_sec - start.tv_sec) * 1000.0f +
                        (end.tv_nsec - start.tv_nsec) / 1000000.0f;

    printf("Inference time: %.2f ms\n", inference_ms);

    // 获取输出
    const TfLiteTensor* output_tensor = TfLiteInterpreterGetOutputTensor(ctx->interpreter, 0);

    if (TfLiteTensorCopyToBuffer(output_tensor, output_data, output_size) != kTfLiteOk) {
        fprintf(stderr, "Failed to copy output\n");
        return -1;
    }

    return 0;
}

// 清理资源
void coral_deinit(coral_ctx_t* ctx) {
    if (ctx) {
        if (ctx->interpreter) TfLiteInterpreterDelete(ctx->interpreter);
        if (ctx->model) TfLiteModelDelete(ctx->model);
        if (ctx->delegate) edgetpu_free_delegate(ctx->delegate);
        if (ctx->tpu_context) edgetpu_free_device(ctx->tpu_context);
        free(ctx);
    }
}

// 温度监控 (Edge TPU可能过热)
float coral_get_temperature(coral_ctx_t* ctx) {
    return edgetpu_get_temperature(ctx->tpu_context);
}

// 性能分析
void coral_profile(coral_ctx_t* ctx) {
    struct edgetpu_performance_stats stats;
    edgetpu_get_performance_stats(ctx->tpu_context, &stats);

    printf("Edge TPU Performance:\n");
    printf("  Active cycles: %lu\n", stats.active_cycles);
    printf("  Stalls: %lu\n", stats.stall_cycles);
    printf("  Temperature: %.1f°C\n", stats.temperature);
}
