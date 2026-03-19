/*
 * Auto-generated from: 11_Machine_Learning_C\03_Embedded_AI_Deployment.md
 * Line: 78
 * Language: c
 * Block ID: 2f2a9678
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
