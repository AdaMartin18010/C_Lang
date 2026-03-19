/*
 * Auto-generated from: 16_Embedded_AI_Agents\07_Hardware_Acceleration\01_NPU_TPU_C_Interfaces.md
 * Line: 397
 * Language: c
 * Block ID: fb9a79ff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// jetson_tensorrt_inference.c
#include "NvInfer.h"
#include "cuda_runtime_api.h"

using namespace nvinfer1;

class Logger : public ILogger {
    void log(Severity severity, const char* msg) noexcept override {
        if (severity <= Severity::kWARNING) {
            printf("[TensorRT] %s\n", msg);
        }
    }
} gLogger;

typedef struct {
    IRuntime* runtime;
    ICudaEngine* engine;
    IExecutionContext* context;
    void* device_input;
    void* device_output;
    float* host_input;
    float* host_output;
    size_t input_size;
    size_t output_size;
    cudaStream_t stream;
} jetson_ctx_t;

// 初始化TensorRT
jetson_ctx_t* jetson_init(const char* engine_file) {
    jetson_ctx_t* ctx = calloc(1, sizeof(jetson_ctx_t));

    // 读取引擎文件
    FILE* fp = fopen(engine_file, "rb");
    if (!fp) {
        fprintf(stderr, "Failed to open %s\n", engine_file);
        free(ctx);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    size_t engine_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* engine_data = malloc(engine_size);
    fread(engine_data, 1, engine_size, fp);
    fclose(fp);

    // 创建运行时
    ctx->runtime = createInferRuntime(gLogger);

    // 反序列化引擎
    ctx->engine = ctx->runtime->deserializeCudaEngine(engine_data, engine_size);
    free(engine_data);

    if (!ctx->engine) {
        fprintf(stderr, "Failed to deserialize engine\n");
        free(ctx);
        return NULL;
    }

    // 创建执行上下文
    ctx->context = ctx->engine->createExecutionContext();

    // 获取I/O大小
    Dims input_dims = ctx->engine->getBindingDimensions(0);
    Dims output_dims = ctx->engine->getBindingDimensions(1);

    ctx->input_size = 1;
    for (int i = 0; i < input_dims.nbDims; i++) {
        ctx->input_size *= input_dims.d[i];
    }
    ctx->input_size *= sizeof(float);

    ctx->output_size = 1;
    for (int i = 0; i < output_dims.nbDims; i++) {
        ctx->output_size *= output_dims.d[i];
    }
    ctx->output_size *= sizeof(float);

    // 分配GPU内存
    cudaMalloc(&ctx->device_input, ctx->input_size);
    cudaMalloc(&ctx->device_output, ctx->output_size);

    // 分配CPU内存
    cudaMallocHost(&ctx->host_input, ctx->input_size);
    cudaMallocHost(&ctx->host_output, ctx->output_size);

    // 创建CUDA流
    cudaStreamCreate(&ctx->stream);

    printf("TensorRT initialized\n");
    printf("  Input: %zu bytes\n", ctx->input_size);
    printf("  Output: %zu bytes\n", ctx->output_size);

    return ctx;
}

// 执行推理
int jetson_inference(jetson_ctx_t* ctx, float* output) {
    // 异步拷贝输入 H2D
    cudaMemcpyAsync(ctx->device_input, ctx->host_input,
                   ctx->input_size, cudaMemcpyHostToDevice, ctx->stream);

    // 设置张量地址
    void* bindings[] = {ctx->device_input, ctx->device_output};

    // 执行推理
    ctx->context->enqueueV2(bindings, ctx->stream, nullptr);

    // 异步拷贝输出 D2H
    cudaMemcpyAsync(ctx->host_output, ctx->device_output,
                   ctx->output_size, cudaMemcpyDeviceToHost, ctx->stream);

    // 同步等待完成
    cudaStreamSynchronize(ctx->stream);

    // 复制结果
    memcpy(output, ctx->host_output, ctx->output_size);

    return 0;
}
