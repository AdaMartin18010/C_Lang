/*
 * Auto-generated from: 11_Machine_Learning_C\03_Embedded_AI_Deployment.md
 * Line: 1310
 * Language: c
 * Block ID: be312dea
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file tflm_integration.cpp
 * @brief TensorFlow Lite Micro完整实现
 * @note 使用C++实现，但提供C接口
 */

#include "tflm_integration.h"
#include <cstring>

/* 使用C链接以兼容C代码 */
extern "C" {

/**
 * @brief 初始化TFLM运行时
 *
 * 初始化流程:
 * 1. 加载FlatBuffer模型
 * 2. 构建算子解析器
 * 3. 创建解释器
 * 4. 分配张量
 * 5. 验证输入输出
 */
int tflm_init(tflm_context_t* ctx, const tflm_config_t* config)
{
    if (!ctx || !config) {
        return -1;
    }

    /* 清零上下文 */
    memset(ctx, 0, sizeof(tflm_context_t));

    /* 1. 从FlatBuffer加载模型 */
    ctx->model = tflite::GetModel(config->model_data);

    /* 验证模型版本 */
    if (ctx->model->version() != TFLITE_SCHEMA_VERSION) {
        MicroPrintf("Model schema version mismatch!");
        return -2;
    }

    /* 2. 创建算子解析器 - 包含所有支持的操作 */
    /* 注意: 如果内存受限，可使用MicroMutableOpResolver替代 */
    ctx->resolver = new (std::nothrow) tflite::AllOpsResolver();
    if (!ctx->resolver) {
        MicroPrintf("Failed to allocate resolver!");
        return -3;
    }

    /* 3. 创建解释器 */
    ctx->interpreter = new (std::nothrow) tflite::MicroInterpreter(
        ctx->model,
        *ctx->resolver,
        config->tensor_arena,
        config->arena_size
    );

    if (!ctx->interpreter) {
        MicroPrintf("Failed to allocate interpreter!");
        delete ctx->resolver;
        return -4;
    }

    /* 4. 分配张量内存 */
    TfLiteStatus allocate_status = ctx->interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        MicroPrintf("Tensor allocation failed!");
        delete ctx->interpreter;
        delete ctx->resolver;
        return -5;
    }

    /* 5. 获取输入输出张量 */
    ctx->input_tensor = ctx->interpreter->input(0);
    ctx->output_tensor = ctx->interpreter->output(0);

    /* 6. 打印模型信息 */
    MicroPrintf("TFLM Initialized Successfully:");
    MicroPrintf("  Input shape: %dD", ctx->input_tensor->dims->size);
    for (int i = 0; i < ctx->input_tensor->dims->size; i++) {
        MicroPrintf("    Dim %d: %d", i, ctx->input_tensor->dims->data[i]);
    }
    MicroPrintf("  Input type: %s", TfLiteTypeGetName(ctx->input_tensor->type));

    ctx->is_initialized = 1;
    return 0;
}

/**
 * @brief 准备输入数据
 * @param ctx TFLM上下文
 * @param data 输入数据指针
 * @param size 数据大小(字节)
 */
int tflm_prepare_input(tflm_context_t* ctx, const void* data, size_t size)
{
    if (!ctx || !ctx->is_initialized || !data) {
        return -1;
    }

    /* 计算期望的输入大小 */
    size_t expected_size = ctx->input_tensor->bytes;

    if (size != expected_size) {
        MicroPrintf("Input size mismatch: expected %d, got %d",
                    expected_size, size);
        return -2;
    }

    /* 根据数据类型处理 */
    switch (ctx->input_tensor->type) {
        case kTfLiteFloat32: {
            /* FP32输入 - 直接拷贝 */
            memcpy(ctx->input_tensor->data.f, data, size);
            break;
        }

        case kTfLiteInt8: {
            /* INT8量化输入 - 确保数据正确量化 */
            memcpy(ctx->input_tensor->data.int8, data, size);
            break;
        }

        case kTfLiteUInt8: {
            /* UINT8量化输入 */
            memcpy(ctx->input_tensor->data.uint8, data, size);
            break;
        }

        case kTfLiteInt16: {
            memcpy(ctx->input_tensor->data.i16, data, size);
            break;
        }

        default:
            MicroPrintf("Unsupported input type: %s",
                        TfLiteTypeGetName(ctx->input_tensor->type));
            return -3;
    }

    return 0;
}

/**
 * @brief 执行推理
 * @param ctx TFLM上下文
 * @return 0成功，负值错误码
 */
int tflm_invoke(tflm_context_t* ctx)
{
    if (!ctx || !ctx->is_initialized) {
        return -1;
    }

    /* 记录开始时间 (如果使用profiling) */
    uint32_t start_time = 0;  /* 需要平台特定的计时实现 */

    /* 执行推理 */
    TfLiteStatus invoke_status = ctx->interpreter->Invoke();

    if (invoke_status != kTfLiteOk) {
        MicroPrintf("Inference failed!");
        return -2;
    }

    ctx->inference_count++;

    /* 计算耗时 */
    uint32_t end_time = 0;
    /* uint32_t elapsed = end_time - start_time; */

    return 0;
}

/**
 * @brief 获取输出数据
 * @param ctx TFLM上下文
 * @param buffer 输出缓冲区
 * @param size 缓冲区大小
 */
int tflm_get_output(tflm_context_t* ctx, void* buffer, size_t size)
{
    if (!ctx || !ctx->is_initialized || !buffer) {
        return -1;
    }

    size_t output_size = ctx->output_tensor->bytes;

    if (size < output_size) {
        MicroPrintf("Output buffer too small: need %d, got %d",
                    output_size, size);
        return -2;
    }

    /* 根据类型拷贝数据 */
    switch (ctx->output_tensor->type) {
        case kTfLiteFloat32:
            memcpy(buffer, ctx->output_tensor->data.f, output_size);
            break;

        case kTfLiteInt8:
            memcpy(buffer, ctx->output_tensor->data.int8, output_size);
            break;

        case kTfLiteUInt8:
            memcpy(buffer, ctx->output_tensor->data.uint8, output_size);
            break;

        case kTfLiteInt16:
            memcpy(buffer, ctx->output_tensor->data.i16, output_size);
            break;

        default:
            return -3;
    }

    return 0;
}

/**
 * @brief 重置推理器
 * 某些模型可能需要在推理间保持状态(RNN等)，此方法用于重置
 */
void tflm_reset(tflm_context_t* ctx)
{
    if (!ctx || !ctx->interpreter) {
        return;
    }

    /* 重置解释器状态 */
    ctx->interpreter->Reset();

    /* 重新分配张量 */
    ctx->interpreter->AllocateTensors();

    /* 更新指针 */
    ctx->input_tensor = ctx->interpreter->input(0);
    ctx->output_tensor = ctx->interpreter->output(0);
}

/**
 * @brief 释放TFLM资源
 */
void tflm_deinit(tflm_context_t* ctx)
{
    if (!ctx) {
        return;
    }

    /* 注意: 解释器和解析器使用placement new在arena上分配
     * 不需要显式delete，重置arena即可 */

    ctx->is_initialized = 0;
    ctx->inference_count = 0;
}

} /* extern "C" */
