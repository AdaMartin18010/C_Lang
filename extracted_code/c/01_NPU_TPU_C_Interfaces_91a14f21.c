/*
 * Auto-generated from: 16_Embedded_AI_Agents\07_Hardware_Acceleration\01_NPU_TPU_C_Interfaces.md
 * Line: 525
 * Language: c
 * Block ID: 91a14f21
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
