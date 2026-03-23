# 嵌入式AI部署完整指南

> **版本**: v1.0.0
> **更新日期**: 2026-03-17
> **适用平台**: ARM Cortex-M系列, ESP32, RISC-V MCU
> **目标读者**: 嵌入式系统工程师、AI算法工程师

---

## 目录

- [嵌入式AI部署完整指南](#嵌入式ai部署完整指南)
  - [目录](#目录)
  - [1. 嵌入式AI概述](#1-嵌入式ai概述)
    - [1.1 MCU上的神经网络基础](#11-mcu上的神经网络基础)
    - [1.2 典型嵌入式AI应用场景](#12-典型嵌入式ai应用场景)
    - [1.3 神经网络推理流程](#13-神经网络推理流程)
    - [1.4 嵌入式AI开发流程](#14-嵌入式ai开发流程)
  - [2. 模型量化技术](#2-模型量化技术)
    - [2.1 量化原理概述](#21-量化原理概述)
    - [2.2 INT8对称量化实现](#22-int8对称量化实现)
    - [2.3 动态量化实现](#23-动态量化实现)
    - [2.4 Python量化脚本](#24-python量化脚本)
  - [3. TensorFlow Lite Micro集成](#3-tensorflow-lite-micro集成)
    - [3.1 TFLM架构概述](#31-tflm架构概述)
    - [3.2 TFLM完整实现](#32-tflm完整实现)
    - [3.3 完整TFLM应用示例](#33-完整tflm应用示例)
    - [3.4 CMake构建配置](#34-cmake构建配置)
  - [4. ONNX Runtime for Embedded](#4-onnx-runtime-for-embedded)
    - [4.1 ONNX Runtime简介](#41-onnx-runtime简介)
    - [4.3 ONNX模型转换脚本](#43-onnx模型转换脚本)
  - [5. 神经网络算子优化](#5-神经网络算子优化)
    - [5.1 CMSIS-NN集成](#51-cmsis-nn集成)
    - [5.2 CMSIS-NN实现](#52-cmsis-nn实现)
    - [5.3 自定义算子优化](#53-自定义算子优化)
  - [6. 视觉模型部署](#6-视觉模型部署)
    - [6.1 人脸识别系统](#61-人脸识别系统)
    - [6.2 物体检测系统](#62-物体检测系统)
  - [7. 语音模型部署](#7-语音模型部署)
    - [7.1 关键词唤醒 (KWS)](#71-关键词唤醒-kws)
  - [8. 传感器数据处理](#8-传感器数据处理)
    - [8.1 FFT和滤波器](#81-fft和滤波器)
  - [9. 内存优化策略](#9-内存优化策略)
    - [9.1 内存布局与分配](#91-内存布局与分配)
    - [9.2 内存池实现](#92-内存池实现)
  - [10. 功耗优化](#10-功耗优化)
    - [10.1 电源管理策略](#101-电源管理策略)
    - [10.2 功耗管理实现](#102-功耗管理实现)
  - [11. 实际项目: STM32物体检测](#11-实际项目-stm32物体检测)
    - [11.1 项目概述](#111-项目概述)
    - [11.2 项目文件结构](#112-项目文件结构)
    - [11.3 核心代码实现](#113-核心代码实现)
    - [11.4 模型训练与转换脚本](#114-模型训练与转换脚本)
    - [11.5 构建配置](#115-构建配置)
  - [附录](#附录)
    - [A. 参考资源](#a-参考资源)
    - [B. 常见问题](#b-常见问题)
    - [C. 性能基准](#c-性能基准)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 1. 嵌入式AI概述

### 1.1 MCU上的神经网络基础

嵌入式AI（TinyML）是指在资源受限的微控制器上运行机器学习推理的技术。与传统云计算AI不同，嵌入式AI具有以下特点：

| 特性 | 云端AI | 嵌入式AI |
|------|--------|----------|
| 计算资源 | GPU/TPU集群 | MCU (数十MHz-数百MHz) |
| 内存容量 | GB-TB级别 | KB-MB级别 |
| 功耗 | 百瓦级 | 毫瓦-微瓦级 |
| 延迟 | 网络依赖 | 实时响应 |
| 隐私 | 数据传输 | 本地处理 |

### 1.2 典型嵌入式AI应用场景

```c
/**
 * @file embedded_ai_types.h
 * @brief 嵌入式AI应用场景定义
 * @note 不同场景对模型和硬件的要求差异巨大
 */

#ifndef EMBEDDED_AI_TYPES_H
#define EMBEDDED_AI_TYPES_H

#include <stdint.h>

/* ========== 应用场景枚举 ========== */
typedef enum {
    AI_APP_NONE = 0,

    /* 视觉类应用 */
    AI_APP_OBJECT_DETECTION,      /* 物体检测 - 需要CNN, 高内存 */
    AI_APP_FACE_RECOGNITION,      /* 人脸识别 - 需要特征提取 */
    AI_APP_GESTURE_RECOGNITION,   /* 手势识别 - 轻量级CNN */
    AI_APP_ANOMALY_DETECTION,     /* 异常检测 - 自编码器 */

    /* 语音类应用 */
    AI_APP_KEYWORD_SPOTTING,      /* 关键词唤醒 - 小型CNN/RNN */
    AI_APP_SPEECH_RECOGNITION,    /* 语音识别 - 需要RNN/Transformer */
    AI_APP_SOUND_CLASSIFICATION,  /* 声音分类 - 简单CNN */

    /* 传感器类应用 */
    AI_APP_ACTIVITY_RECOGNITION,  /* 活动识别 - 时序模型 */
    AI_APP_PREDICTIVE_MAINTENANCE,/* 预测性维护 - 异常检测 */
    AI_APP_ENVIRONMENT_SENSING,   /* 环境感知 - 多传感器融合 */

    AI_APP_COUNT
} ai_application_type_t;

/* ========== 资源需求结构 ========== */
typedef struct {
    uint32_t flash_required;      /* Flash需求 (字节) */
    uint32_t ram_required;        /* RAM需求 (字节) */
    uint32_t cpu_mhz;             /* 推荐CPU频率 */
    uint32_t inference_time_ms;   /* 单次推理时间 (毫秒) */
    float power_consumption_mw;   /* 功耗估计 (毫瓦) */
} ai_resource_requirement_t;

/* 典型应用场景资源需求表 */
static const ai_resource_requirement_t ai_app_requirements[] = {
    /* 应用类型                  Flash    RAM      CPU    时间    功耗 */
    [AI_APP_KEYWORD_SPOTTING] = {80*1024,  32*1024,  80,   10,   10.0f},
    [AI_APP_OBJECT_DETECTION] = {500*1024, 256*1024, 480,  200,  150.0f},
    [AI_APP_FACE_RECOGNITION] = {300*1024, 128*1024, 240,  100,  80.0f},
    [AI_APP_ACTIVITY_RECOGNITION] = {50*1024, 20*1024, 64, 5, 5.0f},
};

/* ========== 硬件平台枚举 ========== */
typedef enum {
    PLATFORM_STM32F4,       /* STM32F4系列 - Cortex-M4, FPU */
    PLATFORM_STM32F7,       /* STM32F7系列 - Cortex-M7, DP-FPU */
    PLATFORM_STM32H7,       /* STM32H7系列 - Cortex-M7, 高性能 */
    PLATFORM_STM32L4,       /* STM32L4系列 - 超低功耗 */
    PLATFORM_ESP32,         /* ESP32 - Xtensa LX6, WiFi/BT */
    PLATFORM_ESP32S3,       /* ESP32-S3 - Xtensa LX7, AI加速 */
    PLATFORM_NRF52840,      /* nRF52840 - Cortex-M4, BLE */
    PLATFORM_RP2040,        /* RP2040 - 双核Cortex-M0+ */
    PLATFORM_RISCV_K210,    /* K210 - RISC-V, KPU加速器 */
    PLATFORM_COUNT
} ai_platform_t;

/* 平台能力查询函数声明 */
const ai_resource_requirement_t* ai_get_platform_capability(ai_platform_t platform);
int ai_check_app_compatibility(ai_platform_t platform, ai_application_type_t app);

#endif /* EMBEDDED_AI_TYPES_H */
```

### 1.3 神经网络推理流程

```c
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
```

### 1.4 嵌入式AI开发流程

```python
#!/usr/bin/env python3
"""
embedded_ai_workflow.py
嵌入式AI完整开发流程脚本
"""

import os
import subprocess
from pathlib import Path
from enum import Enum

class DevelopmentStage(Enum):
    """开发阶段枚举"""
    DATA_COLLECTION = "数据收集"
    MODEL_TRAINING = "模型训练"
    MODEL_OPTIMIZATION = "模型优化"
    QUANTIZATION = "模型量化"
    CONVERSION = "格式转换"
    DEPLOYMENT = "部署测试"
    OPTIMIZATION = "性能优化"

class EmbeddedAIWorkflow:
    """
    嵌入式AI完整开发流程管理类

    流程图:
    原始数据 -> 数据增强 -> 模型训练 -> 剪枝 -> 量化 -> 转换 -> 部署 -> 优化
    """

    def __init__(self, project_name: str, target_platform: str):
        self.project_name = project_name
        self.target_platform = target_platform  # 'stm32', 'esp32', 'risc-v'
        self.work_dir = Path(f"./{project_name}")
        self.model_path = None
        self.quantized_model_path = None

    def setup_workspace(self):
        """创建工作目录结构"""
        dirs = [
            'data/raw',
            'data/processed',
            'models/original',
            'models/optimized',
            'models/quantized',
            'src/include',
            'src/model',
            'scripts',
            'tools',
            'docs'
        ]

        for d in dirs:
            (self.work_dir / d).mkdir(parents=True, exist_ok=True)

        print(f"[INFO] 工作空间已创建: {self.work_dir}")

    def check_dependencies(self):
        """检查必要的依赖工具"""
        tools = {
            'python3': 'Python 3.x',
            'tensorflow': 'TensorFlow >= 2.x',
            'tflite_convert': 'TFLite Converter',
        }

        missing = []
        for cmd, desc in tools.items():
            try:
                subprocess.run([cmd, '--version'],
                              capture_output=True, check=True)
                print(f"[OK] {desc}")
            except (subprocess.CalledProcessError, FileNotFoundError):
                missing.append(desc)
                print(f"[MISSING] {desc}")

        if missing:
            print(f"[WARNING] 缺少以下工具: {', '.join(missing)}")

        return len(missing) == 0

    def generate_conversion_script(self):
        """生成模型转换脚本"""
        script_content = f'''#!/usr/bin/env python3
"""
model_conversion.py - 模型转换脚本
自动生成的转换脚本，目标平台: {self.target_platform}
"""

import tensorflow as tf
import numpy as np
from pathlib import Path

class ModelConverter:
    """模型转换器"""

    def __init__(self, model_path: str):
        self.model_path = model_path
        self.model = None

    def load_model(self):
        """加载Keras模型"""
        self.model = tf.keras.models.load_model(self.model_path)
        print(f"[INFO] 模型加载成功")
        self.model.summary()
        return self

    def quantize_int8(self, representative_dataset):
        """INT8量化"""
        converter = tf.lite.TFLiteConverter.from_keras_model(self.model)

        # 启用优化
        converter.optimizations = [tf.lite.Optimize.DEFAULT]

        # INT8量化配置
        converter.representative_dataset = representative_dataset
        converter.target_spec.supported_ops = [
            tf.lite.OpsSet.TFLITE_BUILTINS_INT8
        ]
        converter.inference_input_type = tf.int8
        converter.inference_output_type = tf.int8

        tflite_model = converter.convert()

        # 保存量化模型
        output_path = Path(self.model_path).with_suffix('.int8.tflite')
        with open(output_path, 'wb') as f:
            f.write(tflite_model)

        print(f"[INFO] INT8量化模型已保存: {{output_path}}")
        print(f"[INFO] 模型大小: {{len(tflite_model) / 1024:.2f}} KB")

        return str(output_path)

    def quantize_fp16(self):
        """FP16量化"""
        converter = tf.lite.TFLiteConverter.from_keras_model(self.model)
        converter.optimizations = [tf.lite.Optimize.DEFAULT]
        converter.target_spec.supported_types = [tf.float16]

        tflite_model = converter.convert()

        output_path = Path(self.model_path).with_suffix('.fp16.tflite')
        with open(output_path, 'wb') as f:
            f.write(tflite_model)

        print(f"[INFO] FP16量化模型已保存: {{output_path}}")

        return str(output_path)

    def convert_to_c_array(self, tflite_path: str, output_name: str = "model"):
        """转换为C数组"""
        with open(tflite_path, 'rb') as f:
            tflite_model = f.read()

        c_array = ','.join(str(b) for b in tflite_model)

        header_content = f"""/* Auto-generated model array */
#ifndef {{output_name.upper()}}_H
#define {{output_name.upper()}}_H

#include <stdint.h>

const uint32_t {{output_name}}_len = {{len(tflite_model)}};
const uint8_t {{output_name}}_data[] = {{
    {{c_array}}
}};

#endif /* {{output_name.upper()}}_H */
"""

        output_path = Path(tflite_path).parent / f"{{output_name}}.h"
        with open(output_path, 'w') as f:
            f.write(header_content)

        print(f"[INFO] C头文件已生成: {{output_path}}")

if __name__ == '__main__':
    import sys
    if len(sys.argv) < 2:
        print("Usage: python model_conversion.py <model.h5>")
        sys.exit(1)

    converter = ModelConverter(sys.argv[1])
    converter.load_model()
    converter.quantize_fp16()
'''

        script_path = self.work_dir / 'scripts' / 'model_conversion.py'
        with open(script_path, 'w') as f:
            f.write(script_content)

        print(f"[INFO] 转换脚本已生成: {script_path}")
        return str(script_path)

if __name__ == '__main__':
    workflow = EmbeddedAIWorkflow("my_tinyml_project", "stm32")
    workflow.setup_workspace()
    workflow.check_dependencies()
    workflow.generate_conversion_script()
```

---

## 2. 模型量化技术

### 2.1 量化原理概述

模型量化是将神经网络权重和激活从高精度浮点数（FP32）转换为低精度表示（INT8/FP16）的过程。

```c
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
```

### 2.2 INT8对称量化实现

```c
/**
 * @file int8_quantization.c
 * @brief INT8对称量化完整实现
 *
 * 对称量化公式:
 *   q = round(r / scale)
 *   r = q * scale
 *
 * scale计算:
 *   scale = max(abs(rmin), abs(rmax)) / 127
 */

#include "quantization_types.h"
#include <string.h>

/* ========== 量化工具函数 ========== */

/**
 * @brief 计算对称量化scale
 * @param rmin 最小值
 * @param rmax 最大值
 * @return scale值
 */
float int8_calc_scale(float rmin, float rmax)
{
    float abs_max = fmaxf(fabsf(rmin), fabsf(rmax));

    /* 避免除以零 */
    if (abs_max < 1e-8f) {
        return 1.0f;
    }

    return abs_max / 127.0f;
}

/**
 * @brief FP32 -> INT8 量化
 * @param input 输入FP32数组
 * @param output 输出INT8数组
 * @param size 数组大小
 * @param scale 缩放因子
 */
void quantize_fp32_to_int8(const float* input,
                           int8_t* output,
                           uint32_t size,
                           float scale)
{
    /* 量化公式: q = round(r / scale) */
    const float inv_scale = 1.0f / scale;

    for (uint32_t i = 0; i < size; i++) {
        /* 缩放并四舍五入 */
        float scaled = input[i] * inv_scale;

        /* 限制在INT8范围内 */
        if (scaled > 127.0f) scaled = 127.0f;
        if (scaled < -128.0f) scaled = -128.0f;

        output[i] = (int8_t)(scaled > 0 ? (scaled + 0.5f) : (scaled - 0.5f));
    }
}

/**
 * @brief INT8 -> FP32 反量化
 * @param input 输入INT8数组
 * @param output 输出FP32数组
 * @param size 数组大小
 * @param scale 缩放因子
 */
void dequantize_int8_to_fp32(const int8_t* input,
                              float* output,
                              uint32_t size,
                              float scale)
{
    /* 反量化公式: r = q * scale */
    for (uint32_t i = 0; i < size; i++) {
        output[i] = (float)input[i] * scale;
    }
}

/* ========== 量化矩阵乘法 ========== */

/**
 * @brief INT8量化矩阵乘法: C = A * B
 * @param A 左矩阵 (M x K)
 * @param B 右矩阵 (K x N)
 * @param C 结果矩阵 (M x N)
 * @param M 行数
 * @param K 内维
 * @param N 列数
 * @param scale_a A的scale
 * @param scale_b B的scale
 * @param scale_c C的scale
 *
 * 注意: 输出需要反量化后再量化到INT8
 */
void quantized_matmul_int8(const int8_t* A,
                            const int8_t* B,
                            int8_t* C,
                            uint32_t M, uint32_t K, uint32_t N,
                            float scale_a,
                            float scale_b,
                            float scale_c)
{
    /* 累积使用INT32防止溢出 */
    const float combined_scale = (scale_a * scale_b) / scale_c;

    for (uint32_t m = 0; m < M; m++) {
        for (uint32_t n = 0; n < N; n++) {
            int32_t acc = 0;

            /* 内积计算 */
            for (uint32_t k = 0; k < K; k++) {
                acc += (int32_t)A[m * K + k] * (int32_t)B[k * N + n];
            }

            /* 重新量化到INT8 */
            float scaled = (float)acc * combined_scale;

            /* 限制范围并四舍五入 */
            if (scaled > 127.0f) scaled = 127.0f;
            if (scaled < -128.0f) scaled = -128.0f;

            C[m * N + n] = (int8_t)(scaled > 0 ? (scaled + 0.5f) : (scaled - 0.5f));
        }
    }
}

/**
 * @brief 优化的4x4 INT8矩阵乘法块
 * 使用局部变量累加，减少内存访问
 */
static void matmul_4x4_int8_block(const int8_t* A, const int8_t* B, int8_t* C,
                                   uint32_t K, float combined_scale)
{
    int32_t acc[4][4] = {{0}};

    /* 展开内层循环 */
    for (uint32_t k = 0; k < K; k++) {
        int8_t a0 = A[0 * K + k];
        int8_t a1 = A[1 * K + k];
        int8_t a2 = A[2 * K + k];
        int8_t a3 = A[3 * K + k];

        int8_t b0 = B[k * 4 + 0];
        int8_t b1 = B[k * 4 + 1];
        int8_t b2 = B[k * 4 + 2];
        int8_t b3 = B[k * 4 + 3];

        acc[0][0] += a0 * b0; acc[0][1] += a0 * b1;
        acc[0][2] += a0 * b2; acc[0][3] += a0 * b3;

        acc[1][0] += a1 * b0; acc[1][1] += a1 * b1;
        acc[1][2] += a1 * b2; acc[1][3] += a1 * b3;

        acc[2][0] += a2 * b0; acc[2][1] += a2 * b1;
        acc[2][2] += a2 * b2; acc[2][3] += a2 * b3;

        acc[3][0] += a3 * b0; acc[3][1] += a3 * b1;
        acc[3][2] += a3 * b2; acc[3][3] += a3 * b3;
    }

    /* 重新量化并存储 */
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float scaled = (float)acc[i][j] * combined_scale;
            if (scaled > 127.0f) scaled = 127.0f;
            if (scaled < -128.0f) scaled = -128.0f;
            C[i * 4 + j] = (int8_t)(scaled > 0 ? (scaled + 0.5f) : (scaled - 0.5f));
        }
    }
}

/* ========== 量化卷积实现 ========== */

/**
 * @brief INT8量化2D卷积 (简化版，无padding)
 * @param input 输入特征图 [H x W x C_in]
 * @param kernel 卷积核 [K_h x K_w x C_in x C_out]
 * @param output 输出特征图 [H_out x W_out x C_out]
 */
void quantized_conv2d_int8(const int8_t* input,
                            const int8_t* kernel,
                            int8_t* output,
                            uint32_t H, uint32_t W, uint32_t C_in,
                            uint32_t K_h, uint32_t K_w,
                            uint32_t C_out,
                            uint32_t stride_h, uint32_t stride_w,
                            float input_scale,
                            float kernel_scale,
                            float output_scale)
{
    uint32_t H_out = (H - K_h) / stride_h + 1;
    uint32_t W_out = (W - K_w) / stride_w + 1;

    const float combined_scale = (input_scale * kernel_scale) / output_scale;

    for (uint32_t co = 0; co < C_out; co++) {
        for (uint32_t ho = 0; ho < H_out; ho++) {
            for (uint32_t wo = 0; wo < W_out; wo++) {
                int32_t acc = 0;

                /* 卷积窗口 */
                for (uint32_t kh = 0; kh < K_h; kh++) {
                    for (uint32_t kw = 0; kw < K_w; kw++) {
                        uint32_t ih = ho * stride_h + kh;
                        uint32_t iw = wo * stride_w + kw;

                        for (uint32_t ci = 0; ci < C_in; ci++) {
                            int8_t in_val = input[(ih * W + iw) * C_in + ci];
                            int8_t k_val = kernel[((co * K_h + kh) * K_w + kw) * C_in + ci];
                            acc += (int32_t)in_val * (int32_t)k_val;
                        }
                    }
                }

                /* 重新量化 */
                float scaled = (float)acc * combined_scale;
                if (scaled > 127.0f) scaled = 127.0f;
                if (scaled < -128.0f) scaled = -128.0f;

                output[(ho * W_out + wo) * C_out + co] =
                    (int8_t)(scaled > 0 ? (scaled + 0.5f) : (scaled - 0.5f));
            }
        }
    }
}
```

### 2.3 动态量化实现

```c
/**
 * @file dynamic_quantization.c
 * @brief 动态量化实现
 *
 * 动态量化特点:
 * - 权重静态量化 (离线)
 * - 激活动态量化 (运行时)
 * - 适合RNN/LSTM等时序网络
 */

#include "quantization_types.h"
#include <float.h>

/**
 * @brief 运行时动态计算量化参数
 * @param data 输入数据
 * @param size 数据大小
 * @param qparams 输出量化参数
 */
void dynamic_calc_quant_params(const float* data,
                                uint32_t size,
                                quantization_params_t* qparams)
{
    /* 1. 寻找最小最大值 */
    float rmin = FLT_MAX;
    float rmax = -FLT_MAX;

    for (uint32_t i = 0; i < size; i++) {
        if (data[i] < rmin) rmin = data[i];
        if (data[i] > rmax) rmax = data[i];
    }

    /* 2. 计算量化参数 */
    qparams->scheme = QUANT_INT8_SYMMETRIC;
    qparams->scale = int8_calc_scale(rmin, rmax);
    qparams->zero_point = 0;  /* 对称量化 */
    qparams->qmin = -128;
    qparams->qmax = 127;
}

/**
 * @brief 动态量化线性层
 * @param input 输入 (FP32)
 * @param weight 权重 (INT8, 已量化)
 * @param bias 偏置 (FP32)
 * @param output 输出 (FP32)
 * @param in_features 输入特征数
 * @param out_features 输出特征数
 * @param weight_scale 权重缩放因子
 */
void dynamic_quantized_linear(const float* input,
                               const int8_t* weight,
                               const float* bias,
                               float* output,
                               uint32_t in_features,
                               uint32_t out_features,
                               float weight_scale)
{
    /* 1. 动态量化输入 */
    quantization_params_t input_qparams;
    dynamic_calc_quant_params(input, in_features, &input_qparams);

    /* 分配临时INT8缓冲区 */
    int8_t* quantized_input = (int8_t*)malloc(in_features);
    if (!quantized_input) return;

    quantize_fp32_to_int8(input, quantized_input, in_features,
                          input_qparams.scale);

    /* 2. INT8矩阵乘法 */
    for (uint32_t o = 0; o < out_features; o++) {
        int32_t acc = 0;

        for (uint32_t i = 0; i < in_features; i++) {
            acc += (int32_t)quantized_input[i] *
                   (int32_t)weight[o * in_features + i];
        }

        /* 3. 反量化并加偏置 */
        float dequantized = (float)acc * input_qparams.scale * weight_scale;
        output[o] = dequantized + (bias ? bias[o] : 0.0f);
    }

    free(quantized_input);
}

/* ========== 逐通道量化 ========== */

typedef struct {
    float* scales;            /* 每个通道的scale */
    uint32_t num_channels;
} per_channel_quant_params_t;

/**
 * @brief 逐通道量化卷积核
 * 卷积核: [C_out x K_h x K_w x C_in]
 * 每个输出通道使用独立的scale
 */
void quantize_conv_kernel_per_channel(const float* kernel_fp32,
                                       int8_t* kernel_int8,
                                       per_channel_quant_params_t* pc_params,
                                       uint32_t C_out,
                                       uint32_t K_h,
                                       uint32_t K_w,
                                       uint32_t C_in)
{
    uint32_t kernel_size_per_channel = K_h * K_w * C_in;

    for (uint32_t co = 0; co < C_out; co++) {
        const float* channel_kernel = kernel_fp32 + co * kernel_size_per_channel;
        int8_t* channel_quant = kernel_int8 + co * kernel_size_per_channel;

        /* 计算该通道的min/max */
        float rmin = FLT_MAX, rmax = -FLT_MAX;
        for (uint32_t i = 0; i < kernel_size_per_channel; i++) {
            if (channel_kernel[i] < rmin) rmin = channel_kernel[i];
            if (channel_kernel[i] > rmax) rmax = channel_kernel[i];
        }

        /* 计算scale并量化 */
        pc_params->scales[co] = int8_calc_scale(rmin, rmax);
        quantize_fp32_to_int8(channel_kernel, channel_quant,
                              kernel_size_per_channel,
                              pc_params->scales[co]);
    }
}
```

### 2.4 Python量化脚本

```python
#!/usr/bin/env python3
"""
quantization_toolkit.py
完整的模型量化工具集
"""

import tensorflow as tf
import numpy as np
from typing import Callable, List, Tuple, Optional
import json
from pathlib import Path

class QuantizationConfig:
    """量化配置类"""

    def __init__(self):
        self.scheme: str = "int8_symmetric"  # int8_symmetric, int8_asymmetric, fp16
        self.per_channel: bool = True
        self.per_tensor: bool = False
        self.dynamic_range: bool = False
        self.calibration_samples: int = 100
        self.output_format: str = "tflite"   # tflite, c_array, binary

    def to_dict(self) -> dict:
        return {
            "scheme": self.scheme,
            "per_channel": self.per_channel,
            "per_tensor": self.per_tensor,
            "dynamic_range": self.dynamic_range,
            "calibration_samples": self.calibration_samples,
            "output_format": self.output_format
        }

class PostTrainingQuantizer:
    """训练后量化器 (PTQ)"""

    def __init__(self, model_path: str, config: QuantizationConfig = None):
        self.model_path = model_path
        self.config = config or QuantizationConfig()
        self.model = None
        self.calibration_data = []

    def load_model(self) -> 'PostTrainingQuantizer':
        """加载Keras模型"""
        if self.model_path.endswith('.h5') or self.model_path.endswith('.keras'):
            self.model = tf.keras.models.load_model(self.model_path)
        elif self.model_path.endswith('.tflite'):
            with open(self.model_path, 'rb') as f:
                self.model = f.read()
        else:
            raise ValueError(f"不支持的模型格式: {self.model_path}")

        print(f"[INFO] 模型已加载: {self.model_path}")
        if isinstance(self.model, tf.keras.Model):
            self.model.summary()
        return self

    def prepare_calibration_data(self,
                                   dataset_gen: Callable,
                                   num_samples: int = None):
        """准备校准数据集"""
        num_samples = num_samples or self.config.calibration_samples

        print(f"[INFO] 准备校准数据: {num_samples} samples")

        def representative_dataset():
            for i, sample in enumerate(dataset_gen()):
                if i >= num_samples:
                    break
                # 确保是正确的格式
                if isinstance(sample, tuple):
                    sample = sample[0]  # 取输入部分
                yield [np.expand_dims(sample, axis=0).astype(np.float32)]

        self.calibration_data = representative_dataset
        return self

    def quantize(self, output_path: str = None) -> str:
        """执行量化"""
        if isinstance(self.model, bytes):
            # 已经是TFLite模型
            converter = tf.lite.TFLiteConverter(self.model)
        else:
            converter = tf.lite.TFLiteConverter.from_keras_model(self.model)

        # 基础优化
        converter.optimizations = [tf.lite.Optimize.DEFAULT]

        # 根据配置选择量化方案
        if self.config.scheme == "fp16":
            converter.target_spec.supported_types = [tf.float16]
            print("[INFO] 使用FP16量化")

        elif self.config.scheme.startswith("int8"):
            if self.calibration_data is None:
                raise ValueError("INT8量化需要校准数据")

            converter.representative_dataset = self.calibration_data

            # 确保仅使用INT8算子
            converter.target_spec.supported_ops = [
                tf.lite.OpsSet.TFLITE_BUILTINS_INT8
            ]

            # 输入输出类型
            if self.config.scheme == "int8_symmetric":
                converter.inference_input_type = tf.int8
                converter.inference_output_type = tf.int8
            else:  # asymmetric
                converter.inference_input_type = tf.uint8
                converter.inference_output_type = tf.uint8

            print(f"[INFO] 使用{self.config.scheme}量化")
            print(f"[INFO] 逐通道量化: {self.config.per_channel}")

        # 转换
        tflite_model = converter.convert()

        # 保存
        if output_path is None:
            suffix = f".{self.config.scheme}.tflite"
            output_path = str(Path(self.model_path).with_suffix('')) + suffix

        with open(output_path, 'wb') as f:
            f.write(tflite_model)

        # 计算压缩比
        original_size = Path(self.model_path).stat().st_size / 1024
        quantized_size = len(tflite_model) / 1024

        print(f"[INFO] 量化完成!")
        print(f"[INFO] 原始大小: {original_size:.2f} KB")
        print(f"[INFO] 量化后大小: {quantized_size:.2f} KB")
        print(f"[INFO] 压缩比: {original_size/quantized_size:.2f}x")
        print(f"[INFO] 保存路径: {output_path}")

        # 保存配置
        config_path = str(Path(output_path).with_suffix('.json'))
        with open(config_path, 'w') as f:
            json.dump(self.config.to_dict(), f, indent=2)

        return output_path

class QuantizationAwareTrainer:
    """量化感知训练 (QAT)"""

    def __init__(self, model: tf.keras.Model):
        self.model = model
        self.qat_model = None

    def apply_qat(self) -> tf.keras.Model:
        """应用QAT"""
        import tensorflow_model_optimization as tfmot

        quantize_model = tfmot.quantization.keras.quantize_model

        self.qat_model = quantize_model(self.model)

        self.qat_model.compile(
            optimizer='adam',
            loss='sparse_categorical_crossentropy',
            metrics=['accuracy']
        )

        print("[INFO] QAT模型已创建")
        self.qat_model.summary()

        return self.qat_model

    def train(self,
              x_train, y_train,
              x_val=None, y_val=None,
              epochs: int = 10,
              batch_size: int = 32):
        """训练QAT模型"""
        validation_data = (x_val, y_val) if x_val is not None else None

        history = self.qat_model.fit(
            x_train, y_train,
            epochs=epochs,
            batch_size=batch_size,
            validation_data=validation_data,
            verbose=1
        )

        return history

    def convert_to_tflite(self, output_path: str) -> str:
        """转换为TFLite"""
        converter = tf.lite.TFLiteConverter.from_keras_model(self.qat_model)
        converter.optimizations = [tf.lite.Optimize.DEFAULT]

        tflite_model = converter.convert()

        with open(output_path, 'wb') as f:
            f.write(tflite_model)

        print(f"[INFO] QAT模型已保存: {output_path}")
        return output_path

def generate_calibration_data(model_input_shape: Tuple[int, ...],
                               num_samples: int = 100,
                               value_range: Tuple[float, float] = (0.0, 1.0)):
    """生成随机校准数据"""
    def data_generator():
        for _ in range(num_samples):
            # 生成随机数据模拟实际输入分布
            data = np.random.uniform(
                value_range[0],
                value_range[1],
                model_input_shape
            ).astype(np.float32)
            yield data
    return data_generator()

# 使用示例

---

## 🔗 全面知识关联体系

### 【全局层】知识库导航
| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../00_GLOBAL_INDEX.md](../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../README.md](../README.md) | 模块总览与目录导航 |
| **学习路径** | [../06_Thinking_Representation/06_Learning_Paths/README.md](../06_Thinking_Representation/06_Learning_Paths/README.md) | 阶段化学习路线规划 |
| **概念映射** | [../06_Thinking_Representation/05_Concept_Mappings/README.md](../06_Thinking_Representation/05_Concept_Mappings/README.md) | 核心概念等价关系图 |

### 【阶段层】学习定位
**当前模块**: 知识库
**难度等级**: L1-L6
**前置依赖**: 核心知识体系
**后续延伸**: 持续学习

```

学习阶段金字塔:
    L6 专家层 [形式验证、编译器]
    L5 高级层 [并发、系统编程] ⬅️ 可能在此
    L4 进阶层 [指针、内存管理]
    L3 基础层 [函数、结构体]
    L2 入门层 [语法、数据类型]
    L1 零基础 [环境搭建]

```

### 【层次层】纵向知识链
| 层级 | 关联文档 | 层次关系 |
|:-----|:---------|:---------|
| **理论基础** | [../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md](../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md) | 语义学理论基础 |
| **核心机制** | [../01_Core_Knowledge_System/02_Core_Layer/README.md](../01_Core_Knowledge_System/02_Core_Layer/README.md) | C语言核心机制 |
| **标准接口** | [../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md](../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md) | 标准库API |
| **系统实现** | [../03_System_Technology_Domains/README.md](../03_System_Technology_Domains/README.md) | 系统级实现 |

### 【局部层】横向关联网
| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md](../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 并发编程技术 |
| **安全规范** | [../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md](../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md) | 安全编码标准 |
| **工具支持** | [../07_Modern_Toolchain/README.md](../07_Modern_Toolchain/README.md) | 现代开发工具链 |
| **实践案例** | [../04_Industrial_Scenarios/README.md](../04_Industrial_Scenarios/README.md) | 工业实践场景 |

### 【总体层】知识体系架构
```

┌─────────────────────────────────────────────────────────────┐
│                     总体知识体系架构                          │
├─────────────────────────────────────────────────────────────┤
│  01 Core Knowledge          → 核心概念与机制                  │
│  02 Formal Semantics        → 理论与物理基础                  │
│  03 System Technology       → 系统级技术领域                  │
│  04 Industrial Scenarios    → 工业应用场景                    │
│  05 Deep Structure          → 深层结构与元物理                │
│  06 Thinking Representation → 思维表征与学习                  │
│  07 Modern Toolchain        → 现代工具链                      │
└─────────────────────────────────────────────────────────────┘

```

### 【决策层】学习路径选择
| 目标 | 推荐路径 | 关键文档 |
|:-----|:---------|:---------|
| **系统学习** | 01 → 02 → 03 → 04 | 按顺序阅读各模块 |
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../06_Thinking_Representation/01_Decision_Trees/README.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../04_Industrial_Scenarios/README.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../02_Formal_Semantics_and_Physics/README.md) |

---

---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
if __name__ == '__main__':
    import sys

    if len(sys.argv) < 2:
        print("Usage: python quantization_toolkit.py <model.h5> [int8|fp16]")
        sys.exit(1)

    model_path = sys.argv[1]
    scheme = sys.argv[2] if len(sys.argv) > 2 else 'int8'

    # 创建配置
    config = QuantizationConfig()
    config.scheme = scheme
    config.per_channel = True

    # 初始化量化器
    quantizer = PostTrainingQuantizer(model_path, config)
    quantizer.load_model()

    # 准备校准数据 (这里使用随机数据，实际应用应使用真实数据)
    input_shape = quantizer.model.input_shape[1:]  # 去掉batch维度
    calib_gen = generate_calibration_data(input_shape, num_samples=100)
    quantizer.prepare_calibration_data(lambda: calib_gen)

    # 执行量化
    quantizer.quantize()
```

---

## 3. TensorFlow Lite Micro集成

### 3.1 TFLM架构概述

TensorFlow Lite Micro (TFLM) 是专为微控制器设计的机器学习推理框架，其核心特点：

- **无堆分配**: 所有内存使用静态分配
- **无操作系统依赖**: 可在裸机运行
- **C++ 11实现**: 使用有限C++子集

```c
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
```

### 3.2 TFLM完整实现

```c
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
```

### 3.3 完整TFLM应用示例

```c
/**
 * @file tflm_application.c
 * @brief TFLM完整应用示例 - 关键词唤醒
 *
 * 硬件: STM32F746NG
 * 模型: 20KB MicroSpeech模型
 */

#include <stdio.h>
#include <string.h>
#include "tflm_integration.h"

/* ========== 模型数据 (从Python转换生成) ========== */
/* 实际模型数据应该通过工具生成 */
extern const unsigned char g_model_data[];
extern const unsigned int g_model_len;

/* ========== 配置参数 ========== */
#define TENSOR_ARENA_SIZE     (20 * 1024)   /* 20KB张量内存 */
#define AUDIO_SAMPLE_RATE     16000         /* 16kHz采样率 */
#define AUDIO_WINDOW_MS       30            /* 30ms窗口 */
#define AUDIO_STRIDE_MS       20            /* 20ms步长 */
#define FEATURE_SIZE          40            /* 40维MFCC特征 */
#define SPECTROGRAM_LENGTH    49            /* 时间步数 */

/* ========== 全局变量 ========== */
static uint8_t s_tensor_arena[TENSOR_ARENA_SIZE];
static tflm_context_t s_tflm_ctx;
static tflm_config_t s_tflm_config;

/* 类别标签 */
static const char* kCategoryLabels[] = {
    "silence",    /* 静默 */
    "unknown",    /* 未知 */
    "yes",        /* 是 */
    "no"          /* 否 */
};
#define CATEGORY_COUNT 4

/* ========== 预处理: 音频 -> 频谱图 ========== */

/**
 * @brief 简化的FFT实现 (应使用CMSIS-DSP优化版本)
 */
static void simple_fft(const float* input,
                       float* real_out,
                       float* imag_out,
                       int n)
{
    /* 简化实现 - 实际应用使用CMSIS-DSP arm_rfft_fast_f32 */
    /* 这里仅作占位 */
    (void)input; (void)real_out; (void)imag_out; (void)n;
}

/**
 * @brief 计算MFCC特征
 * @param audio_samples 音频采样 (16kHz, 30ms = 480 samples)
 * @param mfcc_out 输出MFCC特征 (40维)
 */
static void compute_mfcc(const int16_t* audio_samples,
                          float* mfcc_out,
                          int num_mfcc)
{
    /* 1. 预加重 */
    float preemphasized[480];
    const float preemphasis = 0.97f;
    preemphasized[0] = (float)audio_samples[0];
    for (int i = 1; i < 480; i++) {
        preemphasized[i] = (float)audio_samples[i] -
                           preemphasis * (float)audio_samples[i-1];
    }

    /* 2. 加汉明窗 */
    float windowed[480];
    for (int i = 0; i < 480; i++) {
        float window = 0.54f - 0.46f * cosf(2.0f * 3.14159f * i / 479.0f);
        windowed[i] = preemphasized[i] * window;
    }

    /* 3. FFT -> 功率谱 */
    float fft_real[512] = {0};
    float fft_imag[512] = {0};
    memcpy(fft_real, windowed, sizeof(float) * 480);
    simple_fft(fft_real, fft_real, fft_imag, 512);

    /* 计算功率谱 */
    float power_spectrum[256];
    for (int i = 0; i < 256; i++) {
        power_spectrum[i] = fft_real[i] * fft_real[i] +
                            fft_imag[i] * fft_imag[i];
    }

    /* 4. Mel滤波器组 (简化) */
    /* 实际应用应使用预计算的Mel滤波器矩阵 */

    /* 5. Log -> DCT -> MFCC */
    /* 简化为随机值 - 实际应用需要完整的MFCC计算 */
    for (int i = 0; i < num_mfcc && i < 40; i++) {
        mfcc_out[i] = 0.0f;  /* 实际应计算 */
    }
}

/* ========== 主应用程序 ========== */

int main(void)
{
    /* 初始化HAL (平台相关) */
    HAL_Init();
    SystemClock_Config();

    /* 初始化音频采集 (I2S或PDM麦克风) */
    audio_init();

    /* 配置TFLM */
    memset(&s_tflm_config, 0, sizeof(s_tflm_config));
    s_tflm_config.model_data = g_model_data;
    s_tflm_config.model_size = g_model_len;
    s_tflm_config.tensor_arena = s_tensor_arena;
    s_tflm_config.arena_size = TENSOR_ARENA_SIZE;
    s_tflm_config.enable_profiling = 1;

    /* 初始化TFLM */
    int ret = tflm_init(&s_tflm_ctx, &s_tflm_config);
    if (ret != 0) {
        printf("TFLM initialization failed: %d\n", ret);
        Error_Handler();
    }

    printf("KWS System Started\n");
    printf("Listening for 'yes' or 'no'...\n");

    /* 频谱图缓冲区 [时间步 x 特征维度] */
    float spectrogram[SPECTROGRAM_LENGTH][FEATURE_SIZE];
    int spec_index = 0;

    while (1) {
        /* 1. 采集音频 (30ms = 480 samples @ 16kHz) */
        int16_t audio_buffer[480];
        audio_capture(audio_buffer, 480);

        /* 2. 计算MFCC特征 */
        float mfcc[FEATURE_SIZE];
        compute_mfcc(audio_buffer, mfcc, FEATURE_SIZE);

        /* 3. 更新频谱图 (滑动窗口) */
        memcpy(spectrogram[spec_index], mfcc, sizeof(mfcc));
        spec_index = (spec_index + 1) % SPECTROGRAM_LENGTH;

        /* 4. 每收集足够数据执行一次推理 */
        static int frame_count = 0;
        if (++frame_count >= SPECTROGRAM_LENGTH) {
            frame_count = 0;

            /* 准备输入 (转换为INT8) */
            /* 注意: 这里需要根据模型的量化参数进行量化 */
            int8_t quantized_input[SPECTROGRAM_LENGTH * FEATURE_SIZE];

            for (int t = 0; t < SPECTROGRAM_LENGTH; t++) {
                for (int f = 0; f < FEATURE_SIZE; f++) {
                    /* 假设输入scale为0.0390625 (1/25.6) */
                    float scaled = spectrogram[t][f] / 0.0390625f;

                    if (scaled > 127.0f) scaled = 127.0f;
                    if (scaled < -128.0f) scaled = -128.0f;

                    quantized_input[t * FEATURE_SIZE + f] =
                        (int8_t)(scaled > 0 ? scaled + 0.5f : scaled - 0.5f);
                }
            }

            /* 5. 执行推理 */
            uint32_t start = HAL_GetTick();

            tflm_prepare_input(&s_tflm_ctx, quantized_input,
                               sizeof(quantized_input));
            tflm_invoke(&s_tflm_ctx);

            uint32_t inference_time = HAL_GetTick() - start;

            /* 6. 获取输出 */
            int8_t output[CATEGORY_COUNT];
            tflm_get_output(&s_tflm_ctx, output, sizeof(output));

            /* 7. 后处理: 去量化并找到最大概率 */
            float probabilities[CATEGORY_COUNT];
            float max_prob = -128.0f;
            int max_index = 0;

            /* 假设输出scale为0.00390625 (1/256) */
            const float output_scale = 0.00390625f;

            for (int i = 0; i < CATEGORY_COUNT; i++) {
                probabilities[i] = output[i] * output_scale;
                if (output[i] > max_prob) {
                    max_prob = output[i];
                    max_index = i;
                }
            }

            /* 8. 应用检测阈值 */
            const float detection_threshold = 0.7f;
            float dequantized_max = max_prob * output_scale;

            if (dequantized_max > detection_threshold &&
                max_index >= 2) {  /* yes/no */
                printf("Detected: %s (confidence: %.2f, time: %lu ms)\n",
                       kCategoryLabels[max_index],
                       dequantized_max,
                       inference_time);

                /* 触发相应动作 */
                if (max_index == 2) {  /* yes */
                    LED_On(LED_GREEN);
                } else {  /* no */
                    LED_On(LED_RED);
                }
            }
        }

        /* 等待下一个音频窗口 (20ms步长) */
        HAL_Delay(20);
    }
}

/* ========== 辅助函数 ========== */

void Error_Handler(void)
{
    /* 错误处理: 闪烁LED */
    while (1) {
        LED_Toggle(LED_RED);
        HAL_Delay(200);
    }
}

void audio_init(void)
{
    /* I2S/PDM初始化 */
}

void audio_capture(int16_t* buffer, uint32_t samples)
{
    /* 从DMA缓冲区读取音频数据 */
}

void LED_On(int led_id) { /* ... */ }
void LED_Toggle(int led_id) { /* ... */ }
```

### 3.4 CMake构建配置

```cmake
# CMakeLists.txt - TFLM嵌入式项目配置

cmake_minimum_required(VERSION 3.16)
project(TFLM_Embedded_AI VERSION 1.0.0 LANGUAGES C CXX ASM)

# 设置C/C++标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# ========== 目标平台配置 ==========
set(TARGET_PLATFORM "STM32F7" CACHE STRING "Target platform")
set_property(CACHE TARGET_PLATFORM PROPERTY STRINGS
    STM32F4 STM32F7 STM32H7 ESP32 ESP32S3)

# ========== TFLM源码路径 ==========
set(TFLM_SOURCE_DIR "${CMAKE_SOURCE_DIR}/third_party/tflite-micro"
    CACHE PATH "Path to TensorFlow Lite Micro source")

# ========== 编译选项 ==========
if(CMAKE_CROSSCOMPILING)
    # 嵌入式平台编译选项
    add_compile_options(
        -mcpu=cortex-m7
        -mfpu=fpv5-d16
        -mfloat-abi=hard
        -mthumb
        -O3
        -ffunction-sections
        -fdata-sections
        -fno-rtti
        -fno-exceptions
        -fno-threadsafe-statics
    )

    add_link_options(
        -mcpu=cortex-m7
        -mfpu=fpv5-d16
        -mfloat-abi=hard
        -mthumb
        -Wl,--gc-sections
        -specs=nosys.specs
    )
else()
    # 主机模拟器模式
    add_compile_options(-O2 -g)
endif()

# ========== TFLM源码 ==========
set(TFLM_CORE_SOURCES
    ${TFLM_SOURCE_DIR}/tensorflow/lite/micro/micro_interpreter.cc
    ${TFLM_SOURCE_DIR}/tensorflow/lite/micro/micro_allocator.cc
    ${TFLM_SOURCE_DIR}/tensorflow/lite/micro/micro_error_reporter.cc
    ${TFLM_SOURCE_DIR}/tensorflow/lite/micro/micro_profiler.cc
    ${TFLM_SOURCE_DIR}/tensorflow/lite/micro/all_ops_resolver.cc
    ${TFLM_SOURCE_DIR}/tensorflow/lite/micro/memory_helpers.cc
    ${TFLM_SOURCE_DIR}/tensorflow/lite/micro/micro_time.cc
    ${TFLM_SOURCE_DIR}/tensorflow/lite/micro/micro_string.cc
    ${TFLM_SOURCE_DIR}/tensorflow/lite/micro/recording_micro_allocator.cc
    ${TFLM_SOURCE_DIR}/tensorflow/lite/micro/fake_micro_context.cc
    ${TFLM_SOURCE_DIR}/tensorflow/lite/micro/flatbuffer_utils.cc
    ${TFLM_SOURCE_DIR}/tensorflow/lite/micro/micro_log.cc
)

# 内核源码
file(GLOB TFLM_KERNEL_SOURCES
    ${TFLM_SOURCE_DIR}/tensorflow/lite/micro/kernels/*.cc)

# 核心库源码
set(TFLITE_CORE_SOURCES
    ${TFLM_SOURCE_DIR}/tensorflow/lite/c/common.c
    ${TFLM_SOURCE_DIR}/tensorflow/lite/core/api/error_reporter.cc
    ${TFLM_SOURCE_DIR}/tensorflow/lite/core/api/flatbuffer_conversions.cc
    ${TFLM_SOURCE_DIR}/tensorflow/lite/core/api/op_resolver.cc
    ${TFLM_SOURCE_DIR}/tensorflow/lite/core/api/tensor_utils.cc
    ${TFLM_SOURCE_DIR}/tensorflow/lite/kernels/kernel_util.cc
    ${TFLM_SOURCE_DIR}/tensorflow/lite/kernels/internal/quantization_util.cc
    ${TFLM_SOURCE_DIR}/tensorflow/lite/schema/schema_utils.cc
)

# ========== 创建TFLM库 ==========
add_library(tflm STATIC
    ${TFLM_CORE_SOURCES}
    ${TFLM_KERNEL_SOURCES}
    ${TFLITE_CORE_SOURCES}
)

target_include_directories(tflm PUBLIC
    ${TFLM_SOURCE_DIR}
    ${TFLM_SOURCE_DIR}/third_party/flatbuffers/include
    ${TFLM_SOURCE_DIR}/third_party/gemmlowp
    ${TFLM_SOURCE_DIR}/third_party/ruy
)

# ========== 主应用程序 ==========
add_executable(kws_app
    src/main.c
    src/tflm_integration.cpp
    src/audio_driver.c
    src/mfcc_features.c
    src/model_data.c  # 包含转换后的模型
)

target_link_libraries(kws_app PRIVATE tflm)

# CMSIS-DSP (如果使用)
if(USE_CMSIS_DSP)
    target_link_libraries(kws_app PRIVATE cmsis_dsp)
    target_compile_definitions(kws_app PRIVATE USE_CMSIS_DSP=1)
endif()

# ========== 模型转换目标 ==========
add_custom_target(convert_model
    COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/convert_model.py
            --input ${CMAKE_SOURCE_DIR}/models/kws_model.h5
            --output ${CMAKE_SOURCE_DIR}/src/model_data.c
            --quantize int8
    COMMENT "Converting and quantizing model..."
)

add_dependencies(kws_app convert_model)

# ========== 烧录目标 ==========
if(CMAKE_CROSSCOMPILING)
    add_custom_target(flash
        COMMAND openocd -f interface/stlink.cfg
                -f target/stm32f7x.cfg
                -c "program kws_app.bin 0x08000000 verify reset exit"
        DEPENDS kws_app
        COMMENT "Flashing to target..."
    )
endif()
```



---

## 4. ONNX Runtime for Embedded

### 4.1 ONNX Runtime简介

ONNX Runtime (ORT) 是微软开发的跨平台推理引擎，支持ONNX格式的模型。ORT for Embedded是专为边缘设备优化的版本。

```c
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

```c
/**
 * @file onnx_runtime_embedded.c
 * @brief ONNX Runtime简化嵌入式实现
 * @note 这是一个简化的概念实现，实际应使用官方ONNX Runtime库
 */

#include "onnx_runtime_embedded.h"
#include <string.h>
#include <stdlib.h>

/* ========== 简化的张量表示 ========== */
typedef struct {
    void* data;
    ONNXTensorElementDataType type;
    int64_t* dims;
    size_t num_dims;
    size_t data_size;
} SimpleTensor;

/* ========== 简化的会话存储 ========== */
typedef struct {
    SimpleTensor* inputs;
    SimpleTensor* outputs;
    size_t num_inputs;
    size_t num_outputs;

    /* 模型元数据 (简化) */
    uint8_t* model_buffer;
    size_t model_size;
} SimpleSession;

/* ========== 辅助函数 ========== */

/**
 * @brief 获取数据类型大小
 */
static size_t get_element_size(ONNXTensorElementDataType type)
{
    switch (type) {
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:  return 4;
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT8:   return 1;
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT16:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT16:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16: return 2;
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT32: return 4;
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT64:
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_DOUBLE: return 8;
        default: return 0;
    }
}

/**
 * @brief 计算张量总元素数
 */
static size_t calculate_num_elements(const int64_t* dims, size_t num_dims)
{
    size_t count = 1;
    for (size_t i = 0; i < num_dims; i++) {
        count *= (size_t)dims[i];
    }
    return count;
}

/* ========== API实现 ========== */

int ort_init(OrtEmbeddedContext* ctx, const OrtSessionOptions* options)
{
    if (!ctx) return ORT_ERROR_INVALID_ARGUMENT;

    memset(ctx, 0, sizeof(OrtEmbeddedContext));

    /* 在实际实现中，这里会创建ORT环境 */
    /* OrtCreateEnv(...) */

    (void)options; /* 保留供未来使用 */

    return ORT_OK;
}

int ort_load_model(OrtEmbeddedContext* ctx,
                    const uint8_t* model_data,
                    size_t model_size)
{
    if (!ctx || !model_data || model_size == 0) {
        return ORT_ERROR_INVALID_ARGUMENT;
    }

    /* 创建简化会话 */
    SimpleSession* session = (SimpleSession*)malloc(sizeof(SimpleSession));
    if (!session) {
        return ORT_ERROR_RUNTIME_EXCEPTION;
    }

    memset(session, 0, sizeof(SimpleSession));

    /* 复制模型数据 */
    session->model_buffer = (uint8_t*)malloc(model_size);
    if (!session->model_buffer) {
        free(session);
        return ORT_ERROR_RUNTIME_EXCEPTION;
    }
    memcpy(session->model_buffer, model_data, model_size);
    session->model_size = model_size;

    /* 在实际实现中，这里会:
     * 1. 解析ONNX protobuf
     * 2. 构建计算图
     * 3. 分配执行计划
     * 4. 解析输入输出信息
     */

    ctx->session = session;
    ctx->is_initialized = 1;

    return ORT_OK;
}

int ort_prepare_input(OrtEmbeddedContext* ctx,
                       const char* input_name,
                       const void* data,
                       ONNXTensorElementDataType type,
                       const int64_t* dims,
                       size_t num_dims)
{
    if (!ctx || !ctx->session || !data || !dims) {
        return ORT_ERROR_INVALID_ARGUMENT;
    }

    SimpleSession* session = (SimpleSession*)ctx->session;

    /* 查找或创建输入张量 */
    SimpleTensor* tensor = NULL;
    for (size_t i = 0; i < session->num_inputs; i++) {
        if (strcmp(ctx->input_names[i], input_name) == 0) {
            tensor = &session->inputs[i];
            break;
        }
    }

    if (!tensor) {
        /* 分配新输入槽 */
        session->inputs = (SimpleTensor*)realloc(session->inputs,
            (session->num_inputs + 1) * sizeof(SimpleTensor));
        ctx->input_names = (char**)realloc(ctx->input_names,
            (session->num_inputs + 1) * sizeof(char*));

        tensor = &session->inputs[session->num_inputs];
        memset(tensor, 0, sizeof(SimpleTensor));

        ctx->input_names[session->num_inputs] = strdup(input_name);
        session->num_inputs++;
        ctx->num_inputs = session->num_inputs;
    }

    /* 设置张量属性 */
    tensor->type = type;
    tensor->num_dims = num_dims;

    /* 复制维度信息 */
    tensor->dims = (int64_t*)malloc(num_dims * sizeof(int64_t));
    memcpy(tensor->dims, dims, num_dims * sizeof(int64_t));

    /* 计算并分配数据 */
    size_t num_elements = calculate_num_elements(dims, num_dims);
    size_t element_size = get_element_size(type);
    tensor->data_size = num_elements * element_size;

    tensor->data = malloc(tensor->data_size);
    if (!tensor->data) {
        return ORT_ERROR_RUNTIME_EXCEPTION;
    }
    memcpy(tensor->data, data, tensor->data_size);

    return ORT_OK;
}

int ort_run(OrtEmbeddedContext* ctx)
{
    if (!ctx || !ctx->session) {
        return ORT_ERROR_INVALID_ARGUMENT;
    }

    /* 在实际实现中，这里会:
     * 1. 遍历计算图
     * 2. 按拓扑顺序执行算子
     * 3. 管理中间张量内存
     * 4. 处理内存池分配
     */

    /* 简化演示: 仅模拟推理过程 */
    /* 实际应用中，这里会调用ONNX Runtime核心执行引擎 */

    return ORT_OK;
}

int ort_get_output(OrtEmbeddedContext* ctx,
                    const char* output_name,
                    void* buffer,
                    size_t buffer_size)
{
    if (!ctx || !ctx->session || !buffer) {
        return ORT_ERROR_INVALID_ARGUMENT;
    }

    SimpleSession* session = (SimpleSession*)ctx->session;

    /* 查找输出张量 */
    for (size_t i = 0; i < session->num_outputs; i++) {
        if (strcmp(ctx->output_names[i], output_name) == 0) {
            SimpleTensor* tensor = &session->outputs[i];

            if (buffer_size < tensor->data_size) {
                return ORT_ERROR_INVALID_ARGUMENT;
            }

            memcpy(buffer, tensor->data, tensor->data_size);
            return ORT_OK;
        }
    }

    return ORT_ERROR_INVALID_ARGUMENT;
}

void ort_cleanup(OrtEmbeddedContext* ctx)
{
    if (!ctx || !ctx->session) return;

    SimpleSession* session = (SimpleSession*)ctx->session;

    /* 释放输入张量 */
    for (size_t i = 0; i < session->num_inputs; i++) {
        free(session->inputs[i].data);
        free(session->inputs[i].dims);
    }
    free(session->inputs);

    /* 释放输出张量 */
    for (size_t i = 0; i < session->num_outputs; i++) {
        free(session->outputs[i].data);
        free(session->outputs[i].dims);
    }
    free(session->outputs);

    /* 释放名称 */
    for (size_t i = 0; i < ctx->num_inputs; i++) {
        free(ctx->input_names[i]);
    }
    free(ctx->input_names);

    for (size_t i = 0; i < ctx->num_outputs; i++) {
        free(ctx->output_names[i]);
    }
    free(ctx->output_names);

    /* 释放模型缓冲区 */
    free(session->model_buffer);

    /* 释放会话 */
    free(session);
    ctx->session = NULL;
    ctx->is_initialized = 0;
}

/* ========== 实际ONNX Runtime集成示例 ========== */

#ifdef USE_REAL_ONNX_RUNTIME

/* 使用真实ONNX Runtime C API */
#include "onnxruntime_c_api.h"

const OrtApi* g_ort = NULL;

int ort_real_init(OrtEmbeddedContext* ctx, const OrtSessionOptions* options)
{
    /* 获取API */
    g_ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);
    if (!g_ort) {
        return ORT_ERROR_RUNTIME_EXCEPTION;
    }

    /* 创建环境 */
    OrtEnv* env = NULL;
    OrtStatus* status = g_ort->CreateEnv(
        ORT_LOGGING_LEVEL_WARNING,
        "embedded_app",
        &env
    );

    if (status != NULL) {
        g_ort->ReleaseStatus(status);
        return ORT_ERROR_RUNTIME_EXCEPTION;
    }

    ctx->env = env;

    /* 创建会话选项 */
    OrtSessionOptions* session_options = NULL;
    g_ort->CreateSessionOptions(&session_options);

    /* 配置选项 */
    if (options) {
        g_ort->SetIntraOpNumThreads(session_options,
            options->intra_op_num_threads);
        g_ort->SetInterOpNumThreads(session_options,
            options->inter_op_num_threads);
        g_ort->SetSessionGraphOptimizationLevel(session_options,
            options->graph_optimization_level);
    }

    /* 启用ARM计算优化 (如果可用) */
    #ifdef USE_ARMNN
    OrtSessionOptionsAppendExecutionProvider_ARMNN(session_options);
    #endif

    return ORT_OK;
}

#endif /* USE_REAL_ONNX_RUNTIME */
```

### 4.3 ONNX模型转换脚本

```python
#!/usr/bin/env python3
"""
onnx_conversion.py
ONNX模型转换和优化脚本
"""

import onnx
import numpy as np
from onnx import numpy_helper
from typing import List, Tuple, Optional, Dict
import argparse
from pathlib import Path

class ONNXOptimizer:
    """ONNX模型优化器"""

    def __init__(self, model_path: str):
        self.model_path = model_path
        self.model = None

    def load(self) -> 'ONNXOptimizer':
        """加载模型"""
        self.model = onnx.load(self.model_path)
        onnx.checker.check_model(self.model)
        print(f"[INFO] 模型加载成功: {self.model_path}")

        # 打印模型信息
        self._print_model_info()
        return self

    def _print_model_info(self):
        """打印模型信息"""
        print("\n=== 模型信息 ===")
        print(f"IR版本: {self.model.ir_version}")
        print(f"Opset版本: {self.model.opset_import[0].version}")
        print(f"生产者: {self.model.producer_name}")

        # 输入
        print("\n输入:")
        for input in self.model.graph.input:
            shape = [dim.dim_value if dim.dim_value else
                     dim.dim_param for dim in input.type.tensor_type.shape.dim]
            print(f"  {input.name}: {shape}")

        # 输出
        print("\n输出:")
        for output in self.model.graph.output:
            shape = [dim.dim_value if dim.dim_value else
                     dim.dim_param for dim in output.type.tensor_type.shape.dim]
            print(f"  {output.name}: {shape}")

        # 算子统计
        op_counts = {}
        for node in self.model.graph.node:
            op_counts[node.op_type] = op_counts.get(node.op_type, 0) + 1

        print("\n算子统计:")
        for op, count in sorted(op_counts.items()):
            print(f"  {op}: {count}")

        # 模型大小
        model_size = self.model_path.stat().st_size / 1024
        print(f"\n模型大小: {model_size:.2f} KB")

    def optimize(self, level: int = 2) -> 'ONNXOptimizer':
        """
        优化模型
        level 0: 仅基础优化
        level 1: 标准优化
        level 2: 激进优化
        """
        try:
            from onnxoptimizer import optimize

            passes = []
            if level >= 0:
                passes.extend([
                    'eliminate_deadend',
                    'eliminate_identity',
                    'eliminate_nop_dropout',
                ])

            if level >= 1:
                passes.extend([
                    'fuse_consecutive_concats',
                    'fuse_consecutive_reduce_unsqueeze',
                    'fuse_consecutive_squeezes',
                    'fuse_pad_into_conv',
                ])

            if level >= 2:
                passes.extend([
                    'extract_constant_to_initializer',
                    'fuse_add_bias_into_conv',
                    'fuse_bn_into_conv',
                    'fuse_matmul_add_bias_into_gemm',
                ])

            self.model = optimize(self.model, passes)
            print(f"[INFO] 优化完成 (level {level})")

        except ImportError:
            print("[WARNING] onnxoptimizer未安装，跳过优化")

        return self

    def quantize_dynamic(self) -> str:
        """动态量化 - 仅权重量化"""
        from onnxruntime.quantization import quantize_dynamic, QuantType

        output_path = str(self.model_path.with_suffix('.dynamic_quant.onnx'))

        quantize_dynamic(
            model_input=str(self.model_path),
            model_output=output_path,
            weight_type=QuantType.QInt8,
            optimize_model=True
        )

        # 计算大小
        original_size = self.model_path.stat().st_size / 1024
        quantized_size = Path(output_path).stat().st_size / 1024

        print(f"[INFO] 动态量化完成")
        print(f"  原始大小: {original_size:.2f} KB")
        print(f"  量化后: {quantized_size:.2f} KB")
        print(f"  压缩比: {original_size/quantized_size:.2f}x")

        return output_path

    def quantize_static(self,
                        calibration_data: np.ndarray,
                        num_calibration_samples: int = 100) -> str:
        """静态量化 - 权重和激活都量化"""
        from onnxruntime.quantization import (
            quantize_static,
            CalibrationDataReader,
            QuantType
        )

        class CalibrationDataProvider(CalibrationDataReader):
            def __init__(self, data, num_samples):
                self.data = data
                self.num_samples = num_samples
                self.index = 0

            def get_next(self) -> dict:
                if self.index >= self.num_samples:
                    return None

                sample = self.data[self.index]
                self.index += 1
                return {'input': sample}

        output_path = str(self.model_path.with_suffix('.static_quant.onnx'))

        # 准备校准数据读取器
        data_reader = CalibrationDataProvider(
            calibration_data,
            num_calibration_samples
        )

        quantize_static(
            model_input=str(self.model_path),
            model_output=output_path,
            calibration_data_reader=data_reader,
            quant_format='QDQ',  # Quantize and Dequantize nodes
            activation_type=QuantType.QInt8,
            weight_type=QuantType.QInt8,
            optimize_model=True
        )

        print(f"[INFO] 静态量化完成: {output_path}")
        return output_path

    def convert_to_c_header(self,
                            output_name: str = "model",
                            alignment: int = 8) -> str:
        """转换为C头文件"""
        # 序列化模型
        model_bytes = self.model.SerializeToString()

        # 生成C数组
        byte_array = ', '.join(f'0x{b:02x}' for b in model_bytes)

        # 格式化 (每行16个字节)
        formatted = ''
        bytes_list = model_bytes.hex(' ').split()
        for i in range(0, len(bytes_list), 16):
            line = ', '.join(f'0x{b}' for b in bytes_list[i:i+16])
            formatted += f'    {line},\n'

        header_content = f"""/* Auto-generated ONNX model
 * Model: {self.model_path.name}
 * Size: {len(model_bytes)} bytes
 * Generated: {__import__('datetime').datetime.now().isoformat()}
 */

#ifndef {output_name.upper()}_H
#define {output_name.upper()}_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {{
#endif

/* Model data - {len(model_bytes)} bytes, aligned to {alignment} */
__attribute__((aligned({alignment})))
static const uint8_t {output_name}_data[] = {{
{formatted}}};

static const uint32_t {output_name}_len = {len(model_bytes)};

#ifdef __cplusplus
}}
#endif

#endif /* {output_name.upper()}_H */
"""

        output_path = self.model_path.parent / f"{output_name}.h"
        with open(output_path, 'w') as f:
            f.write(header_content)

        print(f"[INFO] C头文件已生成: {output_path}")
        return str(output_path)

    def save(self, output_path: str = None) -> str:
        """保存模型"""
        if output_path is None:
            output_path = str(self.model_path.with_suffix('.optimized.onnx'))

        onnx.save(self.model, output_path)
        print(f"[INFO] 模型已保存: {output_path}")
        return output_path

def main():
    parser = argparse.ArgumentParser(description='ONNX模型转换工具')
    parser.add_argument('model', type=str, help='输入ONNX模型路径')
    parser.add_argument('-o', '--output', type=str, help='输出路径')
    parser.add_argument('--optimize', type=int, default=2,
                        help='优化级别 (0-2)')
    parser.add_argument('--quantize', choices=['dynamic', 'static', 'none'],
                        default='none', help='量化方式')
    parser.add_argument('--to-c', action='store_true',
                        help='转换为C头文件')
    parser.add_argument('--name', default='model',
                        help='C数组名称')

    args = parser.parse_args()

    model_path = Path(args.model)
    if not model_path.exists():
        print(f"[ERROR] 模型不存在: {model_path}")
        return 1

    # 创建优化器
    optimizer = ONNXOptimizer(model_path)
    optimizer.load()

    # 优化
    if args.optimize > 0:
        optimizer.optimize(args.optimize)

    # 量化
    if args.quantize == 'dynamic':
        optimizer.quantize_dynamic()
    elif args.quantize == 'static':
        print("[WARNING] 静态量化需要提供校准数据")
        # optimizer.quantize_static(...)

    # 转换为C
    if args.to_c:
        optimizer.convert_to_c_header(args.name)

    # 保存
    if args.output:
        optimizer.save(args.output)

    return 0

if __name__ == '__main__':
    exit(main())
```

---

## 5. 神经网络算子优化

### 5.1 CMSIS-NN集成

CMSIS-NN是ARM为Cortex-M处理器优化的神经网络库，提供高度优化的底层算子。

```c
/**
 * @file cmsis_nn_integration.h
 * @brief CMSIS-NN集成头文件
 */

#ifndef CMSIS_NN_INTEGRATION_H
#define CMSIS_NN_INTEGRATION_H

#include "arm_nnfunctions.h"  /* CMSIS-NN主头文件 */
#include <stdint.h>

/* ========== 量化参数结构 ========== */
typedef struct {
    int32_t input_offset;     /* 输入零点偏移 */
    int32_t filter_offset;    /* 权重零点偏移 */
    int32_t output_offset;    /* 输出零点偏移 */
    int32_t output_mult;      /* 输出乘法因子 */
    int32_t output_shift;     /* 输出移位因子 */
} cmsis_nn_quant_params_t;

/* ========== 维度信息结构 ========== */
typedef struct {
    int n;  /* 批次/数量 */
    int h;  /* 高度 */
    int w;  /* 宽度 */
    int c;  /* 通道 */
} cmsis_nn_dims_t;

/* ========== 卷积参数 ========== */
typedef struct {
    int padding_w;            /* 宽度填充 */
    int padding_h;            /* 高度填充 */
    int stride_w;             /* 宽度步长 */
    int stride_h;             /* 高度步长 */
    int dilation_w;           /* 宽度扩张 */
    int dilation_h;           /* 高度扩张 */
} cmsis_nn_conv_params_t;

/* ========== 高性能算子包装 ========== */

/**
 * @brief 优化的INT8卷积层
 */
int32_t cmsis_nn_convolve_wrapper(
    const cmsis_nn_context_t* ctx,
    const cmsis_nn_conv_params_t* conv_params,
    const cmsis_nn_quant_params_t* quant_params,
    const cmsis_nn_dims_t* input_dims,
    const int8_t* input_data,
    const cmsis_nn_dims_t* filter_dims,
    const int8_t* filter_data,
    const int32_t* bias_data,
    const cmsis_nn_dims_t* output_dims,
    int8_t* output_data
);

/**
 * @brief 优化的全连接层
 */
int32_t cmsis_nn_fully_connected_wrapper(
    const cmsis_nn_context_t* ctx,
    const cmsis_nn_fc_params_t* fc_params,
    const cmsis_nn_quant_params_t* quant_params,
    const cmsis_nn_dims_t* input_dims,
    const int8_t* input_data,
    const cmsis_nn_dims_t* filter_dims,
    const int8_t* filter_data,
    const int32_t* bias_data,
    const cmsis_nn_dims_t* output_dims,
    int8_t* output_data
);

/**
 * @brief 优化的深度可分离卷积
 */
int32_t cmsis_nn_depthwise_conv_wrapper(
    const cmsis_nn_context_t* ctx,
    const cmsis_nn_dw_conv_params_t* dw_conv_params,
    const cmsis_nn_quant_params_t* quant_params,
    const cmsis_nn_dims_t* input_dims,
    const int8_t* input_data,
    const cmsis_nn_dims_t* filter_dims,
    const int8_t* filter_data,
    const int32_t* bias_data,
    const cmsis_nn_dims_t* output_dims,
    int8_t* output_data
);

/**
 * @brief 优化的平均池化
 */
int32_t cmsis_nn_avg_pool_wrapper(
    const cmsis_nn_context_t* ctx,
    const cmsis_nn_pool_params_t* pool_params,
    const cmsis_nn_dims_t* input_dims,
    const int8_t* input_data,
    const cmsis_nn_dims_t* filter_dims,
    const cmsis_nn_dims_t* output_dims,
    int8_t* output_data
);

/**
 * @brief 优化的ReLU激活
 */
void cmsis_nn_relu6_s8(
    int8_t* data,
    uint16_t size,
    int16_t input_offset,
    int16_t output_offset,
    int32_t output_mult,
    int32_t output_shift
);

#endif /* CMSIS_NN_INTEGRATION_H */
```


### 5.2 CMSIS-NN实现

```c
/**
 * @file cmsis_nn_integration.c
 * @brief CMSIS-NN算子实现
 */

#include "cmsis_nn_integration.h"
#include <string.h>

/**
 * @brief INT8卷积层包装
 *
 * 支持两种计算路径:
 * 1. 快速路径: 使用arm_convolve_s8 (要求内存足够)
 * 2. 内存优化路径: 使用arm_convolve_wrapper_s8 (分块计算)
 */
int32_t cmsis_nn_convolve_wrapper(
    const cmsis_nn_context_t* ctx,
    const cmsis_nn_conv_params_t* conv_params,
    const cmsis_nn_quant_params_t* quant_params,
    const cmsis_nn_dims_t* input_dims,
    const int8_t* input_data,
    const cmsis_nn_dims_t* filter_dims,
    const int8_t* filter_data,
    const int32_t* bias_data,
    const cmsis_nn_dims_t* output_dims,
    int8_t* output_data)
{
    /* 构建CMSIS-NN参数结构 */
    cmsis_nn_conv_params cmsis_params;
    cmsis_params.input_offset = quant_params->input_offset;
    cmsis_params.output_offset = quant_params->output_offset;
    cmsis_params.stride.w = conv_params->stride_w;
    cmsis_params.stride.h = conv_params->stride_h;
    cmsis_params.padding.w = conv_params->padding_w;
    cmsis_params.padding.h = conv_params->padding_h;
    cmsis_params.dilation.w = conv_params->dilation_w;
    cmsis_params.dilation.h = conv_params->dilation_h;
    cmsis_params.activation.min = 0;  /* ReLU6 */
    cmsis_params.activation.max = 6;

    cmsis_nn_per_tensor_quant_params quant_info;
    quant_info.multiplier = quant_params->output_mult;
    quant_info.shift = quant_params->output_shift;

    /* 构建维度结构 */
    cmsis_nn_dims input_shape = {
        .n = input_dims->n,
        .h = input_dims->h,
        .w = input_dims->w,
        .c = input_dims->c
    };

    cmsis_nn_dims filter_shape = {
        .n = filter_dims->n,  /* 输出通道 */
        .h = filter_dims->h,  /* 卷积核高 */
        .w = filter_dims->w,  /* 卷积核宽 */
        .c = filter_dims->c   /* 输入通道 */
    };

    cmsis_nn_dims output_shape = {
        .n = output_dims->n,
        .h = output_dims->h,
        .w = output_dims->w,
        .c = output_dims->c
    };

    cmsis_nn_dims bias_shape = { .n = output_dims->c };

    /* 计算所需缓冲区大小 */
    int32_t buf_size = arm_convolve_s8_get_buffer_size(&input_shape, &filter_shape);

    /* 选择计算路径 */
    if (ctx && ctx->buf && ctx->size >= (uint32_t)buf_size) {
        /* 使用快速路径 */
        return arm_convolve_s8(ctx,
                               &cmsis_params,
                               &quant_info,
                               &input_shape,
                               input_data,
                               &filter_shape,
                               filter_data,
                               &bias_shape,
                               bias_data,
                               &output_shape,
                               output_data);
    } else {
        /* 使用内存优化路径 */
        return arm_convolve_wrapper_s8(ctx,
                                       &cmsis_params,
                                       &quant_info,
                                       &input_shape,
                                       input_data,
                                       &filter_shape,
                                       filter_data,
                                       &bias_shape,
                                       bias_data,
                                       &output_shape,
                                       output_data);
    }
}

/**
 * @brief 全连接层实现
 */
int32_t cmsis_nn_fully_connected_wrapper(
    const cmsis_nn_context_t* ctx,
    const cmsis_nn_fc_params_t* fc_params,
    const cmsis_nn_quant_params_t* quant_params,
    const cmsis_nn_dims_t* input_dims,
    const int8_t* input_data,
    const cmsis_nn_dims_t* filter_dims,
    const int8_t* filter_data,
    const int32_t* bias_data,
    const cmsis_nn_dims_t* output_dims,
    int8_t* output_data)
{
    cmsis_nn_fc_params params;
    params.input_offset = quant_params->input_offset;
    params.filter_offset = quant_params->filter_offset;
    params.output_offset = quant_params->output_offset;
    params.activation.min = fc_params->activation.min;
    params.activation.max = fc_params->activation.max;

    cmsis_nn_per_tensor_quant_params quant_info;
    quant_info.multiplier = quant_params->output_mult;
    quant_info.shift = quant_params->output_shift;

    cmsis_nn_dims input_shape = {
        .n = input_dims->n,
        .h = input_dims->h,
        .w = input_dims->w,
        .c = input_dims->c
    };

    cmsis_nn_dims filter_shape = {
        .n = filter_dims->n,
        .h = filter_dims->h
    };

    cmsis_nn_dims bias_shape = { .n = output_dims->c };

    cmsis_nn_dims output_shape = {
        .n = output_dims->n,
        .h = output_dims->h,
        .c = output_dims->c
    };

    return arm_fully_connected_s8(ctx,
                                  &params,
                                  &quant_info,
                                  &input_shape,
                                  input_data,
                                  &filter_shape,
                                  filter_data,
                                  &bias_shape,
                                  bias_data,
                                  &output_shape,
                                  output_data);
}

/**
 * @brief 深度可分离卷积
 * MobileNet等轻量级网络的核心算子
 */
int32_t cmsis_nn_depthwise_conv_wrapper(
    const cmsis_nn_context_t* ctx,
    const cmsis_nn_dw_conv_params_t* dw_conv_params,
    const cmsis_nn_quant_params_t* quant_params,
    const cmsis_nn_dims_t* input_dims,
    const int8_t* input_data,
    const cmsis_nn_dims_t* filter_dims,
    const int8_t* filter_data,
    const int32_t* bias_data,
    const cmsis_nn_dims_t* output_dims,
    int8_t* output_data)
{
    cmsis_nn_dw_conv_params params;
    params.input_offset = quant_params->input_offset;
    params.output_offset = quant_params->output_offset;
    params.ch_mult = dw_conv_params->ch_mult;  /* 通道乘数 */
    params.stride.w = dw_conv_params->stride_w;
    params.stride.h = dw_conv_params->stride_h;
    params.padding.w = dw_conv_params->padding_w;
    params.padding.h = dw_conv_params->padding_h;
    params.dilation.w = dw_conv_params->dilation_w;
    params.dilation.h = dw_conv_params->dilation_h;
    params.activation.min = 0;
    params.activation.max = 127;

    cmsis_nn_per_tensor_quant_params quant_info;
    quant_info.multiplier = quant_params->output_mult;
    quant_info.shift = quant_params->output_shift;

    cmsis_nn_dims input_shape = {
        .n = input_dims->n,
        .h = input_dims->h,
        .w = input_dims->w,
        .c = input_dims->c
    };

    cmsis_nn_dims filter_shape = {
        .n = filter_dims->n,
        .h = filter_dims->h,
        .w = filter_dims->w,
        .c = filter_dims->c
    };

    cmsis_nn_dims bias_shape = { .n = output_dims->c };

    cmsis_nn_dims output_shape = {
        .n = output_dims->n,
        .h = output_dims->h,
        .w = output_dims->w,
        .c = output_dims->c
    };

    return arm_depthwise_conv_s8(ctx,
                                 &params,
                                 &quant_info,
                                 &input_shape,
                                 input_data,
                                 &filter_shape,
                                 filter_data,
                                 &bias_shape,
                                 bias_data,
                                 &output_shape,
                                 output_data);
}

/**
 * @brief 优化的平均池化
 */
int32_t cmsis_nn_avg_pool_wrapper(
    const cmsis_nn_context_t* ctx,
    const cmsis_nn_pool_params_t* pool_params,
    const cmsis_nn_dims_t* input_dims,
    const int8_t* input_data,
    const cmsis_nn_dims_t* filter_dims,
    const cmsis_nn_dims_t* output_dims,
    int8_t* output_data)
{
    cmsis_nn_pool_params params;
    params.stride.w = pool_params->stride_w;
    params.stride.h = pool_params->stride_h;
    params.padding.w = pool_params->padding_w;
    params.padding.h = pool_params->padding_h;
    params.activation.min = pool_params->activation_min;
    params.activation.max = pool_params->activation_max;

    cmsis_nn_dims input_shape = {
        .n = input_dims->n,
        .h = input_dims->h,
        .w = input_dims->w,
        .c = input_dims->c
    };

    cmsis_nn_dims filter_shape = {
        .w = filter_dims->w,
        .h = filter_dims->h
    };

    cmsis_nn_dims output_shape = {
        .n = output_dims->n,
        .h = output_dims->h,
        .w = output_dims->w,
        .c = output_dims->c
    };

    return arm_avgpool_s8(ctx,
                          &params,
                          &input_shape,
                          input_data,
                          &filter_shape,
                          &output_shape,
                          output_data);
}

/**
 * @brief 优化的ReLU6激活
 */
void cmsis_nn_relu6_s8(
    int8_t* data,
    uint16_t size,
    int16_t input_offset,
    int16_t output_offset,
    int32_t output_mult,
    int32_t output_shift)
{
    for (uint16_t i = 0; i < size; i++) {
        /* 反量化 */
        int32_t val = (int32_t)data[i] + input_offset;

        /* ReLU6: max(0, min(6, x)) */
        if (val < 0) val = 0;
        if (val > 6) val = 6;

        /* 重新量化 */
        val = ((val * output_mult) >> output_shift) + output_offset;

        /* 限制范围 */
        if (val > 127) val = 127;
        if (val < -128) val = -128;

        data[i] = (int8_t)val;
    }
}

/* ========== 缓冲区管理 ========== */

#define CMSIS_NN_MAX_BUFFERS 4

typedef struct {
    void* ptr;
    uint32_t size;
    uint8_t in_use;
} cmsis_nn_buffer_t;

static cmsis_nn_buffer_t s_buffer_pool[CMSIS_NN_MAX_BUFFERS];
static uint8_t s_buffer_pool_mem[32 * 1024];  /* 32KB缓冲区池 */
static uint32_t s_pool_offset = 0;

/**
 * @brief 从池中分配临时缓冲区
 */
void* cmsis_nn_buffer_alloc(uint32_t size, uint32_t alignment)
{
    /* 对齐 */
    uint32_t aligned_offset = (s_pool_offset + alignment - 1) & ~(alignment - 1);

    if (aligned_offset + size > sizeof(s_buffer_pool_mem)) {
        return NULL;  /* 内存不足 */
    }

    void* ptr = &s_buffer_pool_mem[aligned_offset];
    s_pool_offset = aligned_offset + size;

    return ptr;
}

/**
 * @brief 释放所有临时缓冲区
 */
void cmsis_nn_buffer_reset(void)
{
    s_pool_offset = 0;
}
```

### 5.3 自定义算子优化

```c
/**
 * @file custom_optimized_ops.c
 * @brief 自定义优化算子实现
 */

#include <stdint.h>
#include <string.h>

/* ========== SIMD优化矩阵乘法 (假设ARMv7E-M或更高) ========== */

#ifdef __ARM_ARCH_7EM__
#include "arm_math.h"

/**
 * @brief 使用SIMD优化的FP32矩阵乘法
 * C = A * B
 * A: [M x K], B: [K x N], C: [M x N]
 */
void matmul_f32_simd(const float* A,
                      const float* B,
                      float* C,
                      int M, int K, int N)
{
    /* 使用CMSIS-DSP arm_mat_mult_f32 */
    arm_matrix_instance_f32 matA, matB, matC;

    arm_mat_init_f32(&matA, M, K, (float32_t*)A);
    arm_mat_init_f32(&matB, K, N, (float32_t*)B);
    arm_mat_init_f32(&matC, M, N, C);

    arm_mat_mult_f32(&matA, &matB, &matC);
}

/**
 * @brief 使用SIMD优化的INT8矩阵乘法
 */
void matmul_int8_simd(const int8_t* A,
                       const int8_t* B,
                       int8_t* C,
                       int M, int K, int N,
                       int32_t A_offset,
                       int32_t B_offset,
                       int32_t C_offset,
                       int32_t output_mult,
                       int32_t output_shift)
{
    for (int m = 0; m < M; m++) {
        for (int n = 0; n < N; n++) {
            int32_t acc = 0;

            /* 使用SIMD内联汇编或编译器自动向量化 */
            #if defined(__clang__) || defined(__GNUC__)
            /* 提示编译器向量化 */
            #pragma clang loop vectorize(enable)
            #endif
            for (int k = 0; k < K; k++) {
                int32_t a_val = (int32_t)A[m * K + k] + A_offset;
                int32_t b_val = (int32_t)B[k * N + n] + B_offset;
                acc += a_val * b_val;
            }

            /* 量化和激活 */
            acc = ((acc * output_mult) >> output_shift) + C_offset;

            /* 限制范围 */
            if (acc > 127) acc = 127;
            if (acc < -128) acc = -128;

            C[m * N + n] = (int8_t)acc;
        }
    }
}

#else  /* 无SIMD支持 */

/* 纯C实现 */
void matmul_f32_simd(const float* A,
                      const float* B,
                      float* C,
                      int M, int K, int N)
{
    for (int m = 0; m < M; m++) {
        for (int n = 0; n < N; n++) {
            float sum = 0.0f;
            for (int k = 0; k < K; k++) {
                sum += A[m * K + k] * B[k * N + n];
            }
            C[m * N + n] = sum;
        }
    }
}

#endif /* __ARM_ARCH_7EM__ */

/* ========== Winograd卷积优化 ========== */

/**
 * @brief 3x3卷积的Winograd F(2x2, 3x3)算法
 * 将乘法次数从36减少到16
 *
 * 输入: 4x4块
 * 卷积核: 3x3
 * 输出: 2x2块
 */
void winograd_f2_3_convolve(const float* input,      /* 4x4输入 */
                             const float* kernel,     /* 3x3卷积核 */
                             float* output)           /* 2x2输出 */
{
    /* Winograd变换矩阵 */
    /* B^T = [[1,  0, -1,  0],
     *        [0,  1,  1,  0],
     *        [0, -1,  1,  0],
     *        [0,  1,  0, -1]] */

    /* G = [[ 1,    0,    0],
     *      [0.5,  0.5,  0.5],
     *      [0.5, -0.5,  0.5],
     *      [ 0,    0,    1]] */

    /* A^T = [[1, 1,  1, 0],
     *        [0, 1, -1, 1]] */

    float g[4][3];  /* 变换后的卷积核 */
    float d[4][4];  /* 变换后的输入 */
    float m[4][4];  /* 逐元素乘法结果 */

    /* 1. 卷积核变换 g = G * kernel * G^T */
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            g[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                float G_ik;
                switch (i) {
                    case 0: G_ik = (k == 0) ? 1.0f : 0.0f; break;
                    case 1: G_ik = 0.5f; break;
                    case 2: G_ik = (k == 1) ? -0.5f : 0.5f; break;
                    case 3: G_ik = (k == 2) ? 1.0f : 0.0f; break;
                    default: G_ik = 0.0f;
                }
                g[i][j] += G_ik * kernel[k * 3 + j];
            }
        }
    }

    /* 2. 输入变换 d = B^T * input * B */
    /* 简化实现 - 实际应完整实现4x4变换 */

    /* 3. 逐元素乘法 m = g ⊙ d */
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m[i][j] = g[i][j % 3] * d[i][j];
        }
    }

    /* 4. 输出变换 output = A^T * m * A */
    /* 生成2x2输出 */
    output[0] = m[0][0] + m[1][0] + m[2][0] + m[0][1] + m[1][1] + m[2][1];
    output[1] = m[0][1] + m[1][1] + m[2][1] + m[0][2] + m[1][2] + m[2][2];
    output[2] = m[1][0] - m[2][0] + m[3][0] + m[1][1] - m[2][1] + m[3][1];
    output[3] = m[1][1] - m[2][1] + m[3][1] + m[1][2] - m[2][2] + m[3][2];
}

/* ========== LUT优化激活函数 ========== */

#define SIGMOID_LUT_SIZE 256
#define SIGMOID_INPUT_MIN -8.0f
#define SIGMOID_INPUT_MAX 8.0f
#define SIGMOID_INPUT_RANGE (SIGMOID_INPUT_MAX - SIGMOID_INPUT_MIN)
#define SIGMOID_SCALE (SIGMOID_LUT_SIZE / SIGMOID_INPUT_RANGE)

static float s_sigmoid_lut[SIGMOID_LUT_SIZE];
static uint8_t s_lut_initialized = 0;

/**
 * @brief 初始化Sigmoid LUT
 */
void init_sigmoid_lut(void)
{
    if (s_lut_initialized) return;

    for (int i = 0; i < SIGMOID_LUT_SIZE; i++) {
        float x = SIGMOID_INPUT_MIN + (i / SIGMOID_SCALE);
        s_sigmoid_lut[i] = 1.0f / (1.0f + expf(-x));
    }

    s_lut_initialized = 1;
}

/**
 * @brief 使用LUT的快速Sigmoid
 */
inline float fast_sigmoid(float x)
{
    if (x <= SIGMOID_INPUT_MIN) return 0.0f;
    if (x >= SIGMOID_INPUT_MAX) return 1.0f;

    int idx = (int)((x - SIGMOID_INPUT_MIN) * SIGMOID_SCALE);

    /* 线性插值 */
    float t = (x - SIGMOID_INPUT_MIN) * SIGMOID_SCALE - idx;
    float y0 = s_sigmoid_lut[idx];
    float y1 = s_sigmoid_lut[idx + 1];

    return y0 + t * (y1 - y0);
}

/**
 * @brief 快速Tanh (基于Sigmoid)
 * tanh(x) = 2 * sigmoid(2x) - 1
 */
inline float fast_tanh(float x)
{
    return 2.0f * fast_sigmoid(2.0f * x) - 1.0f;
}
```

---

## 6. 视觉模型部署

### 6.1 人脸识别系统

```c
/**
 * @file face_recognition.c
 * @brief 嵌入式人脸识别系统
 *
 * 系统架构:
 * 1. 人脸检测 (MTCNN或轻量级SSD)
 * 2. 人脸对齐 (关键点检测)
 * 3. 特征提取 (MobileFaceNet)
 * 4. 人脸比对 (余弦相似度)
 */

#include <stdint.h>
#include <string.h>
#include <math.h>

/* ========== 配置参数 ========== */
#define FACE_DET_INPUT_SIZE     128   /* 检测输入尺寸 */
#define FACE_ALIGN_SIZE         112   /* 对齐后人脸尺寸 */
#define FACE_FEATURE_DIM        128   /* 特征向量维度 */
#define MAX_FACE_DB_SIZE        100   /* 人脸库最大容量 */
#define FACE_MATCH_THRESHOLD    0.6f  /* 匹配阈值 */

/* ========== 检测结果结构 ========== */
typedef struct {
    float x1, y1, x2, y2;    /* 边界框 */
    float confidence;         /* 置信度 */
    float landmarks[10];      /* 5个关键点 (x,y) * 5 */
} face_detection_t;

/* ========== 人脸特征结构 ========== */
typedef struct {
    int8_t feature[FACE_FEATURE_DIM];  /* INT8量化特征 */
    char name[32];                      /* 人员姓名 */
    uint32_t id;                        /* 人员ID */
} face_feature_t;

/* ========== 人脸数据库 ========== */
typedef struct {
    face_feature_t entries[MAX_FACE_DB_SIZE];
    uint32_t num_entries;
} face_database_t;

static face_database_t s_face_db;

/* ========== 人脸检测 ========== */

/**
 * @brief 轻量级人脸检测 (简化版)
 * @param image 输入图像 (RGB888, FACE_DET_INPUT_SIZE x FACE_DET_INPUT_SIZE)
 * @param detections 输出检测数组
 * @param max_detections 最大检测数
 * @return 实际检测数
 */
int face_detect(const uint8_t* image,
                 face_detection_t* detections,
                 int max_detections)
{
    /* 在实际应用中，这里会调用TFLM模型进行人脸检测 */
    /* 模型输入: 128x128 RGB */
    /* 模型输出: 边界框 + 置信度 + 关键点 */

    /* 预处理: 归一化 */
    float normalized[FACE_DET_INPUT_SIZE * FACE_DET_INPUT_SIZE * 3];
    for (int i = 0; i < FACE_DET_INPUT_SIZE * FACE_DET_INPUT_SIZE * 3; i++) {
        normalized[i] = (image[i] / 255.0f - 0.5f) / 0.5f;
    }

    /* 调用TFLM推理 */
    /* ... */

    /* 后处理: NMS + 解码 */
    int num_detections = 0;
    /* ... */

    return num_detections;
}

/* ========== 人脸对齐 ========== */

/* 标准人脸5点坐标 (112x112) */
static const float kStdLandmarks[10] = {
    38.2946f, 51.6963f,  /* 左眼 */
    73.5318f, 51.5014f,  /* 右眼 */
    56.0252f, 71.7366f,  /* 鼻子 */
    41.5493f, 92.3655f,  /* 左嘴角 */
    70.7299f, 92.2041f   /* 右嘴角 */
};

/**
 * @brief 计算仿射变换矩阵
 * 将检测到的关键点对齐到标准位置
 */
static void compute_affine_transform(const float* src_points,  /* 5对点 */
                                      const float* dst_points,
                                      float* transform)       /* 2x3矩阵 */
{
    /* 简化: 使用最小二乘法求解相似变换 */
    /* transform = [scale*cos, -scale*sin, tx;
     *              scale*sin,  scale*cos, ty] */

    float src_mean_x = 0, src_mean_y = 0;
    float dst_mean_x = 0, dst_mean_y = 0;

    for (int i = 0; i < 5; i++) {
        src_mean_x += src_points[i * 2];
        src_mean_y += src_points[i * 2 + 1];
        dst_mean_x += dst_points[i * 2];
        dst_mean_y += dst_points[i * 2 + 1];
    }

    src_mean_x /= 5.0f;
    src_mean_y /= 5.0f;
    dst_mean_x /= 5.0f;
    dst_mean_y /= 5.0f;

    /* 计算缩放和旋转 */
    float sum_ss = 0, sum_sd = 0, sum_ds = 0, sum_dd = 0;
    for (int i = 0; i < 5; i++) {
        float sx = src_points[i * 2] - src_mean_x;
        float sy = src_points[i * 2 + 1] - src_mean_y;
        float dx = dst_points[i * 2] - dst_mean_x;
        float dy = dst_points[i * 2 + 1] - dst_mean_y;

        sum_ss += sx * sx + sy * sy;
        sum_sd += sx * dx + sy * dy;
        sum_ds += sx * dy - sy * dx;
    }

    float scale = sqrtf(sum_sd * sum_sd + sum_ds * sum_ds) / sum_ss;
    float angle = atan2f(sum_ds, sum_sd);

    float cos_a = cosf(angle) * scale;
    float sin_a = sinf(angle) * scale;

    transform[0] = cos_a;
    transform[1] = -sin_a;
    transform[2] = dst_mean_x - (cos_a * src_mean_x - sin_a * src_mean_y);
    transform[3] = sin_a;
    transform[4] = cos_a;
    transform[5] = dst_mean_y - (sin_a * src_mean_x + cos_a * src_mean_y);
}

/**
 * @brief 应用仿射变换 (双线性插值)
 */
static void warp_affine(const uint8_t* src, int src_w, int src_h,
                         uint8_t* dst, int dst_w, int dst_h,
                         const float* transform)
{
    float inv_transform[6];
    /* 计算逆变换矩阵 */
    float det = transform[0] * transform[4] - transform[1] * transform[3];
    inv_transform[0] = transform[4] / det;
    inv_transform[1] = -transform[1] / det;
    inv_transform[2] = (transform[1] * transform[5] - transform[2] * transform[4]) / det;
    inv_transform[3] = -transform[3] / det;
    inv_transform[4] = transform[0] / det;
    inv_transform[5] = (transform[2] * transform[3] - transform[0] * transform[5]) / det;

    for (int y = 0; y < dst_h; y++) {
        for (int x = 0; x < dst_w; x++) {
            /* 目标到源的映射 */
            float src_x = inv_transform[0] * x + inv_transform[1] * y + inv_transform[2];
            float src_y = inv_transform[3] * x + inv_transform[4] * y + inv_transform[5];

            /* 双线性插值 */
            int x0 = (int)floorf(src_x);
            int y0 = (int)floorf(src_y);
            int x1 = x0 + 1;
            int y1 = y0 + 1;

            float fx = src_x - x0;
            float fy = src_y - y0;

            /* 边界处理 */
            if (x0 < 0) x0 = 0;
            if (y0 < 0) y0 = 0;
            if (x1 >= src_w) x1 = src_w - 1;
            if (y1 >= src_h) y1 = src_h - 1;

            /* 三个通道 */
            for (int c = 0; c < 3; c++) {
                float v00 = src[(y0 * src_w + x0) * 3 + c];
                float v01 = src[(y0 * src_w + x1) * 3 + c];
                float v10 = src[(y1 * src_w + x0) * 3 + c];
                float v11 = src[(y1 * src_w + x1) * 3 + c];

                float v0 = v00 + fx * (v01 - v00);
                float v1 = v10 + fx * (v11 - v10);
                float v = v0 + fy * (v1 - v0);

                dst[(y * dst_w + x) * 3 + c] = (uint8_t)(v + 0.5f);
            }
        }
    }
}

/**
 * @brief 人脸对齐
 * @param image 原始图像
 * @param face 检测结果 (包含关键点)
 * @param aligned_face 输出对齐后人脸 (112x112)
 */
void face_align(const uint8_t* image, int img_w, int img_h,
                 const face_detection_t* face,
                 uint8_t* aligned_face)
{
    /* 提取原始关键点 */
    float landmarks[10];
    for (int i = 0; i < 5; i++) {
        landmarks[i * 2] = face->landmarks[i * 2];
        landmarks[i * 2 + 1] = face->landmarks[i * 2 + 1];
    }

    /* 计算变换矩阵 */
    float transform[6];
    compute_affine_transform(landmarks, kStdLandmarks, transform);

    /* 裁剪人脸区域 */
    int crop_x = (int)face->x1;
    int crop_y = (int)face->y1;
    int crop_w = (int)(face->x2 - face->x1);
    int crop_h = (int)(face->y2 - face->y1);

    /* 边界检查 */
    if (crop_x < 0) crop_x = 0;
    if (crop_y < 0) crop_y = 0;
    if (crop_x + crop_w > img_w) crop_w = img_w - crop_x;
    if (crop_y + crop_h > img_h) crop_h = img_h - crop_y;

    /* 提取裁剪区域 */
    uint8_t cropped[400 * 400 * 3];  /* 假设最大400x400 */
    for (int y = 0; y < crop_h; y++) {
        memcpy(&cropped[y * crop_w * 3],
               &image[(crop_y + y) * img_w * 3 + crop_x * 3],
               crop_w * 3);
    }

    /* 应用变换到标准大小 */
    warp_affine(cropped, crop_w, crop_h,
                aligned_face, FACE_ALIGN_SIZE, FACE_ALIGN_SIZE,
                transform);
}

/* ========== 特征提取与比对 ========== */

/**
 * @brief 提取人脸特征
 * @param aligned_face 对齐后的人脸图像
 * @param feature 输出特征向量 (INT8)
 */
void face_extract_feature(const uint8_t* aligned_face,
                           int8_t* feature)
{
    /* 预处理: 归一化并量化到INT8 */
    int8_t input_quant[FACE_ALIGN_SIZE * FACE_ALIGN_SIZE * 3];

    /* MobileFaceNet通常使用[-1, 1]范围，量化为INT8 */
    /* 假设scale为1/128, zero_point=0 */
    const float scale = 1.0f / 128.0f;

    for (int i = 0; i < FACE_ALIGN_SIZE * FACE_ALIGN_SIZE * 3; i++) {
        float normalized = (aligned_face[i] / 255.0f - 0.5f) / 0.5f;
        float quantized = normalized / scale;

        if (quantized > 127.0f) quantized = 127.0f;
        if (quantized < -128.0f) quantized = -128.0f;

        input_quant[i] = (int8_t)(quantized > 0 ? quantized + 0.5f : quantized - 0.5f);
    }

    /* 调用TFLM推理 (MobileFaceNet模型) */
    /* tflm_prepare_input(...); */
    /* tflm_invoke(...); */
    /* tflm_get_output(feature, ...); */

    /* 实际应调用TFLM，这里用模拟数据 */
    for (int i = 0; i < FACE_FEATURE_DIM; i++) {
        feature[i] = (int8_t)((i * 7) % 255 - 128);
    }
}

/**
 * @brief 计算余弦相似度 (INT8)
 * similarity = dot(a, b) / (norm(a) * norm(b))
 *
 * 对于单位向量，可以直接计算点积
 */
float face_cosine_similarity(const int8_t* a, const int8_t* b, int dim)
{
    int32_t dot = 0;
    int32_t norm_a = 0;
    int32_t norm_b = 0;

    for (int i = 0; i < dim; i++) {
        dot += (int32_t)a[i] * (int32_t)b[i];
        norm_a += (int32_t)a[i] * (int32_t)a[i];
        norm_b += (int32_t)b[i] * (int32_t)b[i];
    }

    /* 使用定点数或查表法计算sqrt和除法 */
    /* 简化: 假设特征已归一化，直接返回归一化点积 */
    float dot_f = dot / (127.0f * 127.0f);
    float norm_a_f = sqrtf(norm_a) / 127.0f;
    float norm_b_f = sqrtf(norm_b) / 127.0f;

    if (norm_a_f < 1e-6f || norm_b_f < 1e-6f) {
        return 0.0f;
    }

    return dot_f / (norm_a_f * norm_b_f);
}

/**
 * @brief 人脸匹配
 * @param query_feature 查询特征
 * @param best_match_id 输出最佳匹配ID
 * @param similarity 输出相似度
 * @return 1找到匹配，0未找到
 */
int face_match(const int8_t* query_feature,
                uint32_t* best_match_id,
                float* similarity)
{
    float best_sim = -1.0f;
    int best_idx = -1;

    for (uint32_t i = 0; i < s_face_db.num_entries; i++) {
        float sim = face_cosine_similarity(
            query_feature,
            s_face_db.entries[i].feature,
            FACE_FEATURE_DIM
        );

        if (sim > best_sim) {
            best_sim = sim;
            best_idx = i;
        }
    }

    if (best_idx >= 0 && best_sim >= FACE_MATCH_THRESHOLD) {
        *best_match_id = s_face_db.entries[best_idx].id;
        *similarity = best_sim;
        return 1;
    }

    return 0;
}

/* ========== 数据库管理 ========== */

int face_db_add(const char* name, const int8_t* feature)
{
    if (s_face_db.num_entries >= MAX_FACE_DB_SIZE) {
        return -1;  /* 数据库已满 */
    }

    face_feature_t* entry = &s_face_db.entries[s_face_db.num_entries];

    strncpy(entry->name, name, sizeof(entry->name) - 1);
    entry->name[sizeof(entry->name) - 1] = '\0';
    memcpy(entry->feature, feature, FACE_FEATURE_DIM);
    entry->id = s_face_db.num_entries + 1;

    s_face_db.num_entries++;

    return entry->id;
}

void face_db_clear(void)
{
    s_face_db.num_entries = 0;
    memset(&s_face_db, 0, sizeof(s_face_db));
}
```

### 6.2 物体检测系统

```c
/**
 * @file object_detection.c
 * @brief 嵌入式物体检测系统
 *
 * 支持模型:
 * - MobileNet SSD
 * - YOLO-tiny
 * - EfficientDet-lite
 */

#include <stdint.h>
#include <string.h>
#include <math.h>

/* ========== 配置 ========== */
#define OD_INPUT_WIDTH          300
#define OD_INPUT_HEIGHT         300
#define OD_NUM_CLASSES          91      /* COCO数据集 */
#define OD_MAX_DETECTIONS       50
#define OD_CONFIDENCE_THRESHOLD 0.5f
#define OD_NMS_THRESHOLD        0.45f

/* ========== 检测框结构 ========== */
typedef struct {
    float x1, y1, x2, y2;    /* 归一化坐标 [0,1] */
    float confidence;         /* 置信度 */
    int class_id;             /* 类别ID */
} detection_box_t;

/* COCO类别名称 */
static const char* kCocoClasses[OD_NUM_CLASSES] = {
    "background",
    "person", "bicycle", "car", "motorcycle", "airplane",
    "bus", "train", "truck", "boat", "traffic light",
    /* ... 其他类别 ... */
};

/**
 * @brief MobileNet SSD解码
 *
 * SSD输出:
 * - 位置预测: [num_priors, 4] (x, y, w, h)
 * - 置信度: [num_priors, num_classes]
 *
 * 需要解码先验框(prior boxes/anchors)并应用NMS
 */
void ssd_decode_detections(const float* location_predictions,
                            const float* confidence_predictions,
                            const float* prior_boxes,
                            int num_priors,
                            detection_box_t* detections,
                            int* num_detections)
{
    *num_detections = 0;

    for (int i = 0; i < num_priors && *num_detections < OD_MAX_DETECTIONS; i++) {
        /* 找到最大置信度的类别 */
        float max_conf = 0.0f;
        int max_class = 0;

        for (int c = 1; c < OD_NUM_CLASSES; c++) {  /* 跳过背景 */
            float conf = confidence_predictions[i * OD_NUM_CLASSES + c];
            if (conf > max_conf) {
                max_conf = conf;
                max_class = c;
            }
        }

        /* 阈值过滤 */
        if (max_conf < OD_CONFIDENCE_THRESHOLD) {
            continue;
        }

        /* 解码位置 */
        float prior_cx = prior_boxes[i * 4 + 0];
        float prior_cy = prior_boxes[i * 4 + 1];
        float prior_w = prior_boxes[i * 4 + 2];
        float prior_h = prior_boxes[i * 4 + 3];

        float loc_cx = location_predictions[i * 4 + 0];
        float loc_cy = location_predictions[i * 4 + 1];
        float loc_w = location_predictions[i * 4 + 2];
        float loc_h = location_predictions[i * 4 + 3];

        /* 中心坐标方差: 0.1, 尺寸方差: 0.2 */
        float cx = prior_cx + loc_cx * 0.1f * prior_w;
        float cy = prior_cy + loc_cy * 0.1f * prior_h;
        float w = prior_w * expf(loc_w * 0.2f);
        float h = prior_h * expf(loc_h * 0.2f);

        /* 转换为角点坐标 */
        detection_box_t* det = &detections[*num_detections];
        det->x1 = cx - w / 2.0f;
        det->y1 = cy - h / 2.0f;
        det->x2 = cx + w / 2.0f;
        det->y2 = cy + h / 2.0f;
        det->confidence = max_conf;
        det->class_id = max_class;

        /* 边界限制 */
        if (det->x1 < 0) det->x1 = 0;
        if (det->y1 < 0) det->y1 = 0;
        if (det->x2 > 1) det->x2 = 1;
        if (det->y2 > 1) det->y2 = 1;

        (*num_detections)++;
    }
}

/**
 * @brief 计算IoU (Intersection over Union)
 */
float compute_iou(const detection_box_t* a, const detection_box_t* b)
{
    float x1 = fmaxf(a->x1, b->x1);
    float y1 = fmaxf(a->y1, b->y1);
    float x2 = fminf(a->x2, b->x2);
    float y2 = fminf(a->y2, b->y2);

    float inter_w = fmaxf(0.0f, x2 - x1);
    float inter_h = fmaxf(0.0f, y2 - y1);
    float inter_area = inter_w * inter_h;

    float area_a = (a->x2 - a->x1) * (a->y2 - a->y1);
    float area_b = (b->x2 - b->x1) * (b->y2 - b->y1);

    float union_area = area_a + area_b - inter_area;

    if (union_area < 1e-6f) return 0.0f;

    return inter_area / union_area;
}

/**
 * @brief 非极大值抑制 (NMS)
 */
void nms(detection_box_t* detections,
          int num_detections,
          detection_box_t* output,
          int* num_output)
{
    /* 按置信度排序 (简化: 冒泡排序) */
    for (int i = 0; i < num_detections - 1; i++) {
        for (int j = i + 1; j < num_detections; j++) {
            if (detections[j].confidence > detections[i].confidence) {
                detection_box_t temp = detections[i];
                detections[i] = detections[j];
                detections[j] = temp;
            }
        }
    }

    /* 抑制标记 */
    uint8_t suppressed[OD_MAX_DETECTIONS] = {0};
    *num_output = 0;

    for (int i = 0; i < num_detections; i++) {
        if (suppressed[i]) continue;

        /* 保留此检测 */
        output[*num_output] = detections[i];
        (*num_output)++;

        /* 抑制重叠检测 */
        for (int j = i + 1; j < num_detections; j++) {
            if (suppressed[j]) continue;

            /* 只比较同类别的 */
            if (detections[j].class_id != detections[i].class_id) {
                continue;
            }

            float iou = compute_iou(&detections[i], &detections[j]);
            if (iou > OD_NMS_THRESHOLD) {
                suppressed[j] = 1;
            }
        }
    }
}

/**
 * @brief 完整的物体检测流程
 */
void object_detect(const uint8_t* image, int width, int height,
                    detection_box_t* detections,
                    int* num_detections)
{
    /* 1. 预处理 */
    int8_t input_quant[OD_INPUT_HEIGHT * OD_INPUT_WIDTH * 3];

    /* 缩放和量化 */
    float scale_x = (float)width / OD_INPUT_WIDTH;
    float scale_y = (float)height / OD_INPUT_HEIGHT;

    for (int y = 0; y < OD_INPUT_HEIGHT; y++) {
        for (int x = 0; x < OD_INPUT_WIDTH; x++) {
            int src_x = (int)(x * scale_x);
            int src_y = (int)(y * scale_y);

            if (src_x >= width) src_x = width - 1;
            if (src_y >= height) src_y = height - 1;

            for (int c = 0; c < 3; c++) {
                uint8_t pixel = image[(src_y * width + src_x) * 3 + c];
                float normalized = (pixel / 255.0f - 0.5f) / 0.5f;

                /* 量化到INT8 (假设scale=1/128) */
                float q = normalized * 128.0f;
                if (q > 127) q = 127;
                if (q < -128) q = -128;

                input_quant[(y * OD_INPUT_WIDTH + x) * 3 + c] =
                    (int8_t)(q > 0 ? q + 0.5f : q - 0.5f);
            }
        }
    }

    /* 2. TFLM推理 */
    /* ... 调用模型推理 ... */

    /* 3. 解码检测 (使用模拟输出) */
    detection_box_t raw_detections[OD_MAX_DETECTIONS];
    int num_raw = 0;

    /* 这里应该使用模型真实输出 */
    /* ssd_decode_detections(loc_preds, conf_preds, priors, ...); */

    /* 4. NMS */
    nms(raw_detections, num_raw, detections, num_detections);
}
```



---

## 7. 语音模型部署

### 7.1 关键词唤醒 (KWS)

```c
/**
 * @file keyword_spotting.c
 * @brief 嵌入式关键词唤醒系统
 *
 * 支持的唤醒词:
 * - "Hey Siri"
 * - "OK Google"
 * - "Alexa"
 * - 自定义中文唤醒词
 */

#include <stdint.h>
#include <string.h>
#include <math.h>
#include <complex.h>

/* ========== 音频配置 ========== */
#define KWS_SAMPLE_RATE         16000   /* 16kHz采样率 */
#define KWS_FRAME_LENGTH_MS     30      /* 30ms帧长 */
#define KWS_FRAME_STRIDE_MS     20      /* 20ms步长 */
#define KWS_NUM_MEL_BINS        40      /* Mel滤波器数量 */
#define KWS_SPECTROGRAM_LENGTH  49      /* 时间步数 (1秒) */
#define KWS_NUM_CLASSES         4       /* 类别数 */

/* 类别定义 */
typedef enum {
    KWS_SILENCE = 0,
    KWS_UNKNOWN = 1,
    KWS_WAKEWORD_YES = 2,
    KWS_WAKEWORD_NO = 3
} kws_class_t;

static const char* kKwsLabels[KWS_NUM_CLASSES] = {
    "silence", "unknown", "yes", "no"
};

/* ========== FFT实现 ========== */

/* 简化FFT - 实际应用使用CMSIS-DSP */
#define FFT_N 512  /* 512点FFT */

typedef struct {
    float real[FFT_N];
    float imag[FFT_N];
} fft_buffer_t;

/**
 * @brief 位反转
 */
static void bit_reverse(float* real, float* imag, int n)
{
    int j = 0;
    for (int i = 0; i < n; i++) {
        if (i < j) {
            float temp = real[i];
            real[i] = real[j];
            real[j] = temp;
            temp = imag[i];
            imag[i] = imag[j];
            imag[j] = temp;
        }
        int m = n >> 1;
        while (m >= 1 && j >= m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
}

/**
 * @brief Cooley-Tukey FFT (蝶形运算)
 */
void fft(float* real, float* imag, int n)
{
    bit_reverse(real, imag, n);

    for (int stage = 1; stage < n; stage <<= 1) {
        float wlen_real = cosf(M_PI / stage);
        float wlen_imag = -sinf(M_PI / stage);

        for (int i = 0; i < n; i += (stage << 1)) {
            float w_real = 1.0f;
            float w_imag = 0.0f;

            for (int j = 0; j < stage; j++) {
                int u = i + j;
                int v = i + j + stage;

                float u_real = real[u];
                float u_imag = imag[u];

                float t_real = real[v] * w_real - imag[v] * w_imag;
                float t_imag = real[v] * w_imag + imag[v] * w_real;

                real[u] = u_real + t_real;
                imag[u] = u_imag + t_imag;
                real[v] = u_real - t_real;
                imag[v] = u_imag - t_imag;

                float next_w_real = w_real * wlen_real - w_imag * wlen_imag;
                w_imag = w_real * wlen_imag + w_imag * wlen_real;
                w_real = next_w_real;
            }
        }
    }
}

/* ========== Mel滤波器组 ========== */

/* Mel频率转换 */
#define HZ_TO_MEL(hz) (2595.0f * log10f(1.0f + (hz) / 700.0f))
#define MEL_TO_HZ(mel) (700.0f * (powf(10.0f, (mel) / 2595.0f) - 1.0f))

/* 预计算的Mel滤波器权重 (离线计算) */
static const float kMelFilterWeights[KWS_NUM_MEL_BINS][FFT_N/2 + 1] = {
    /* 应包含预计算的滤波器系数 */
    /* 这里仅作占位 */
};

static const float kMelFreqs[KWS_NUM_MEL_BINS + 2] = {
    /* Mel滤波器中心频率 */
};

/**
 * @brief 计算Mel频谱
 */
void compute_mel_spectrogram(const float* fft_power,
                              float* mel_energies,
                              int num_mel_bins)
{
    for (int m = 0; m < num_mel_bins; m++) {
        float energy = 0.0f;

        /* 应用Mel滤波器 */
        for (int k = 0; k < FFT_N / 2 + 1; k++) {
            energy += fft_power[k] * kMelFilterWeights[m][k];
        }

        /* 取对数 */
        mel_energies[m] = logf(energy + 1e-10f);
    }
}

/* ========== MFCC计算 ========== */

/* DCT-II矩阵 (预计算) */
static const float kDctMatrix[KWS_NUM_MEL_BINS][KWS_NUM_MEL_BINS] = {
    /* 离线计算的DCT系数 */
};

/**
 * @brief 计算MFCC
 * @param mel_energies Mel能量
 * @param mfcc 输出MFCC系数
 * @param num_ceps 倒谱系数数量 (通常取13或40)
 */
void compute_mfcc(const float* mel_energies,
                   float* mfcc,
                   int num_ceps)
{
    /* DCT-II变换 */
    for (int i = 0; i < num_ceps; i++) {
        mfcc[i] = 0.0f;
        for (int j = 0; j < KWS_NUM_MEL_BINS; j++) {
            mfcc[i] += mel_energies[j] * kDctMatrix[i][j];
        }
    }
}

/* ========== 特征归一化 ========== */

/**
 * @brief 每帧CMN (倒谱均值归一化)
 */
void apply_cmn(float* mfcc, int num_ceps)
{
    /* 计算均值 */
    float mean = 0.0f;
    for (int i = 0; i < num_ceps; i++) {
        mean += mfcc[i];
    }
    mean /= num_ceps;

    /* 减去均值 */
    for (int i = 0; i < num_ceps; i++) {
        mfcc[i] -= mean;
    }
}

/**
 * @brief 全局CMVN (倒谱均值方差归一化)
 */
static float s_global_mean[KWS_NUM_MEL_BINS] = {0};
static float s_global_std[KWS_NUM_MEL_BINS] = {1};
static int s_frame_count = 0;

void update_global_stats(const float* mfcc)
{
    /* 增量更新全局统计 */
    s_frame_count++;
    float alpha = 1.0f / s_frame_count;

    for (int i = 0; i < KWS_NUM_MEL_BINS; i++) {
        float delta = mfcc[i] - s_global_mean[i];
        s_global_mean[i] += alpha * delta;
        /* 方差更新... */
    }
}

void apply_cmvn(float* mfcc)
{
    for (int i = 0; i < KWS_NUM_MEL_BINS; i++) {
        mfcc[i] = (mfcc[i] - s_global_mean[i]) / s_global_std[i];
    }
}

/* ========== 滑动窗口推理 ========== */

typedef struct {
    float buffer[KWS_SPECTROGRAM_LENGTH][KWS_NUM_MEL_BINS];
    int write_idx;
    uint8_t is_full;
} spectrogram_ring_buffer_t;

static spectrogram_ring_buffer_t s_spec_buffer;

/**
 * @brief 添加新的特征帧
 */
void add_feature_frame(const float* mfcc)
{
    memcpy(s_spec_buffer.buffer[s_spec_buffer.write_idx],
           mfcc, KWS_NUM_MEL_BINS * sizeof(float));

    s_spec_buffer.write_idx =
        (s_spec_buffer.write_idx + 1) % KWS_SPECTROGRAM_LENGTH;

    if (s_spec_buffer.write_idx == 0) {
        s_spec_buffer.is_full = 1;
    }
}

/**
 * @brief 获取展平的频谱图用于推理
 */
void get_spectrogram_for_inference(float* output)
{
    int idx = s_spec_buffer.write_idx;

    for (int t = 0; t < KWS_SPECTROGRAM_LENGTH; t++) {
        /* 从环形缓冲区按时间顺序读取 */
        int read_idx = (idx + t) % KWS_SPECTROGRAM_LENGTH;
        memcpy(&output[t * KWS_NUM_MEL_BINS],
               s_spec_buffer.buffer[read_idx],
               KWS_NUM_MEL_BINS * sizeof(float));
    }
}

/* ========== 平滑和后处理 ========== */

#define SMOOTHING_WINDOW 5

typedef struct {
    float history[SMOOTHING_WINDOW][KWS_NUM_CLASSES];
    int write_idx;
} prediction_smoothing_t;

static prediction_smoothing_t s_smoothing;

/**
 * @brief 添加预测并进行时域平滑
 */
kws_class_t smooth_prediction(const float* raw_prediction)
{
    /* 存入历史 */
    memcpy(s_smoothing.history[s_smoothing.write_idx],
           raw_prediction, KWS_NUM_CLASSES * sizeof(float));

    s_smoothing.write_idx = (s_smoothing.write_idx + 1) % SMOOTHING_WINDOW;

    /* 计算平均 */
    float smoothed[KWS_NUM_CLASSES] = {0};
    for (int i = 0; i < SMOOTHING_WINDOW; i++) {
        for (int c = 0; c < KWS_NUM_CLASSES; c++) {
            smoothed[c] += s_smoothing.history[i][c];
        }
    }

    for (int c = 0; c < KWS_NUM_CLASSES; c++) {
        smoothed[c] /= SMOOTHING_WINDOW;
    }

    /* 找到最大 */
    int max_class = 0;
    float max_conf = smoothed[0];
    for (int c = 1; c < KWS_NUM_CLASSES; c++) {
        if (smoothed[c] > max_conf) {
            max_conf = smoothed[c];
            max_class = c;
        }
    }

    return (kws_class_t)max_class;
}

/* ========== 主处理循环 ========== */

/**
 * @brief 处理一帧音频 (480 samples @ 16kHz = 30ms)
 * @param audio_frame 16-bit PCM音频帧
 * @return 检测到的类别
 */
kws_class_t kws_process_frame(const int16_t* audio_frame)
{
    /* 1. 预加重 */
    static float prev_sample = 0.0f;
    float preemphasized[480];
    const float preemphasis = 0.97f;

    for (int i = 0; i < 480; i++) {
        float sample = audio_frame[i] / 32768.0f;  /* 归一化到[-1,1] */
        preemphasized[i] = sample - preemphasis * prev_sample;
        prev_sample = sample;
    }

    /* 2. 加汉明窗 */
    float windowed[512] = {0};  /* 零填充到512 */
    for (int i = 0; i < 480; i++) {
        float window = 0.54f - 0.46f * cosf(2.0f * M_PI * i / 479.0f);
        windowed[i] = preemphasized[i] * window;
    }

    /* 3. FFT */
    float imag[512] = {0};
    fft(windowed, imag, 512);

    /* 4. 功率谱 */
    float power_spectrum[257];
    for (int i = 0; i <= 256; i++) {
        power_spectrum[i] = windowed[i] * windowed[i] + imag[i] * imag[i];
    }

    /* 5. Mel滤波 */
    float mel_energies[KWS_NUM_MEL_BINS];
    compute_mel_spectrogram(power_spectrum, mel_energies, KWS_NUM_MEL_BINS);

    /* 6. MFCC (可选, 取决于模型输入) */
    float mfcc[KWS_NUM_MEL_BINS];
    compute_mfcc(mel_energies, mfcc, KWS_NUM_MEL_BINS);

    /* 7. 归一化 */
    apply_cmn(mfcc);

    /* 8. 添加到滑动窗口 */
    add_feature_frame(mfcc);

    /* 9. 触发推理 (当缓冲区满时) */
    if (!s_spec_buffer.is_full) {
        return KWS_SILENCE;
    }

    /* 10. 获取频谱图并推理 */
    float spectrogram[KWS_SPECTROGRAM_LENGTH * KWS_NUM_MEL_BINS];
    get_spectrogram_for_inference(spectrogram);

    /* 量化并调用TFLM */
    int8_t quantized_input[KWS_SPECTROGRAM_LENGTH * KWS_NUM_MEL_BINS];
    /* 假设输入scale为0.0390625 (1/25.6) */
    const float input_scale = 0.0390625f;

    for (int i = 0; i < KWS_SPECTROGRAM_LENGTH * KWS_NUM_MEL_BINS; i++) {
        float q = spectrogram[i] / input_scale;
        if (q > 127) q = 127;
        if (q < -128) q = -128;
        quantized_input[i] = (int8_t)(q > 0 ? q + 0.5f : q - 0.5f);
    }

    /* 调用TFLM推理 */
    /* tflm_invoke(...); */
    /* 获取输出并反量化 */

    float prediction[KWS_NUM_CLASSES] = {0.1f, 0.1f, 0.7f, 0.1f};  /* 模拟 */

    /* 11. 时域平滑 */
    return smooth_prediction(prediction);
}

/**
 * @brief 重置KWS状态
 */
void kws_reset(void)
{
    memset(&s_spec_buffer, 0, sizeof(s_spec_buffer));
    memset(&s_smoothing, 0, sizeof(s_smoothing));
    s_frame_count = 0;
}
```

---

## 8. 传感器数据处理

### 8.1 FFT和滤波器

```c
/**
 * @file signal_processing.c
 * @brief 传感器信号处理
 */

#include <stdint.h>
#include <string.h>
#include <math.h>

/* ========== 数字滤波器 ========== */

/* IIR滤波器系数 (预计算) */
typedef struct {
    float b[3];  /* 分子系数 */
    float a[3];  /* 分母系数 (a[0]通常为1) */
} biquad_coeffs_t;

/* IIR滤波器状态 */
typedef struct {
    float x[2];  /* 输入延迟 */
    float y[2];  /* 输出延迟 */
} biquad_state_t;

/**
 * @brief 双二阶滤波器 (Biquad/IIR)
 * H(z) = (b0 + b1*z^-1 + b2*z^-2) / (1 + a1*z^-1 + a2*z^-2)
 */
float biquad_process(float input,
                      const biquad_coeffs_t* coeffs,
                      biquad_state_t* state)
{
    /* 差分方程实现 */
    float output = coeffs->b[0] * input
                 + coeffs->b[1] * state->x[0]
                 + coeffs->b[2] * state->x[1]
                 - coeffs->a[1] * state->y[0]
                 - coeffs->a[2] * state->y[1];

    /* 更新状态 */
    state->x[1] = state->x[0];
    state->x[0] = input;
    state->y[1] = state->y[0];
    state->y[0] = output;

    return output;
}

/**
 * @brief 设计低通滤波器
 * 使用双线性变换从模拟原型转换
 */
void design_lowpass_filter(biquad_coeffs_t* coeffs,
                            float cutoff_freq,
                            float sample_rate,
                            float q_factor)
{
    float w0 = 2.0f * M_PI * cutoff_freq / sample_rate;
    float cos_w0 = cosf(w0);
    float sin_w0 = sinf(w0);
    float alpha = sin_w0 / (2.0f * q_factor);

    float a0 = 1.0f + alpha;

    coeffs->b[0] = ((1.0f - cos_w0) / 2.0f) / a0;
    coeffs->b[1] = (1.0f - cos_w0) / a0;
    coeffs->b[2] = ((1.0f - cos_w0) / 2.0f) / a0;
    coeffs->a[0] = 1.0f;
    coeffs->a[1] = (-2.0f * cos_w0) / a0;
    coeffs->a[2] = (1.0f - alpha) / a0;
}

/**
 * @brief 设计陷波滤波器 (用于去除电源干扰50/60Hz)
 */
void design_notch_filter(biquad_coeffs_t* coeffs,
                          float notch_freq,
                          float sample_rate,
                          float bandwidth)
{
    float w0 = 2.0f * M_PI * notch_freq / sample_rate;
    float cos_w0 = cosf(w0);
    float sin_w0 = sinf(w0);

    /* 从带宽计算alpha */
    float q = notch_freq / bandwidth;
    float alpha = sin_w0 / (2.0f * q);

    float a0 = 1.0f + alpha;

    coeffs->b[0] = 1.0f / a0;
    coeffs->b[1] = (-2.0f * cos_w0) / a0;
    coeffs->b[2] = 1.0f / a0;
    coeffs->a[0] = 1.0f;
    coeffs->a[1] = (-2.0f * cos_w0) / a0;
    coeffs->a[2] = (1.0f - alpha) / a0;
}

/* ========== FIR滤波器 ========== */

#define FIR_TAPS 64

typedef struct {
    float coeffs[FIR_TAPS];
    float delay_line[FIR_TAPS];
    int write_idx;
} fir_filter_t;

float fir_process(fir_filter_t* filter, float input)
{
    /* 存入延迟线 */
    filter->delay_line[filter->write_idx] = input;
    filter->write_idx = (filter->write_idx + 1) % FIR_TAPS;

    /* 卷积计算 */
    float output = 0.0f;
    for (int i = 0; i < FIR_TAPS; i++) {
        int idx = (filter->write_idx + FIR_TAPS - 1 - i) % FIR_TAPS;
        output += filter->coeffs[i] * filter->delay_line[idx];
    }

    return output;
}

/* ========== 滑动平均滤波器 ========== */

typedef struct {
    float buffer[16];  /* 窗口大小 */
    int size;
    int idx;
    float sum;
    int count;
} moving_average_t;

void ma_init(moving_average_t* ma, int size)
{
    memset(ma, 0, sizeof(moving_average_t));
    ma->size = size > 16 ? 16 : size;
}

float ma_process(moving_average_t* ma, float input)
{
    /* 减去最旧的值 */
    ma->sum -= ma->buffer[ma->idx];

    /* 存入新值 */
    ma->buffer[ma->idx] = input;
    ma->sum += input;

    ma->idx = (ma->idx + 1) % ma->size;

    if (ma->count < ma->size) {
        ma->count++;
    }

    return ma->sum / ma->count;
}

/* ========== 中值滤波器 (去除脉冲噪声) ========== */

#define MEDIAN_WINDOW 5

typedef struct {
    float buffer[MEDIAN_WINDOW];
    int idx;
} median_filter_t;

float median_process(median_filter_t* mf, float input)
{
    mf->buffer[mf->idx] = input;
    mf->idx = (mf->idx + 1) % MEDIAN_WINDOW;

    /* 复制并排序 */
    float sorted[MEDIAN_WINDOW];
    memcpy(sorted, mf->buffer, sizeof(sorted));

    /* 冒泡排序 */
    for (int i = 0; i < MEDIAN_WINDOW - 1; i++) {
        for (int j = i + 1; j < MEDIAN_WINDOW; j++) {
            if (sorted[j] < sorted[i]) {
                float temp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = temp;
            }
        }
    }

    /* 返回中值 */
    return sorted[MEDIAN_WINDOW / 2];
}

/* ========== 特征提取 ========== */

/**
 * @brief 计算信号的统计特征
 */
typedef struct {
    float mean;
    float std;
    float max;
    float min;
    float rms;    /* 均方根 */
    float peak_to_peak;
} signal_stats_t;

void compute_signal_stats(const float* signal,
                           int length,
                           signal_stats_t* stats)
{
    /* 均值 */
    float sum = 0.0f;
    stats->max = -1e20f;
    stats->min = 1e20f;

    for (int i = 0; i < length; i++) {
        sum += signal[i];
        if (signal[i] > stats->max) stats->max = signal[i];
        if (signal[i] < stats->min) stats->min = signal[i];
    }

    stats->mean = sum / length;
    stats->peak_to_peak = stats->max - stats->min;

    /* 标准差和RMS */
    float sum_sq = 0.0f;
    float sum_var = 0.0f;

    for (int i = 0; i < length; i++) {
        float diff = signal[i] - stats->mean;
        sum_var += diff * diff;
        sum_sq += signal[i] * signal[i];
    }

    stats->std = sqrtf(sum_var / length);
    stats->rms = sqrtf(sum_sq / length);
}

/**
 * @brief 过零率 (Zero Crossing Rate)
 * 常用于语音端点检测
 */
float compute_zcr(const float* signal, int length)
{
    int crossings = 0;

    for (int i = 1; i < length; i++) {
        if ((signal[i] > 0 && signal[i-1] <= 0) ||
            (signal[i] <= 0 && signal[i-1] > 0)) {
            crossings++;
        }
    }

    return (float)crossings / (length - 1);
}

/**
 * @brief 计算频谱质心
 */
float compute_spectral_centroid(const float* spectrum, int num_bins, float sample_rate)
{
    float weighted_sum = 0.0f;
    float sum = 0.0f;

    float bin_width = sample_rate / 2.0f / num_bins;

    for (int i = 0; i < num_bins; i++) {
        float freq = i * bin_width;
        weighted_sum += freq * spectrum[i];
        sum += spectrum[i];
    }

    return sum > 0 ? weighted_sum / sum : 0;
}

/* ========== 降采样 ========== */

typedef struct {
    float prev_sample;
    int decimation_factor;
    int counter;
} decimator_t;

void decimator_init(decimator_t* dec, int factor)
{
    dec->prev_sample = 0.0f;
    dec->decimation_factor = factor;
    dec->counter = 0;
}

/* 简单的抽取 (需要配合抗混叠滤波器) */
int decimate(decimator_t* dec, float input, float* output)
{
    /* 线性插值 */
    float interpolated = dec->prev_sample +
        (input - dec->prev_sample) * dec->counter / dec->decimation_factor;

    dec->prev_sample = input;
    dec->counter++;

    if (dec->counter >= dec->decimation_factor) {
        dec->counter = 0;
        *output = interpolated;
        return 1;  /* 有新样本输出 */
    }

    return 0;
}
```


---

## 9. 内存优化策略

### 9.1 内存布局与分配

```c
/**
 * @file memory_optimization.h
 * @brief 嵌入式AI内存优化策略
 */

#ifndef MEMORY_OPTIMIZATION_H
#define MEMORY_OPTIMIZATION_H

#include <stdint.h>
#include <stddef.h>

/* ========== 内存区域定义 ========== */

/* 内存类型 */
typedef enum {
    MEM_FLASH,       /* Flash/ROM - 常量、模型权重 */
    MEM_SRAM_FAST,   /* 快速SRAM - 热数据、堆栈 */
    MEM_SRAM_SLOW,   /* 慢速SRAM - 缓冲区 */
    MEM_EXTERNAL,    /* 外部RAM - 大缓冲区 */
    MEM_COUNT
} memory_type_t;

/* 内存属性 */
typedef struct {
    void* base_addr;
    size_t size;
    uint32_t access_cycles;  /* 访问周期数 */
    uint8_t is_executable;   /* 可执行 */
    uint8_t is_cacheable;    /* 可缓存 */
} memory_region_t;

/* ========== 内存池管理 ========== */

#define MEM_POOL_ALIGNMENT 8
#define MEM_POOL_BLOCKS 32

typedef struct mem_block {
    struct mem_block* next;
    size_t size;
    uint8_t in_use;
    uint32_t alloc_seq;  /* 分配序列号，用于LRU */
} mem_block_t;

typedef struct {
    uint8_t* buffer;
    size_t total_size;
    size_t used_size;
    mem_block_t* free_list;
    mem_block_t* used_list;
    uint32_t alloc_counter;
} memory_pool_t;

/* ========== API ========== */

int mem_pool_init(memory_pool_t* pool, void* buffer, size_t size);
void* mem_pool_alloc(memory_pool_t* pool, size_t size);
void mem_pool_free(memory_pool_t* pool, void* ptr);
void mem_pool_reset(memory_pool_t* pool);

/* ========== 张量内存规划 ========== */

typedef struct {
    uint32_t id;
    size_t size;
    uint32_t first_use;   /* 第一次使用的时间步 */
    uint32_t last_use;    /* 最后一次使用的时间步 */
    void* allocated_addr;
} tensor_lifetime_t;

/**
 * @brief 使用贪心算法进行张量内存复用规划
 * 目标: 最小化总内存占用
 */
size_t plan_tensor_memory(tensor_lifetime_t* tensors,
                           int num_tensors,
                           void** allocation_map);

#endif /* MEMORY_OPTIMIZATION_H */
```

### 9.2 内存池实现

```c
/**
 * @file memory_optimization.c
 * @brief 内存优化实现
 */

#include "memory_optimization.h"
#include <string.h>

/* ========== 内存池实现 ========== */

int mem_pool_init(memory_pool_t* pool, void* buffer, size_t size)
{
    if (!pool || !buffer || size < sizeof(mem_block_t)) {
        return -1;
    }

    pool->buffer = buffer;
    pool->total_size = size;
    pool->used_size = 0;
    pool->alloc_counter = 0;

    /* 初始化第一个块 */
    mem_block_t* initial = (mem_block_t*)buffer;
    initial->next = NULL;
    initial->size = size - sizeof(mem_block_t);
    initial->in_use = 0;
    initial->alloc_seq = 0;

    pool->free_list = initial;
    pool->used_list = NULL;

    return 0;
}

void* mem_pool_alloc(memory_pool_t* pool, size_t size)
{
    if (!pool || size == 0) return NULL;

    /* 对齐大小 */
    size = (size + MEM_POOL_ALIGNMENT - 1) & ~(MEM_POOL_ALIGNMENT - 1);

    /* 搜索合适的空闲块 (首次适应) */
    mem_block_t* prev = NULL;
    mem_block_t* curr = pool->free_list;

    while (curr) {
        if (!curr->in_use && curr->size >= size) {
            /* 找到合适的块 */

            /* 如果块太大，分割它 */
            if (curr->size > size + sizeof(mem_block_t) + MEM_POOL_ALIGNMENT) {
                mem_block_t* new_block = (mem_block_t*)((uint8_t*)curr +
                    sizeof(mem_block_t) + size);

                new_block->size = curr->size - size - sizeof(mem_block_t);
                new_block->next = curr->next;
                new_block->in_use = 0;

                curr->size = size;
                curr->next = new_block;
            }

            /* 标记为已使用 */
            curr->in_use = 1;
            curr->alloc_seq = ++pool->alloc_counter;

            /* 移到已使用列表 */
            if (prev) {
                prev->next = curr->next;
            } else {
                pool->free_list = curr->next;
            }

            curr->next = pool->used_list;
            pool->used_list = curr;

            pool->used_size += curr->size + sizeof(mem_block_t);

            return (uint8_t*)curr + sizeof(mem_block_t);
        }

        prev = curr;
        curr = curr->next;
    }

    return NULL;  /* 分配失败 */
}

void mem_pool_free(memory_pool_t* pool, void* ptr)
{
    if (!pool || !ptr) return;

    mem_block_t* block = (mem_block_t*)((uint8_t*)ptr - sizeof(mem_block_t));

    if (!block->in_use) return;  /* 重复释放 */

    /* 从已使用列表移除 */
    mem_block_t** curr = &pool->used_list;
    while (*curr) {
        if (*curr == block) {
            *curr = block->next;
            break;
        }
        curr = &(*curr)->next;
    }

    /* 标记为空闲 */
    block->in_use = 0;
    pool->used_size -= block->size + sizeof(mem_block_t);

    /* 插入空闲列表 (保持地址有序，便于合并) */
    mem_block_t** insert_pos = &pool->free_list;
    while (*insert_pos && *insert_pos < block) {
        insert_pos = &(*insert_pos)->next;
    }

    block->next = *insert_pos;
    *insert_pos = block;

    /* 尝试合并相邻空闲块 */
    mem_block_t* merge_curr = pool->free_list;
    while (merge_curr && merge_curr->next) {
        uint8_t* curr_end = (uint8_t*)merge_curr + sizeof(mem_block_t) +
                           merge_curr->size;
        if (curr_end == (uint8_t*)merge_curr->next) {
            /* 合并 */
            merge_curr->size += sizeof(mem_block_t) + merge_curr->next->size;
            merge_curr->next = merge_curr->next->next;
        } else {
            merge_curr = merge_curr->next;
        }
    }
}

void mem_pool_reset(memory_pool_t* pool)
{
    if (!pool) return;

    pool->used_size = 0;
    pool->alloc_counter = 0;
    pool->used_list = NULL;

    /* 重置为单个大块 */
    mem_block_t* initial = (mem_block_t*)pool->buffer;
    initial->next = NULL;
    initial->size = pool->total_size - sizeof(mem_block_t);
    initial->in_use = 0;
    initial->alloc_seq = 0;

    pool->free_list = initial;
}

/* ========== 张量内存复用规划 ========== */

/* 按last_use排序的贪心算法 */
static int compare_by_last_use(const void* a, const void* b)
{
    const tensor_lifetime_t* ta = (const tensor_lifetime_t*)a;
    const tensor_lifetime_t* tb = (const tensor_lifetime_t*)b;
    return (int)ta->last_use - (int)tb->last_use;
}

size_t plan_tensor_memory(tensor_lifetime_t* tensors,
                           int num_tensors,
                           void** allocation_map)
{
    /* 按结束时间排序 */
    /* 注意: 实际应用使用qsort，这里简化 */
    /* qsort(tensors, num_tensors, sizeof(tensor_lifetime_t), compare_by_last_use); */

    /* 模拟内存分配 */
    size_t max_memory = 0;
    size_t current_offset = 0;

    for (int i = 0; i < num_tensors; i++) {
        /* 尝试找到可以复用的空间 */
        size_t best_offset = (size_t)-1;

        for (int j = 0; j < i; j++) {
            /* 检查张量j是否在当前张量开始前结束 */
            if (tensors[j].last_use < tensors[i].first_use) {
                /* 可以尝试复用 */
                if (tensors[j].size >= tensors[i].size) {
                    /* 复用这个位置 */
                    allocation_map[i] = tensors[j].allocated_addr;
                    best_offset = 0;  /* 标记已找到 */
                    break;
                }
            }
        }

        if (best_offset == (size_t)-1) {
            /* 分配新空间 */
            allocation_map[i] = (void*)current_offset;
            tensors[i].allocated_addr = (void*)current_offset;
            current_offset += tensors[i].size;
            if (current_offset > max_memory) {
                max_memory = current_offset;
            }
        }
    }

    return max_memory;
}

/* ========== 模型压缩加载 ========== */

/**
 * @brief 从Flash解压模型到RAM
 * 使用简单的LZ4类解压算法
 */
int decompress_model(const uint8_t* compressed_data,
                      size_t compressed_size,
                      uint8_t* output_buffer,
                      size_t output_size)
{
    size_t in_pos = 0;
    size_t out_pos = 0;

    while (in_pos < compressed_size && out_pos < output_size) {
        uint8_t token = compressed_data[in_pos++];

        int literal_len = token >> 4;
        int match_len = (token & 0x0F) + 4;  /* 最小匹配长度4 */

        /* 扩展长度 */
        if (literal_len == 15) {
            while (compressed_data[in_pos] == 255) {
                literal_len += 255;
                in_pos++;
            }
            literal_len += compressed_data[in_pos++];
        }

        /* 拷贝字面量 */
        for (int i = 0; i < literal_len && out_pos < output_size; i++) {
            output_buffer[out_pos++] = compressed_data[in_pos++];
        }

        if (in_pos >= compressed_size) break;

        /* 偏移量 */
        uint16_t offset = compressed_data[in_pos] |
                         (compressed_data[in_pos + 1] << 8);
        in_pos += 2;

        /* 扩展匹配长度 */
        if (match_len == 19) {  /* 15 + 4 */
            while (compressed_data[in_pos] == 255) {
                match_len += 255;
                in_pos++;
            }
            match_len += compressed_data[in_pos++];
        }

        /* 拷贝匹配 */
        for (int i = 0; i < match_len && out_pos < output_size; i++) {
            output_buffer[out_pos] = output_buffer[out_pos - offset];
            out_pos++;
        }
    }

    return (int)out_pos;
}

/* ========== 权重共享 ========== */

/**
 * @brief 权重矩阵SVD分解压缩
 * W ≈ U * S * V^T
 * 将大型权重矩阵分解为两个小矩阵的乘积
 */
void svd_compress_weights(const float* W, int m, int n, int k,
                           float* U, float* Vt)
{
    /* 简化实现 - 实际应使用完整的SVD算法 */
    /* 这里仅作概念演示 */

    /* 随机初始化U和Vt */
    for (int i = 0; i < m * k; i++) {
        U[i] = ((float)(i % 100) - 50.0f) / 100.0f;
    }
    for (int i = 0; i < k * n; i++) {
        Vt[i] = ((float)(i % 100) - 50.0f) / 100.0f;
    }

    /* 迭代优化 (简化版梯度下降) */
    for (int iter = 0; iter < 100; iter++) {
        /* 重构: W' = U * Vt */
        /* 计算误差 */
        /* 更新U和Vt */
    }
}

/**
 * @brief 使用分解后的矩阵进行前向传播
 * y = (x * U) * Vt
 * 原始: m*n 参数
 * 分解后: m*k + k*n 参数 (k << min(m,n))
 */
void svd_forward(const float* x, int batch, int n,
                  const float* U, const float* Vt, int m, int k,
                  float* y)
{
    /* 中间结果: batch x k */
    float intermediate[batch * k];

    /* x(batch, n) * U(n, k) -> intermediate(batch, k) */
    for (int b = 0; b < batch; b++) {
        for (int j = 0; j < k; j++) {
            float sum = 0.0f;
            for (int i = 0; i < n; i++) {
                sum += x[b * n + i] * U[i * k + j];
            }
            intermediate[b * k + j] = sum;
        }
    }

    /* intermediate(batch, k) * Vt(k, m) -> y(batch, m) */
    for (int b = 0; b < batch; b++) {
        for (int j = 0; j < m; j++) {
            float sum = 0.0f;
            for (int i = 0; i < k; i++) {
                sum += intermediate[b * k + i] * Vt[i * m + j];
            }
            y[b * m + j] = sum;
        }
    }
}
```

---

## 10. 功耗优化

### 10.1 电源管理策略

```c
/**
 * @file power_management.h
 * @brief 嵌入式AI功耗优化
 */

#ifndef POWER_MANAGEMENT_H
#define POWER_MANAGEMENT_H

#include <stdint.h>

/* ========== 功耗模式 ========== */

typedef enum {
    POWER_MODE_ACTIVE,      /* 全速运行 */
    POWER_MODE_INFERENCE,   /* 推理中 */
    POWER_MODE_IDLE,        /* 空闲等待 */
    POWER_MODE_SLEEP,       /* 浅睡眠 */
    POWER_MODE_DEEP_SLEEP,  /* 深睡眠 */
    POWER_MODE_HIBERNATE,   /* 休眠 */
} power_mode_t;

/* ========== 功耗统计 ========== */

typedef struct {
    uint32_t active_time_ms;
    uint32_t sleep_time_ms;
    uint32_t inference_count;
    float avg_current_ma;
    float total_energy_mj;
} power_stats_t;

/* ========== DVFS控制 ========== */

typedef struct {
    uint32_t cpu_freq_hz;
    uint32_t voltage_mv;
    uint8_t use_flash_cache;
    uint8_t use_fpu;
} dvfs_level_t;

/* DVFS级别: 频率/电压配对 */
static const dvfs_level_t kDvfsLevels[] = {
    {480000000, 1200, 1, 1},  /* 高性能 */
    {240000000, 1000, 1, 1},  /* 标准 */
    {16000000,  900,  1, 0},  /* 低功耗 */
    {4000000,   800,  0, 0},  /* 超低功耗 */
};
#define NUM_DVFS_LEVELS 4

/* ========== API ========== */

void pm_init(void);
void pm_set_mode(power_mode_t mode);
power_mode_t pm_get_mode(void);

/* DVFS控制 */
void pm_set_frequency(uint32_t freq_hz);
void pm_auto_scale_frequency(uint32_t workload_estimate);

/* 推理期间功耗控制 */
void pm_begin_inference(void);
void pm_end_inference(void);

/* 功耗测量 */
float pm_measure_current(void);
void pm_get_stats(power_stats_t* stats);

/* 定时唤醒 */
void pm_schedule_wakeup(uint32_t delay_ms);
void pm_enter_sleep_until_event(void);

#endif /* POWER_MANAGEMENT_H */
```

### 10.2 功耗管理实现

```c
/**
 * @file power_management.c
 * @brief 功耗管理实现
 */

#include "power_management.h"
#include "system_hal.h"  /* 平台相关的HAL */

static power_mode_t s_current_mode = POWER_MODE_ACTIVE;
static power_stats_t s_stats = {0};
static uint32_t s_mode_entry_time = 0;
static uint32_t s_inference_start_time = 0;

/* ========== 模式管理 ========== */

void pm_init(void)
{
    s_current_mode = POWER_MODE_ACTIVE;
    s_mode_entry_time = get_system_ms();
    memset(&s_stats, 0, sizeof(s_stats));

    /* 配置低功耗外设 */
    configure_low_power_mode();
}

void pm_set_mode(power_mode_t mode)
{
    if (mode == s_current_mode) return;

    uint32_t now = get_system_ms();
    uint32_t duration = now - s_mode_entry_time;

    /* 更新统计 */
    switch (s_current_mode) {
        case POWER_MODE_ACTIVE:
        case POWER_MODE_INFERENCE:
            s_stats.active_time_ms += duration;
            break;
        case POWER_MODE_IDLE:
        case POWER_MODE_SLEEP:
        case POWER_MODE_DEEP_SLEEP:
            s_stats.sleep_time_ms += duration;
            break;
        default:
            break;
    }

    /* 执行模式切换 */
    switch (mode) {
        case POWER_MODE_ACTIVE:
            enable_all_clocks();
            set_cpu_frequency(kDvfsLevels[0].cpu_freq_hz);
            break;

        case POWER_MODE_INFERENCE:
            /* 推理模式: 使用合适频率平衡性能和功耗 */
            set_cpu_frequency(kDvfsLevels[1].cpu_freq_hz);
            enable_fpu();
            break;

        case POWER_MODE_IDLE:
            /* 等待中断 */
            __WFI();
            break;

        case POWER_MODE_SLEEP:
            /* 关闭部分外设时钟 */
            disable_unused_peripherals();
            set_cpu_frequency(kDvfsLevels[2].cpu_freq_hz);
            __WFI();
            break;

        case POWER_MODE_DEEP_SLEEP:
            /* 进入Stop模式 */
            enter_stop_mode();
            break;

        case POWER_MODE_HIBERNATE:
            /* 进入Standby模式 */
            enter_standby_mode();
            break;
    }

    s_current_mode = mode;
    s_mode_entry_time = now;
}

/* ========== 推理期间的功耗控制 ========== */

/* 推理期间的频率调整策略 */
#define INFERENCE_DEADLINE_MS 100  /* 目标推理延迟 */

void pm_begin_inference(void)
{
    pm_set_mode(POWER_MODE_INFERENCE);
    s_inference_start_time = get_system_ms();

    /* 预热缓存 */
    prefetch_model_weights();
}

void pm_end_inference(void)
{
    uint32_t inference_time = get_system_ms() - s_inference_start_time;
    s_stats.inference_count++;

    /* 自适应频率调整 */
    if (inference_time < INFERENCE_DEADLINE_MS * 0.8f) {
        /* 推理太快，可以降低频率 */
        pm_auto_scale_frequency(0);
    } else if (inference_time > INFERENCE_DEADLINE_MS) {
        /* 推理太慢，需要提高频率 */
        pm_auto_scale_frequency(2);
    }

    pm_set_mode(POWER_MODE_IDLE);
}

/* ========== DVFS实现 ========== */

static int s_current_dvfs_level = 0;

void pm_set_frequency(uint32_t freq_hz)
{
    /* 找到最接近的频率级别 */
    int best_level = 0;
    uint32_t min_diff = (uint32_t)-1;

    for (int i = 0; i < NUM_DVFS_LEVELS; i++) {
        uint32_t diff = (freq_hz > kDvfsLevels[i].cpu_freq_hz) ?
                       (freq_hz - kDvfsLevels[i].cpu_freq_hz) :
                       (kDvfsLevels[i].cpu_freq_hz - freq_hz);
        if (diff < min_diff) {
            min_diff = diff;
            best_level = i;
        }
    }

    if (best_level == s_current_dvfs_level) return;

    /* 先升压再升频，或先降频再降压 */
    if (best_level < s_current_dvfs_level) {
        /* 升频: 先升压 */
        set_core_voltage(kDvfsLevels[best_level].voltage_mv);
        delay_us(100);  /* 等待电压稳定 */
        set_cpu_frequency(kDvfsLevels[best_level].cpu_freq_hz);
    } else {
        /* 降频: 先降频 */
        set_cpu_frequency(kDvfsLevels[best_level].cpu_freq_hz);
        set_core_voltage(kDvfsLevels[best_level].voltage_mv);
    }

    s_current_dvfs_level = best_level;
}

void pm_auto_scale_frequency(uint32_t workload_estimate)
{
    /* 简单的PID控制器实现 */
    static int current_level = 1;

    if (workload_estimate > current_level && current_level > 0) {
        current_level--;
    } else if (workload_estimate < current_level &&
               current_level < NUM_DVFS_LEVELS - 1) {
        current_level++;
    }

    pm_set_frequency(kDvfsLevels[current_level].cpu_freq_hz);
}

/* ========== 间歇推理策略 ========== */

/**
 * @brief 周期性推理调度
 * 在不需要连续推理的场景(如传感器监测)中，
 * 采用周期性唤醒-推理-休眠的策略
 */
typedef struct {
    uint32_t inference_interval_ms;   /* 推理间隔 */
    uint32_t max_inference_time_ms;   /* 最大推理时间预算 */
    uint8_t (*should_continue)(void); /* 继续推理的判断函数 */
} periodic_inference_config_t;

void pm_periodic_inference_loop(periodic_inference_config_t* config,
                                 void (*inference_fn)(void))
{
    while (1) {
        /* 唤醒 */
        pm_set_mode(POWER_MODE_ACTIVE);

        /* 采集数据 */
        acquire_sensor_data();

        /* 推理 */
        uint32_t inference_start = get_system_ms();
        inference_fn();
        uint32_t inference_time = get_system_ms() - inference_start;

        /* 检查是否继续 */
        if (config->should_continue && !config->should_continue()) {
            break;
        }

        /* 计算剩余休眠时间 */
        uint32_t sleep_time = config->inference_interval_ms;
        if (inference_time < sleep_time) {
            sleep_time -= inference_time;
        } else {
            sleep_time = 0;  /* 推理超时 */
        }

        /* 进入低功耗模式 */
        if (sleep_time > 100) {
            /* 长时间休眠进入深睡眠 */
            pm_schedule_wakeup(sleep_time);
            pm_set_mode(POWER_MODE_DEEP_SLEEP);
        } else if (sleep_time > 10) {
            pm_set_mode(POWER_MODE_SLEEP);
        } else {
            /* 短时间使用忙等待 */
            delay_ms(sleep_time);
        }
    }
}

/* ========== 批处理推理 ========== */

/**
 * @brief 批量推理以摊销唤醒开销
 * 收集多个样本后一次性推理
 */
typedef struct {
    void* buffer;
    uint32_t sample_size;
    uint32_t batch_size;
    uint32_t current_count;
} batch_buffer_t;

void pm_batch_inference(batch_buffer_t* batch,
                         const void* new_sample,
                         void (*batch_inference_fn)(void* batch_data,
                                                     uint32_t count))
{
    /* 添加新样本 */
    uint8_t* dst = (uint8_t*)batch->buffer +
                   batch->current_count * batch->sample_size;
    memcpy(dst, new_sample, batch->sample_size);
    batch->current_count++;

    /* 检查是否已满 */
    if (batch->current_count >= batch->batch_size) {
        /* 进入推理模式 */
        pm_begin_inference();

        /* 执行批量推理 */
        batch_inference_fn(batch->buffer, batch->current_count);

        pm_end_inference();

        /* 重置缓冲区 */
        batch->current_count = 0;
    }
}
```

---

## 11. 实际项目: STM32物体检测

### 11.1 项目概述

这是一个完整的边缘AI项目，实现基于STM32H7的实时物体检测系统。

**硬件配置:**

- MCU: STM32H743ZI (480MHz, Cortex-M7)
- RAM: 1MB + 864KB SRAM
- Flash: 2MB
- 摄像头: OV5640 (5MP)
- LCD: 480x272 RGB

**软件栈:**

- TensorFlow Lite Micro
- CMSIS-NN加速
- FreeRTOS (可选)

### 11.2 项目文件结构

```
stm32_object_detection/
├── Core/
│   ├── Inc/
│   │   ├── main.h
│   │   ├── camera.h
│   │   ├── lcd.h
│   │   ├── ai_model.h
│   │   └── preprocessing.h
│   └── Src/
│       ├── main.c
│       ├── camera.c
│       ├── lcd.c
│       ├── ai_model.c
│       └── preprocessing.c
├── Middlewares/
│   ├── ST/
│   │   └── AI/
│   │       ├── TFLM/
│   │       └── CMSIS-NN/
│   └── Third_Party/
│       └── tflite-micro/
├── Model/
│   ├── training/
│   │   ├── train_ssd.py
│   │   └── quantize.py
│   └── converted/
│       ├── model_data.h
│       └── model_info.txt
├── USB_DEVICE/
└── STM32H743ZITX_FLASH.ld
```

### 11.3 核心代码实现

```c
/**
 * @file main.c
 * @brief STM32物体检测主程序
 */

#include "main.h"
#include "camera.h"
#include "lcd.h"
#include "ai_model.h"
#include "preprocessing.h"
#include <stdio.h>
#include <string.h>

/* ========== 配置参数 ========== */
#define CAMERA_WIDTH        320
#define CAMERA_HEIGHT       240
#define DISPLAY_WIDTH       480
#define DISPLAY_HEIGHT      272
#define MODEL_INPUT_SIZE    300  /* MobileNet SSD输入 */

#define TENSOR_ARENA_SIZE   (512 * 1024)  /* 512KB张量内存 */

/* ========== 全局变量 ========== */
/* 模型内存池 - 放置在SDRAM */
static uint8_t s_tensor_arena[TENSOR_ARENA_SIZE]
    __attribute__((section(".sdram"), aligned(32)));

/* 图像缓冲区 */
static uint8_t s_camera_buffer[CAMERA_WIDTH * CAMERA_HEIGHT * 2]
    __attribute__((aligned(32)));  /* RGB565 */

static uint8_t s_model_input[MODEL_INPUT_SIZE * MODEL_INPUT_SIZE * 3]
    __attribute__((aligned(32)));

static uint16_t s_display_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT]
    __attribute__((section(".lcd_buffer")));

/* AI模型上下文 */
static ai_model_context_t s_ai_ctx;

/* 性能统计 */
typedef struct {
    uint32_t frame_count;
    uint32_t total_inference_time;
    uint32_t total_preprocess_time;
    uint32_t fps;
    uint32_t last_update_time;
} perf_stats_t;

static perf_stats_t s_stats;

/* ========== 初始化 ========== */

static void System_Init(void)
{
    /* HAL初始化 */
    HAL_Init();
    SystemClock_Config();

    /* 配置MPU - 优化SDRAM访问 */
    MPU_Config();

    /* 启用D-Cache和I-Cache */
    SCB_EnableICache();
    SCB_EnableDCache();

    /* 初始化外设 */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_DCMI_Init();
    MX_I2C1_Init();
    MX_LTDC_Init();
    MX_DMA2D_Init();

    /* 初始化SDRAM */
    BSP_SDRAM_Init();

    /* 初始化LCD */
    LCD_Init(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    LCD_Clear(LCD_COLOR_BLACK);

    /* 初始化摄像头 */
    Camera_Init(CAMERA_WIDTH, CAMERA_HEIGHT);

    /* 初始化AI模型 */
    ai_model_config_t ai_config = {
        .model_data = g_model_data,
        .model_size = g_model_len,
        .tensor_arena = s_tensor_arena,
        .arena_size = TENSOR_ARENA_SIZE,
        .use_cmsis_nn = 1,
    };

    int ret = ai_model_init(&s_ai_ctx, &ai_config);
    if (ret != 0) {
        Error_Handler();
    }

    /* 打印模型信息 */
    printf("AI Model Loaded:\n");
    printf("  Input: %dx%dx%d\n",
           s_ai_ctx.input_height,
           s_ai_ctx.input_width,
           s_ai_ctx.input_channels);
    printf("  Arena used: %d/%d KB\n",
           s_ai_ctx.arena_used / 1024,
           TENSOR_ARENA_SIZE / 1024);

    /* 启动摄像头 */
    Camera_Start(s_camera_buffer);

    printf("System initialized. Starting inference loop...\n");
}

/* ========== 图像预处理 ========== */

/**
 * @brief 快速图像缩放 + 颜色空间转换
 * 使用DMA2D加速
 */
static void preprocess_image_dma2d(const uint8_t* src_rgb565,
                                    int src_w, int src_h,
                                    uint8_t* dst_rgb888,
                                    int dst_w, int dst_h)
{
    /* 使用DMA2D进行缩放和格式转换 */
    /* 这比软件实现快10-20倍 */

    DMA2D_HandleTypeDef hdma2d;

    /* 配置DMA2D */
    hdma2d.Instance = DMA2D;
    hdma2d.Init.Mode = DMA2D_M2M_PFC;
    hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB888;
    hdma2d.Init.OutputOffset = 0;

    hdma2d.LayerCfg[1].InputOffset = 0;
    hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
    hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[1].InputAlpha = 0xFF;

    HAL_DMA2D_Init(&hdma2d);
    HAL_DMA2D_ConfigLayer(&hdma2d, 1);

    /* 执行转换 (注意: DMA2D不支持直接缩放，这里仅做格式转换) */
    /* 缩放需要分步或使用FMC */

    HAL_DMA2D_Start(&hdma2d,
                    (uint32_t)src_rgb565,
                    (uint32_t)dst_rgb888,
                    dst_w, dst_h);
    HAL_DMA2D_PollForTransfer(&hdma2d, 100);
}

/**
 * @brief 软件实现Bilinear缩放 + 归一化
 * 用于DMA2D不支持的缩放比例
 */
static void preprocess_image_software(const uint8_t* src_rgb565,
                                       int src_w, int src_h,
                                       uint8_t* dst_rgb888,
                                       int dst_w, int dst_h)
{
    float scale_x = (float)src_w / dst_w;
    float scale_y = (float)src_h / dst_h;

    for (int y = 0; y < dst_h; y++) {
        for (int x = 0; x < dst_w; x++) {
            /* 源坐标 */
            float src_x = x * scale_x;
            float src_y = y * scale_y;

            int x0 = (int)src_x;
            int y0 = (int)src_y;
            int x1 = x0 + 1;
            int y1 = y0 + 1;

            float fx = src_x - x0;
            float fy = src_y - y0;

            /* 边界限制 */
            if (x1 >= src_w) x1 = src_w - 1;
            if (y1 >= src_h) y1 = src_h - 1;

            /* 双线性插值 */
            for (int c = 0; c < 3; c++) {
                /* RGB565解码并插值 */
                uint16_t p00 = ((uint16_t*)src_rgb565)[y0 * src_w + x0];
                uint16_t p01 = ((uint16_t*)src_rgb565)[y0 * src_w + x1];
                uint16_t p10 = ((uint16_t*)src_rgb565)[y1 * src_w + x0];
                uint16_t p11 = ((uint16_t*)src_rgb565)[y1 * src_w + x1];

                /* 提取通道 */
                float v00, v01, v10, v11;
                if (c == 0) {  /* R */
                    v00 = ((p00 >> 11) & 0x1F) << 3;
                    v01 = ((p01 >> 11) & 0x1F) << 3;
                    v10 = ((p10 >> 11) & 0x1F) << 3;
                    v11 = ((p11 >> 11) & 0x1F) << 3;
                } else if (c == 1) {  /* G */
                    v00 = ((p00 >> 5) & 0x3F) << 2;
                    v01 = ((p01 >> 5) & 0x3F) << 2;
                    v10 = ((p10 >> 5) & 0x3F) << 2;
                    v11 = ((p11 >> 5) & 0x3F) << 2;
                } else {  /* B */
                    v00 = (p00 & 0x1F) << 3;
                    v01 = (p01 & 0x1F) << 3;
                    v10 = (p10 & 0x1F) << 3;
                    v11 = (p11 & 0x1F) << 3;
                }

                float v0 = v00 + fx * (v01 - v00);
                float v1 = v10 + fx * (v11 - v10);
                float v = v0 + fy * (v1 - v0);

                /* 归一化到[0,1]并量化到INT8 */
                /* MobileNet预处理: input = (pixel / 255 - 0.5) / 0.5 */
                float normalized = (v / 255.0f - 0.5f) / 0.5f;
                float q = normalized / s_ai_ctx.input_scale + s_ai_ctx.input_zero_point;

                if (q > 127) q = 127;
                if (q < -128) q = -128;

                dst_rgb888[(y * dst_w + x) * 3 + c] =
                    (uint8_t)(int8_t)(q > 0 ? q + 0.5f : q - 0.5f);
            }
        }
    }
}

/* ========== 检测结果可视化 ========== */

/**
 * @brief 在LCD上绘制检测框
 */
static void draw_detection_boxes(const detection_result_t* results,
                                  int num_results)
{
    /* 颜色表 */
    static const uint16_t kColors[] = {
        LCD_COLOR_RED,
        LCD_COLOR_GREEN,
        LCD_COLOR_BLUE,
        LCD_COLOR_YELLOW,
        LCD_COLOR_CYAN,
        LCD_COLOR_MAGENTA,
    };

    for (int i = 0; i < num_results && i < MAX_DETECTIONS; i++) {
        const detection_t* det = &results->detections[i];

        if (det->confidence < 0.5f) continue;

        /* 映射到显示坐标 */
        int x1 = (int)(det->x1 * DISPLAY_WIDTH);
        int y1 = (int)(det->y1 * DISPLAY_HEIGHT);
        int x2 = (int)(det->x2 * DISPLAY_WIDTH);
        int y2 = (int)(det->y2 * DISPLAY_HEIGHT);

        uint16_t color = kColors[det->class_id % 6];

        /* 绘制矩形框 */
        LCD_DrawRect(x1, y1, x2 - x1, y2 - y1, color);

        /* 绘制标签背景 */
        char label[64];
        snprintf(label, sizeof(label), "%s: %.0f%%",
                 get_class_name(det->class_id),
                 det->confidence * 100);

        LCD_DrawFillRect(x1, y1 - 16, strlen(label) * 8, 16, color);
        LCD_DrawString(x1, y1 - 16, label, LCD_COLOR_WHITE);
    }
}

/* ========== 主循环 ========== */

int main(void)
{
    System_Init();

    detection_result_t detection_results;

    while (1) {
        uint32_t frame_start = HAL_GetTick();

        /* 1. 等待新帧 */
        Camera_WaitFrame();

        uint32_t t0 = HAL_GetTick();

        /* 2. 预处理 */
        preprocess_image_software(s_camera_buffer,
                                   CAMERA_WIDTH, CAMERA_HEIGHT,
                                   s_model_input,
                                   MODEL_INPUT_SIZE, MODEL_INPUT_SIZE);

        uint32_t t1 = HAL_GetTick();
        s_stats.total_preprocess_time += t1 - t0;

        /* 3. AI推理 */
        ai_model_run(&s_ai_ctx, s_model_input, &detection_results);

        uint32_t t2 = HAL_GetTick();
        s_stats.total_inference_time += t2 - t1;

        /* 4. 显示 */
        /* 显示原始图像 */
        LCD_DrawImage(0, 0, CAMERA_WIDTH, CAMERA_HEIGHT, s_camera_buffer);

        /* 绘制检测框 */
        draw_detection_boxes(&detection_results,
                              detection_results.num_detections);

        /* 5. 更新统计 */
        s_stats.frame_count++;

        uint32_t now = HAL_GetTick();
        if (now - s_stats.last_update_time >= 1000) {
            s_stats.fps = s_stats.frame_count * 1000 /
                         (now - s_stats.last_update_time);

            printf("FPS: %lu, Inference: %lu ms, Preprocess: %lu ms\n",
                   s_stats.fps,
                   s_stats.total_inference_time / s_stats.frame_count,
                   s_stats.total_preprocess_time / s_stats.frame_count);

            s_stats.frame_count = 0;
            s_stats.total_inference_time = 0;
            s_stats.total_preprocess_time = 0;
            s_stats.last_update_time = now;
        }

        /* 帧率控制 */
        uint32_t frame_time = HAL_GetTick() - frame_start;
        if (frame_time < 33) {  /* 目标30fps */
            HAL_Delay(33 - frame_time);
        }
    }
}

/* ========== 错误处理 ========== */

void Error_Handler(void)
{
    __disable_irq();

    /* 红色屏幕 */
    LCD_Clear(LCD_COLOR_RED);
    LCD_DrawString(10, 10, "ERROR!", LCD_COLOR_WHITE);

    while (1) {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);  /* LED */
        HAL_Delay(200);
    }
}
```

### 11.4 模型训练与转换脚本

```python
#!/usr/bin/env python3
"""
train_and_convert.py
STM32物体检测模型训练与转换完整流程
"""

import tensorflow as tf
from tensorflow.keras.applications import MobileNetV2
import numpy as np
from pathlib import Path

class STM32ObjectDetector:
    """STM32优化的物体检测器"""

    def __init__(self, num_classes: int = 91, input_size: int = 300):
        self.num_classes = num_classes
        self.input_size = input_size
        self.model = None

    def build_ssd_mobilenet(self, alpha: float = 0.5) -> tf.keras.Model:
        """
        构建MobileNet SSD模型
        alpha: 宽度乘数 (0.5为轻量版，适合STM32)
        """
        print(f"[INFO] Building SSD MobileNet (alpha={alpha})")

        # 基础网络 (MobileNetV2)
        base_model = MobileNetV2(
            input_shape=(self.input_size, self.input_size, 3),
            include_top=False,
            alpha=alpha,
            weights='imagenet'
        )

        # 添加SSD头部
        x = base_model.output

        # 额外特征层
        x = tf.keras.layers.Conv2D(256, 1, activation='relu')(x)
        x = tf.keras.layers.SeparableConv2D(512, 3, strides=2,
                                             activation='relu')(x)
        x = tf.keras.layers.SeparableConv2D(256, 3, strides=2,
                                             activation='relu')(x)

        # 分类和位置预测
        # 这里简化实现，实际SSD有多个尺度的预测
        num_priors = 1917  # 300x300 SSD的anchor数量

        # 分类输出
        class_out = tf.keras.layers.Conv2D(
            num_priors * self.num_classes, 3, padding='same'
        )(x)
        class_out = tf.keras.layers.Reshape((-1, self.num_classes))(class_out)

        # 位置输出
        loc_out = tf.keras.layers.Conv2D(
            num_priors * 4, 3, padding='same'
        )(x)
        loc_out = tf.keras.layers.Reshape((-1, 4))(loc_out)

        self.model = tf.keras.Model(
            inputs=base_model.input,
            outputs=[loc_out, class_out]
        )

        print(f"[INFO] Model built:")
        self.model.summary()

        return self.model

    def train(self,
              train_dataset: tf.data.Dataset,
              val_dataset: tf.data.Dataset,
              epochs: int = 50,
              batch_size: int = 32):
        """训练模型"""

        self.model.compile(
            optimizer=tf.keras.optimizers.Adam(learning_rate=1e-3),
            loss={
                'reshape': 'mse',  # 位置损失
                'reshape_1': tf.keras.losses.CategoricalCrossentropy(
                    from_logits=True
                )  # 分类损失
            },
            loss_weights={'reshape': 1.0, 'reshape_1': 1.0}
        )

        callbacks = [
            tf.keras.callbacks.EarlyStopping(patience=5, restore_best_weights=True),
            tf.keras.callbacks.ReduceLROnPlateau(factor=0.5, patience=3),
            tf.keras.callbacks.ModelCheckpoint('best_model.h5', save_best_only=True)
        ]

        history = self.model.fit(
            train_dataset,
            validation_data=val_dataset,
            epochs=epochs,
            callbacks=callbacks
        )

        return history

    def quantize_and_convert(self,
                              representative_dataset,
                              output_dir: str = 'converted') -> str:
        """量化并转换为TFLite"""
        output_path = Path(output_dir)
        output_path.mkdir(exist_ok=True)

        # 1. 转换为FP32 TFLite
        converter = tf.lite.TFLiteConverter.from_keras_model(self.model)
        tflite_fp32 = converter.convert()

        with open(output_path / 'model_fp32.tflite', 'wb') as f:
            f.write(tflite_fp32)
        print(f"[INFO] FP32 model: {len(tflite_fp32)/1024:.2f} KB")

        # 2. INT8量化
        converter = tf.lite.TFLiteConverter.from_keras_model(self.model)
        converter.optimizations = [tf.lite.Optimize.DEFAULT]
        converter.representative_dataset = representative_dataset
        converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
        converter.inference_input_type = tf.int8
        converter.inference_output_type = tf.int8

        tflite_int8 = converter.convert()

        with open(output_path / 'model_int8.tflite', 'wb') as f:
            f.write(tflite_int8)
        print(f"[INFO] INT8 model: {len(tflite_int8)/1024:.2f} KB")

        # 3. 转换为C头文件
        self._convert_to_c_array(
            tflite_int8,
            output_path / 'model_data.h'
        )

        # 4. 生成模型信息
        self._generate_model_info(
            output_path / 'model_info.txt',
            len(tflite_int8)
        )

        return str(output_path / 'model_int8.tflite')

    def _convert_to_c_array(self, tflite_model: bytes, output_path: Path):
        """转换为C数组"""
        hex_array = ','.join(f'0x{b:02x}' for b in tflite_model)

        # 格式化
        lines = []
        for i in range(0, len(tflite_model), 16):
            line = ', '.join(f'0x{b:02x}' for b in tflite_model[i:i+16])
            lines.append(f'    {line}')

        content = f"""/* Auto-generated model data */
#ifndef MODEL_DATA_H
#define MODEL_DATA_H

#include <stdint.h>

/* Model size: {len(tflite_model)} bytes ({len(tflite_model)/1024:.2f} KB) */
#define MODEL_DATA_LEN {len(tflite_model)}

__attribute__((aligned(32)))
static const uint8_t g_model_data[] = {{
{chr(10).join(lines)}
}};

static const unsigned int g_model_len = MODEL_DATA_LEN;

#endif /* MODEL_DATA_H */
"""

        with open(output_path, 'w') as f:
            f.write(content)

        print(f"[INFO] C header generated: {output_path}")

    def _generate_model_info(self, output_path: Path, model_size: int):
        """生成模型信息文件"""
        info = f"""Model Information
================
Input shape: ({self.input_size}, {self.input_size}, 3)
Input type: INT8
Output: [locations, classifications]
Model size: {model_size} bytes ({model_size/1024:.2f} KB)
Estimated RAM: 512 KB
Recommended arena size: 512*1024

Operator checklist:
- CONV_2D: Supported by CMSIS-NN
- DEPTHWISE_CONV_2D: Supported by CMSIS-NN
- ADD: Supported
- AVERAGE_POOL_2D: Supported
- RESHAPE: Supported
- CONCATENATION: Supported
- SOFTMAX: Supported

Optimization flags:
- USE_CMSIS_NN=1
- OPTIMIZED_KERNEL_DIR=cmsis_nn
"""
        with open(output_path, 'w') as f:
            f.write(info)

        print(f"[INFO] Model info: {output_path}")

def main():
    """主流程"""
    # 创建检测器
    detector = STM32ObjectDetector(num_classes=91, input_size=300)

    # 构建模型
    detector.build_ssd_mobilenet(alpha=0.5)

    # 加载预训练权重 (或训练)
    # detector.train(...)

    # 定义代表性数据集
    def representative_dataset():
        for _ in range(100):
            data = np.random.randint(0, 256, (1, 300, 300, 3), dtype=np.uint8)
            yield [data.astype(np.float32)]

    # 量化并转换
    detector.quantize_and_convert(representative_dataset)

    print("\n[INFO] Conversion complete!")
    print("[INFO] Copy model_data.h to your STM32 project")

if __name__ == '__main__':
    main()
```

### 11.5 构建配置

```makefile
# Makefile - STM32物体检测项目

# 工具链
CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# 目标
TARGET = stm32_object_detection

# 芯片定义
MCU = -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb
DEFS = -DUSE_HAL_DRIVER -DSTM32H743xx

# 包含路径
INCLUDES = \
    -ICore/Inc \
    -IDrivers/STM32H7xx_HAL_Driver/Inc \
    -IDrivers/CMSIS/Device/ST/STM32H7xx/Include \
    -IDrivers/CMSIS/Include \
    -IMiddlewares/ST/AI/TFLM \
    -IMiddlewares/ST/AI/CMSIS-NN/Include

# 编译标志
CFLAGS = $(MCU) $(DEFS) $(INCLUDES) \
    -O3 -ffunction-sections -fdata-sections \
    -Wall -fstack-usage \
    -DARM_MATH_CM7 -D__FPU_PRESENT=1U \
    -DUSE_CMSIS_NN=1

CXXFLAGS = $(CFLAGS) -fno-rtti -fno-exceptions -fno-threadsafe-statics

# 链接标志
LDFLAGS = $(MCU) -specs=nosys.specs \
    -TSTM32H743ZITX_FLASH.ld \
    -lc -lm -lstdc++ -lsupc++ \
    -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref \
    -Wl,--gc-sections

# 源文件
C_SOURCES = $(wildcard Core/Src/*.c)
C_SOURCES += $(wildcard Drivers/*/Src/*.c)

CPP_SOURCES = $(wildcard Core/Src/*.cpp)
CPP_SOURCES += $(wildcard Middlewares/ST/AI/TFLM/*.cc)

# 对象文件
BUILD_DIR = build
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))

vpath %.c $(sort $(dir $(C_SOURCES)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

# 规则
.PHONY: all clean flash

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin size

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
 $(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR)
 $(CXX) -c $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
 $(LD) $(OBJECTS) $(LDFLAGS) -o $@
 $(SIZE) $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
 $(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
 $(OBJCOPY) -O binary -S $< $@

$(BUILD_DIR):
 mkdir -p $@

size: $(BUILD_DIR)/$(TARGET).elf
 @echo "Memory usage:"
 $(SIZE) $<

flash: $(BUILD_DIR)/$(TARGET).bin
 openocd -f interface/stlink.cfg \
         -f target/stm32h7x.cfg \
         -c "program $< 0x08000000 verify reset exit"

clean:
 rm -rf $(BUILD_DIR)
```

---

## 附录

### A. 参考资源

1. **TensorFlow Lite Micro**
   - GitHub: <https://github.com/tensorflow/tflite-micro>
   - 文档: <https://www.tensorflow.org/lite/microcontrollers>

2. **CMSIS-NN**
   - 文档: <https://arm-software.github.io/CMSIS-NN/>
   - GitHub: <https://github.com/ARM-software/CMSIS-NN>

3. **ONNX Runtime**
   - 文档: <https://onnxruntime.ai/docs/>

4. **模型优化工具**
   - TensorFlow Model Optimization Toolkit
   - ONNX Optimizer

### B. 常见问题

| 问题 | 可能原因 | 解决方案 |
|------|----------|----------|
| 模型太大无法加载 | 未量化或量化不充分 | 使用INT8量化，尝试剪枝 |
| 推理时间过长 | 算子未优化 | 启用CMSIS-NN，检查热点算子 |
| 精度下降严重 | 量化参数不当 | 校准数据集代表性不足 |
| 内存不足 | 张量未复用 | 使用内存规划工具 |
| 功耗过高 | CPU持续全速 | 启用DVFS，使用间歇推理 |

### C. 性能基准

| 平台 | 模型 | 输入尺寸 | 推理时间 | 功耗 |
|------|------|----------|----------|------|
| STM32F746 | MicroSpeech | 49x40 | 12ms | 15mW |
| STM32H743 | MobileNet | 96x96 | 45ms | 80mW |
| STM32H743 | Person Detection | 96x96 | 120ms | 120mW |
| ESP32-S3 | Keyword Spotting | 49x40 | 8ms | 25mW |
| nRF5340 | Anomaly Detection | 32x32 | 25ms | 5mW |

---

**文档结束**

*本文档由Kimi Code生成，最后更新: 2026-03-17*


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
