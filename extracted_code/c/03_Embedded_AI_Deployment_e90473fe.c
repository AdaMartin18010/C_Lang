/*
 * Auto-generated from: 11_Machine_Learning_C\03_Embedded_AI_Deployment.md
 * Line: 1967
 * Language: c
 * Block ID: e90473fe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file onnx_runtime_embedded.h
 * @brief ONNX Runtime嵌入式推理头文件
 */

#ifndef ONNX_RUNTIME_EMBEDDED_H
#define ONNX_RUNTIME_EMBEDDED_H

#include <stdint.h>
#include <stddef.h>

/* ========== ONNX Runtime错误码 ========== */
typedef enum {
    ORT_OK = 0,
    ORT_ERROR_INVALID_ARGUMENT = -1,
    ORT_ERROR_NO_SUCHFILE = -2,
    ORT_ERROR_NO_MODEL = -3,
    ORT_ERROR_ENGINE_ERROR = -4,
    ORT_ERROR_RUNTIME_EXCEPTION = -5,
    ORT_ERROR_INVALID_PROTOBUF = -6,
    ORT_ERROR_MODEL_LOADED = -7,
    ORT_ERROR_NOT_IMPLEMENTED = -8,
    ORT_ERROR_INVALID_GRAPH = -9,
    ORT_ERROR_EP_FAIL = -10,
} OrtErrorCode;

/* ========== 数据类型枚举 ========== */
typedef enum {
    ONNX_TENSOR_ELEMENT_DATA_TYPE_UNDEFINED = 0,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT = 1,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8 = 2,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_INT8 = 3,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT16 = 4,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_INT16 = 5,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32 = 6,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64 = 7,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_STRING = 8,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_BOOL = 9,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16 = 10,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_DOUBLE = 11,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT32 = 12,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT64 = 13,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_COMPLEX64 = 14,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_COMPLEX128 = 15,
    ONNX_TENSOR_ELEMENT_DATA_TYPE_BFLOAT16 = 16,
} ONNXTensorElementDataType;

/* ========== 会话配置 ========== */
typedef struct {
    /* 线程设置 */
    int intra_op_num_threads;
    int inter_op_num_threads;

    /* 内存优化 */
    uint8_t enable_mem_pattern;
    uint8_t enable_cpu_mem_arena;

    /* 图优化级别 */
    int graph_optimization_level;  /* 0-3 */

    /* 日志级别 */
    int log_severity_level;
} OrtSessionOptions;

/* ========== 运行时环境 ========== */
typedef struct {
    void* env;
    void* session;
    void* allocator;

    /* 输入输出信息缓存 */
    char** input_names;
    char** output_names;
    size_t num_inputs;
    size_t num_outputs;

    uint8_t is_initialized;
} OrtEmbeddedContext;

/* ========== API函数 ========== */

/**
 * @brief 初始化ONNX Runtime环境
 */
int ort_init(OrtEmbeddedContext* ctx, const OrtSessionOptions* options);

/**
 * @brief 加载ONNX模型
 */
int ort_load_model(OrtEmbeddedContext* ctx,
                    const uint8_t* model_data,
                    size_t model_size);

/**
 * @brief 准备输入张量
 */
int ort_prepare_input(OrtEmbeddedContext* ctx,
                       const char* input_name,
                       const void* data,
                       ONNXTensorElementDataType type,
                       const int64_t* dims,
                       size_t num_dims);

/**
 * @brief 执行推理
 */
int ort_run(OrtEmbeddedContext* ctx);

/**
 * @brief 获取输出张量
 */
int ort_get_output(OrtEmbeddedContext* ctx,
                    const char* output_name,
                    void* buffer,
                    size_t buffer_size);

/**
 * @brief 释放资源
 */
void ort_cleanup(OrtEmbeddedContext* ctx);

#endif /* ONNX_RUNTIME_EMBEDDED_H */


### 4.2 ONNX Runtime嵌入式实现

