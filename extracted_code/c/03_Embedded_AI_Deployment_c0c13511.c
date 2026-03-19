/*
 * Auto-generated from: 11_Machine_Learning_C\03_Embedded_AI_Deployment.md
 * Line: 1231
 * Language: c
 * Block ID: c0c13511
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file tflm_integration.h
 * @brief TensorFlow Lite Micro集成头文件
 */

#ifndef TFLM_INTEGRATION_H
#define TFLM_INTEGRATION_H

#include <stdint.h>
#include <stddef.h>

/* TFLM包含路径 - 根据实际安装调整 */
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_utils.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/c/common.h"

/* ========== TFLM配置结构 ========== */
typedef struct {
    /* 模型数据 */
    const uint8_t* model_data;
    uint32_t model_size;

    /* 张量内存池 */
    uint8_t* tensor_arena;
    uint32_t arena_size;

    /* 调试选项 */
    uint8_t enable_profiling;
    uint8_t enable_verbose_logging;
} tflm_config_t;

/* ========== TFLM上下文 ========== */
typedef struct {
    const tflite::Model* model;
    tflite::MicroInterpreter* interpreter;
    tflite::AllOpsResolver* resolver;
    TfLiteTensor* input_tensor;
    TfLiteTensor* output_tensor;

    uint8_t is_initialized;
    uint32_t inference_count;
} tflm_context_t;

/* ========== API函数 ========== */

/**
 * @brief 初始化TFLM运行时
 */
int tflm_init(tflm_context_t* ctx, const tflm_config_t* config);

/**
 * @brief 准备输入数据
 */
int tflm_prepare_input(tflm_context_t* ctx, const void* data, size_t size);

/**
 * @brief 执行推理
 */
int tflm_invoke(tflm_context_t* ctx);

/**
 * @brief 获取输出数据
 */
int tflm_get_output(tflm_context_t* ctx, void* buffer, size_t size);

/**
 * @brief 重置推理器状态
 */
void tflm_reset(tflm_context_t* ctx);

#endif /* TFLM_INTEGRATION_H */
