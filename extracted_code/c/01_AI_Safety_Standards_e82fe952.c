/*
 * Auto-generated from: 16_Embedded_AI_Agents\09_Security_Reliability\01_AI_Safety_Standards.md
 * Line: 115
 * Language: c
 * Block ID: e82fe952
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ai_safety_monitor.h
#ifndef AI_SAFETY_MONITOR_H
#define AI_SAFETY_MONITOR_H

#include <stdint.h>
#include <stdbool.h>

// 安全监控状态
typedef enum {
    SAFETY_STATE_NORMAL = 0,        // 正常运行
    SAFETY_STATE_DEGRADED,          // 性能降级
    SAFETY_STATE_FAIL_SAFE,         // 进入安全状态
    SAFETY_STATE_EMERGENCY_STOP     // 紧急停止
} safety_state_t;

// 监控指标
typedef struct {
    // 性能指标
    float inference_time_ms;        // 推理时间
    float confidence_score;         // 置信度
    uint32_t input_data_quality;    // 输入数据质量分数

    // 资源指标
    float cpu_utilization;          // CPU使用率
    float memory_utilization;       // 内存使用率
    float temperature;              // 温度

    // 安全指标
    uint32_t anomaly_count;         // 异常检测计数
    uint32_t timeout_count;         // 超时计数
    uint32_t checksum_failures;     // 校验失败次数
} ai_health_metrics_t;

// 安全监控器上下文
typedef struct {
    safety_state_t current_state;
    ai_health_metrics_t metrics;
    ai_health_metrics_t thresholds;

    // 安全目标
    float max_inference_time_ms;
    float min_confidence_threshold;
    float max_temperature;

    // 回调函数
    void (*on_state_change)(safety_state_t old_state, safety_state_t new_state);
    void (*on_emergency)(const char* reason);
} safety_monitor_t;

// API
int safety_monitor_init(safety_monitor_t* monitor);
safety_state_t safety_monitor_check(safety_monitor_t* monitor,
                                    const ai_health_metrics_t* current);
int safety_monitor_set_fail_safe(safety_monitor_t* monitor);

#endif
