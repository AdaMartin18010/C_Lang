# NPU/TPU硬件加速C语言接口

> **文档状态**: 🆕 重要补充
> **创建日期**: 2026-03-18
> **覆盖硬件**: ARM Ethos-U, Google Coral, Intel NCS, NVIDIA Jetson

---

## 概述

专用神经网络加速器(NPU/TPU)可提供比通用CPU高10-100倍的能效比。本节详细介绍如何在C语言中集成主流嵌入式AI加速器。

### 硬件对比

| 加速器 | 算力 | 功耗 | 接口 | 适用场景 |
|:-----|:-----|:-----|:-----|:---------|
| **ARM Ethos-U55** | 512 MAC/cycle | <0.5mW | AXI | MCU级设备 |
| **Google Coral TPU** | 4 TOPS | 2W | USB/PCIe/M.2 | 边缘网关 |
| **Intel NCS2** | 1 TOPS | 1W | USB | 开发原型 |
| **NVIDIA Jetson Nano** | 472 GFLOPS | 10W | PCIe/内存 | 边缘AI |
| **Himax HX6537** | 2 TOPS | <100mW | 片上 | 电池设备 |

---


---

## 📑 目录

- [NPU/TPU硬件加速C语言接口](#nputpu硬件加速c语言接口)
  - [概述](#概述)
    - [硬件对比](#硬件对比)
  - [📑 目录](#-目录)
  - [ARM Ethos-U (微控制器NPU)](#arm-ethos-u-微控制器npu)
    - [架构概述](#架构概述)
    - [Vela编译器与模型部署](#vela编译器与模型部署)
    - [C语言驱动代码](#c语言驱动代码)
  - [Google Coral Edge TPU](#google-coral-edge-tpu)
    - [libedgetpu C API](#libedgetpu-c-api)
  - [NVIDIA Jetson (CUDA/TensoRT C API)](#nvidia-jetson-cudatensort-c-api)
  - [通用抽象层设计](#通用抽象层设计)
  - [性能基准](#性能基准)
    - [相同模型在不同硬件上的对比](#相同模型在不同硬件上的对比)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## ARM Ethos-U (微控制器NPU)

### 架构概述

```
┌─────────────────────────────────────────────────────────────┐
│                   Ethos-U55 NPU                             │
├─────────────────────────────────────────────────────────────┤
│  MAC引擎 (512 MACs/cycle)                                    │
│  ├─ 卷积加速                                                │
│  ├─ 深度可分离卷积                                          │
│  ├─ 全连接层                                                │
│  └─ 激活函数                                                │
├─────────────────────────────────────────────────────────────┤
│  DMA引擎 (高效数据传输)                                       │
├─────────────────────────────────────────────────────────────┤
│  命令流解析器 (从Cortex-M接收命令)                            │
└─────────────────────────────────────────────────────────────┘
         │
         │ AXI接口
         ▼
┌─────────────────┐
│   Cortex-M55    │ ← 主控MCU运行C代码
└─────────────────┘
```

### Vela编译器与模型部署

```bash
# 1. 安装Vela编译器

---

## 🔗 全面知识关联体系

### 【全局层】知识库导航
| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../../00_GLOBAL_INDEX.md](../../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../../readme.md](../../readme.md) | 模块总览与目录导航 |
| **学习路径** | [../../06_Thinking_Representation/06_Learning_Paths/readme.md](../../06_Thinking_Representation/06_Learning_Paths/readme.md) | 阶段化学习路线规划 |
| **概念映射** | [../../06_Thinking_Representation/05_Concept_Mappings/readme.md](../../06_Thinking_Representation/05_Concept_Mappings/readme.md) | 核心概念等价关系图 |

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
| **理论基础** | [../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md](../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md) | 语义学理论基础 |
| **核心机制** | [../../01_Core_Knowledge_System/02_Core_Layer/readme.md](../../01_Core_Knowledge_System/02_Core_Layer/readme.md) | C语言核心机制 |
| **标准接口** | [../../01_Core_Knowledge_System/04_Standard_Library_Layer/readme.md](../../01_Core_Knowledge_System/04_Standard_Library_Layer/readme.md) | 标准库API |
| **系统实现** | [../../03_System_Technology_Domains/readme.md](../../03_System_Technology_Domains/readme.md) | 系统级实现 |

### 【局部层】横向关联网
| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md](../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 并发编程技术 |
| **安全规范** | [../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md](../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md) | 安全编码标准 |
| **工具支持** | [../../07_Modern_Toolchain/readme.md](../../07_Modern_Toolchain/readme.md) | 现代开发工具链 |
| **实践案例** | [../../04_Industrial_Scenarios/readme.md](../../04_Industrial_Scenarios/readme.md) | 工业实践场景 |

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
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../../06_Thinking_Representation/01_Decision_Trees/readme.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../../04_Industrial_Scenarios/readme.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../../02_Formal_Semantics_and_Physics/readme.md) |

---

pip install ethos-u-vela

# 2. 将TFLite模型编译为Ethos-U格式
vela --accelerator-config ethos-u55-256 \
     --config vela_config.ini \
     --output-dir ./optimized \
     model.tflite

# 输出: model_vela.tflite (包含Ethos-U命令流)
```

### C语言驱动代码

```c
// ethos_u_driver.c
#include "ethos_u.h"
#include "pmu_ethosu.h"

// Ethos-U设备句柄
typedef struct {
    struct ethosu_driver drv;       // 底层驱动
    void* cmd_data;                  // 命令流地址
    void* weight_data;               // 权重地址
    void* scratch_data;              // 临时缓冲区
} ethos_u_ctx_t;

// 初始化Ethos-U
int ethos_u_init(ethos_u_ctx_t* ctx,
                void* cmd_base,
                void* weight_base,
                void* scratch_base,
                size_t scratch_size) {

    ctx->cmd_data = cmd_base;
    ctx->weight_data = weight_base;
    ctx->scratch_data = scratch_base;

    // 配置NPU
    struct ethosu_config cfg = {
        .queue_size = 1,
        .custom_dma = NULL,
    };

    // 初始化驱动
    int status = ethosu_init(&ctx->drv,
                            ETHOS_U_BASE_ADDR,  // 0x48102000
                            cmd_base,
                            weight_base,
                            scratch_base,
                            scratch_size,
                            &cfg);

    if (status != 0) {
        printf("Ethos-U init failed: %d\n", status);
        return -1;
    }

    printf("Ethos-U initialized successfully\n");
    printf("  Version: %d.%d.%d\n",
           ethosu_get_version(&ctx->drv) >> 16,
           (ethosu_get_version(&ctx->drv) >> 8) & 0xFF,
           ethosu_get_version(&ctx->drv) & 0xFF);

    return 0;
}

// 执行推理
int ethos_u_inference(ethos_u_ctx_t* ctx,
                     const void* input_data,
                     void* output_data,
                     size_t input_size,
                     size_t output_size) {

    // 准备输入数据 (DMA传输到NPU可访问内存)
    void* input_npu = ethosu_map_input(&ctx->drv, input_size);
    memcpy(input_npu, input_data, input_size);
    ethosu_flush_input(&ctx->drv, input_size);

    // 创建推理任务
    struct ethosu_inference_job job = {
        .input = input_npu,
        .output = ethosu_map_output(&ctx->drv, output_size),
        .custom_data = NULL,
    };

    // 提交到NPU执行
    int status = ethosu_schedule_inference(&ctx->drv, &job);
    if (status != 0) {
        return -1;
    }

    // 等待完成 (可配置为中断或轮询)
    #ifdef ETHOS_U_INTERRUPT_MODE
    // 等待中断信号
    ethosu_wait_for_completion(&ctx->drv, &job);
    #else
    // 轮询模式
    while (!ethosu_is_inference_done(&ctx->drv, &job)) {
        __WFI();  // 等待中断
    }
    #endif

    // 获取输出
    ethosu_invalidate_output(&ctx->drv, output_size);
    memcpy(output_data, job.output, output_size);

    return 0;
}

// 获取性能计数器
void ethos_u_get_perf_counters(ethos_u_ctx_t* ctx,
                              uint32_t* cycles,
                              uint32_t* mac_count,
                              uint32_t* dma_stalls) {
    struct ethosu_pmu_counters counters;
    ethosu_get_pmu_counters(&ctx->drv, &counters);

    *cycles = counters.cycle_count;
    *mac_count = counters.event_count[ETHOSU_PMU_NPU_ACTIVE];
    *dma_stalls = counters.event_count[ETHOSU_PMU_SRAM_ACCESS_STALL];
}

// 完整使用示例
void ethos_u_example(void) {
    // 从Vela编译器生成的文件加载
    extern const uint8_t model_cmd_data[];      // 命令流
    extern const uint8_t model_weight_data[];   // 权重

    ethos_u_ctx_t ctx;

    // 分配scratch内存 (通常256KB-512KB)
    static uint8_t scratch_buffer[512 * 1024] __attribute__((aligned(16)));

    // 初始化
    ethos_u_init(&ctx,
                (void*)model_cmd_data,
                (void*)model_weight_data,
                scratch_buffer,
                sizeof(scratch_buffer));

    // 输入数据
    int8_t input[224 * 224 * 3];
    int8_t output[1000];  // 分类结果

    // 准备输入 (量化等)
    prepare_input(input);

    // 执行推理
    uint32_t start = get_cycle_count();
    ethos_u_inference(&ctx, input, output, sizeof(input), sizeof(output));
    uint32_t cycles = get_cycle_count() - start;

    printf("Inference completed in %u cycles\n", cycles);
    printf("  Equivalent to %.2f ms @ 480MHz\n",
           cycles / 480000.0f);

    // 获取性能数据
    uint32_t npu_cycles, mac_ops, dma_stall;
    ethos_u_get_perf_counters(&ctx, &npu_cycles, &mac_ops, &dma_stall);

    printf("NPU MAC utilization: %.1f%%\n",
           100.0f * mac_ops / npu_cycles);
}
```

---

## Google Coral Edge TPU

### libedgetpu C API

```c
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
```

---

## NVIDIA Jetson (CUDA/TensoRT C API)

```c
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
```

---

## 通用抽象层设计

```c
// embedded_npu_hal.h
// 硬件抽象层 - 统一的NPU接口

#ifndef EMBEDDED_NPU_HAL_H
#define EMBEDDED_NPU_HAL_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    NPU_TYPE_NONE = 0,
    NPU_TYPE_ETHOS_U,
    NPU_TYPE_CORAL_TPU,
    NPU_TYPE_INTEL_NCS,
    NPU_TYPE_JETSON_GPU,
    NPU_TYPE_CUSTOM
} npu_type_t;

typedef struct {
    npu_type_t type;
    const char* name;
    uint32_t max_tops;
    uint32_t power_mw;
    void* driver_ctx;
} npu_info_t;

typedef struct {
    int (*init)(void** ctx, const void* model_data, size_t model_size);
    int (*inference)(void* ctx, const void* input, void* output,
                    size_t input_size, size_t output_size);
    int (*get_info)(void* ctx, npu_info_t* info);
    int (*profile)(void* ctx, float* inference_ms, float* power_mw);
    void (*deinit)(void* ctx);
} npu_driver_t;

// 自动检测并初始化可用的NPU
npu_type_t npu_auto_detect(void);
int npu_init(npu_type_t type, npu_driver_t** driver, void** ctx);

#endif
```

---

## 性能基准

### 相同模型在不同硬件上的对比

| 硬件 | MobileNetV2延迟 | 功耗 | 能效 |
|:-----|:----------------|:-----|:-----|
| **Cortex-M55 (纯CPU)** | 450ms | 150mW | 低 |
| **+ Ethos-U55** | 12ms | 180mW | **高** |
| **Coral USB** | 2.5ms | 2W | **极高** |
| **Jetson Nano** | 8ms | 10W | 中 |

---

**上一章**: [../08_Real_Time_AI/01_Deterministic_Inference.md](../08_Real_Time_AI/01_Deterministic_Inference.md)
**下一章**: [../09_Security_Reliability/01_AI_Safety_Standards.md](../09_Security_Reliability/01_AI_Safety_Standards.md)

**最后更新**: 2026-03-18


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
