/*
 * Auto-generated from: 11_Machine_Learning_C\03_Embedded_AI_Deployment.md
 * Line: 564
 * Language: c
 * Block ID: 12700c8f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file quantization_types.h
 * @brief 量化类型定义和基础操作
 */

#ifndef QUANTIZATION_TYPES_H
#define QUANTIZATION_TYPES_H

#include <stdint.h>
#include <math.h>

/* ========== 量化方案枚举 ========== */
typedef enum {
    QUANT_NONE = 0,           /* 无量化 (FP32) */
    QUANT_FP16,               /* 半精度浮点 */
    QUANT_INT8_SYMMETRIC,     /* 对称INT8量化 */
    QUANT_INT8_ASYMMETRIC,    /* 非对称INT8量化 */
    QUANT_INT16,              /* INT16量化 */
    QUANT_DYNAMIC,            /* 动态量化 */
    QUANT_PER_CHANNEL,        /* 逐通道量化 */
} quantization_scheme_t;

/* ========== 量化参数结构 ========== */
typedef struct {
    quantization_scheme_t scheme;
    float scale;              /* 缩放因子 */
    int32_t zero_point;       /* 零点偏移 (非对称量化) */
    int32_t qmin;             /* 量化最小值 */
    int32_t qmax;             /* 量化最大值 */
} quantization_params_t;

/* ========== 量化张量 ========== */
typedef struct {
    void* data;               /* 量化数据指针 */
    uint32_t num_elements;    /* 元素数量 */
    uint32_t element_size;    /* 元素大小 (1=INT8, 2=FP16/INT16, 4=FP32) */
    quantization_params_t qparams;
    uint8_t is_per_channel;   /* 是否逐通道量化 */
    uint32_t channel_dim;     /* 通道维度 */
} quantized_tensor_t;

#endif /* QUANTIZATION_TYPES_H */
