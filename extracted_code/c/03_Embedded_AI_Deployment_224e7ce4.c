/*
 * Auto-generated from: 11_Machine_Learning_C\03_Embedded_AI_Deployment.md
 * Line: 154
 * Language: c
 * Block ID: 224e7ce4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file neural_network_inference.c
 * @brief 神经网络推理通用流程框架
 * @note 适用于任何嵌入式神经网络推理引擎
 */

#include <string.h>
#include <stdlib.h>
#include "embedded_ai_types.h"

/* ========== 推理上下文结构 ========== */
typedef struct {
    /* 模型相关 */
    const void* model_data;           /* 模型数据指针 */
    uint32_t model_size;              /* 模型大小 */

    /* 内存管理 */
    uint8_t* tensor_arena;            /* 张量内存池 */
    uint32_t arena_size;              /* 内存池大小 */
    uint32_t arena_used;              /* 已使用内存 */

    /* 输入输出 */
    void* input_buffer;               /* 输入缓冲区 */
    void* output_buffer;              /* 输出缓冲区 */

    /* 运行时状态 */
    volatile uint8_t is_initialized;  /* 初始化标志 */
    volatile uint8_t is_busy;         /* 忙标志 */
    uint32_t inference_count;         /* 推理计数 */
    uint32_t total_time_us;           /* 累计推理时间 */
} nn_inference_context_t;

/* ========== 推理错误码 ========== */
typedef enum {
    NN_OK = 0,
    NN_ERROR_INVALID_PARAM = -1,      /* 参数错误 */
    NN_ERROR_MEMORY_ALLOC = -2,       /* 内存分配失败 */
    NN_ERROR_MODEL_LOAD = -3,         /* 模型加载失败 */
    NN_ERROR_INFERENCE = -4,          /* 推理错误 */
    NN_ERROR_UNSUPPORTED_OP = -5,     /* 不支持的算子 */
    NN_ERROR_BUSY = -6,               /* 推理忙 */
} nn_error_t;

/* ========== 推理回调函数类型 ========== */
typedef void (*nn_preprocessing_fn)(const void* raw_data,
                                     void* input_buffer,
                                     uint32_t input_size);
typedef void (*nn_postprocessing_fn)(const void* output_buffer,
                                      void* results,
                                      uint32_t output_size);

/* ========== 初始化函数 ========== */
/**
 * @brief 初始化神经网络推理上下文
 * @param ctx 推理上下文
 * @param model_data 模型数据指针
 * @param model_size 模型大小
 * @param arena 张量内存池
 * @param arena_size 内存池大小
 * @return NN_OK成功，否则返回错误码
 */
int nn_inference_init(nn_inference_context_t* ctx,
                      const void* model_data,
                      uint32_t model_size,
                      uint8_t* arena,
                      uint32_t arena_size)
{
    if (!ctx || !model_data || !arena) {
        return NN_ERROR_INVALID_PARAM;
    }

    /* 清零上下文 */
    memset(ctx, 0, sizeof(nn_inference_context_t));

    /* 设置模型数据 */
    ctx->model_data = model_data;
    ctx->model_size = model_size;

    /* 设置内存池 */
    ctx->tensor_arena = arena;
    ctx->arena_size = arena_size;
    ctx->arena_used = 0;

    /* 标记已初始化 */
    ctx->is_initialized = 1;

    return NN_OK;
}

/* ========== 推理主循环 ========== */
/**
 * @brief 单次推理完整流程
 * @param ctx 推理上下文
 * @param raw_input 原始输入数据
 * @param results 输出结果缓冲区
 * @param preprocess 预处理函数
 * @param postprocess 后处理函数
 * @return NN_OK成功，否则返回错误码
 */
int nn_inference_run(nn_inference_context_t* ctx,
                     const void* raw_input,
                     void* results,
                     nn_preprocessing_fn preprocess,
                     nn_postprocessing_fn postprocess)
{
    if (!ctx || !ctx->is_initialized) {
        return NN_ERROR_INVALID_PARAM;
    }

    /* 检查忙状态 */
    if (ctx->is_busy) {
        return NN_ERROR_BUSY;
    }

    ctx->is_busy = 1;

    /* 1. 预处理: 原始数据 -> 模型输入 */
    if (preprocess && ctx->input_buffer) {
        preprocess(raw_input, ctx->input_buffer, 0);
    }

    /* 2. 神经网络推理 (由具体实现填充) */
    /* int ret = platform_specific_inference(ctx); */

    /* 3. 后处理: 模型输出 -> 结构化结果 */
    if (postprocess && ctx->output_buffer) {
        postprocess(ctx->output_buffer, results, 0);
    }

    /* 更新统计 */
    ctx->inference_count++;

    ctx->is_busy = 0;

    return NN_OK;
}

/* ========== 内存分配器 ========== */
/**
 * @brief 从张量内存池中分配内存
 * @param ctx 推理上下文
 * @param size 请求大小
 * @param alignment 对齐要求
 * @return 分配的内存指针，失败返回NULL
 */
void* nn_arena_alloc(nn_inference_context_t* ctx,
                     uint32_t size,
                     uint32_t alignment)
{
    if (!ctx || !ctx->tensor_arena) {
        return NULL;
    }

    /* 计算对齐后的地址 */
    uint32_t aligned_used = (ctx->arena_used + alignment - 1) & ~(alignment - 1);

    /* 检查内存是否足够 */
    if (aligned_used + size > ctx->arena_size) {
        return NULL;  /* 内存不足 */
    }

    void* ptr = ctx->tensor_arena + aligned_used;
    ctx->arena_used = aligned_used + size;

    return ptr;
}

/**
 * @brief 重置内存池
 * @param ctx 推理上下文
 */
void nn_arena_reset(nn_inference_context_t* ctx)
{
    if (ctx) {
        ctx->arena_used = 0;
    }
}

/* ========== 性能监控 ========== */
typedef struct {
    uint32_t min_time_us;
    uint32_t max_time_us;
    uint32_t avg_time_us;
    uint32_t total_inferences;
} nn_performance_stats_t;

void nn_get_performance_stats(const nn_inference_context_t* ctx,
                              nn_performance_stats_t* stats)
{
    if (!ctx || !stats) return;

    stats->total_inferences = ctx->inference_count;
    if (ctx->inference_count > 0) {
        stats->avg_time_us = ctx->total_time_us / ctx->inference_count;
    }
}
