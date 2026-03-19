/*
 * Auto-generated from: 16_Embedded_AI_Agents\09_Security_Reliability\01_AI_Safety_Standards.md
 * Line: 176
 * Language: c
 * Block ID: 6254a28e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ai_safety_monitor.c
#include "ai_safety_monitor.h"
#include <string.h>
#include <stdio.h>

int safety_monitor_init(safety_monitor_t* monitor) {
    memset(monitor, 0, sizeof(*monitor));
    monitor->current_state = SAFETY_STATE_NORMAL;

    // 默认阈值 (ASIL D级别)
    monitor->max_inference_time_ms = 50.0f;      // 50ms
    monitor->min_confidence_threshold = 0.95f;   // 95%
    monitor->max_temperature = 85.0f;            // 85°C
    monitor->thresholds.timeout_count = 3;
    monitor->thresholds.anomaly_count = 10;

    return 0;
}

safety_state_t safety_monitor_check(safety_monitor_t* monitor,
                                    const ai_health_metrics_t* current) {
    safety_state_t new_state = monitor->current_state;

    // 检查推理时间
    if (current->inference_time_ms > monitor->max_inference_time_ms) {
        monitor->metrics.timeout_count++;
        if (monitor->metrics.timeout_count >= monitor->thresholds.timeout_count) {
            new_state = SAFETY_STATE_FAIL_SAFE;
            printf("[SAFETY] Inference timeout violation: %.1f ms\n",
                   current->inference_time_ms);
        }
    } else {
        monitor->metrics.timeout_count = 0;  // 重置
    }

    // 检查置信度
    if (current->confidence_score < monitor->min_confidence_threshold) {
        new_state = SAFETY_STATE_DEGRADED;
        printf("[SAFETY] Low confidence: %.2f\n", current->confidence_score);
    }

    // 检查温度
    if (current->temperature > monitor->max_temperature) {
        new_state = SAFETY_STATE_EMERGENCY_STOP;
        printf("[SAFETY] Over temperature: %.1f°C\n", current->temperature);
    }

    // 检查输入数据质量
    if (current->input_data_quality < 50) {  // 质量分数0-100
        new_state = SAFETY_STATE_DEGRADED;
        printf("[SAFETY] Poor input data quality: %u\n",
               current->input_data_quality);
    }

    // 状态转换
    if (new_state != monitor->current_state) {
        if (monitor->on_state_change) {
            monitor->on_state_change(monitor->current_state, new_state);
        }

        if (new_state == SAFETY_STATE_EMERGENCY_STOP && monitor->on_emergency) {
            monitor->on_emergency("Critical safety violation detected");
        }

        monitor->current_state = new_state;
    }

    // 更新指标
    monitor->metrics = *current;

    return new_state;
}

int safety_monitor_set_fail_safe(safety_monitor_t* monitor) {
    monitor->current_state = SAFETY_STATE_FAIL_SAFE;

    // 执行安全动作
    printf("[SAFETY] Entering fail-safe mode\n");

    // 示例: 切换到备份系统、降低车速等

    return 0;
}

// 使用示例
void safety_example(void) {
    safety_monitor_t monitor;
    safety_monitor_init(&monitor);

    // 设置回调
    monitor.on_state_change = [](safety_state_t old, safety_state_t new_s) {
        printf("State change: %d -> %d\n", old, new_s);
    };

    monitor.on_emergency = [](const char* reason) {
        printf("EMERGENCY: %s\n", reason);
        trigger_emergency_brake();
    };

    // 主循环
    while (1) {
        ai_health_metrics_t metrics = {0};

        // 收集指标
        metrics.inference_time_ms = measure_inference_time();
        metrics.confidence_score = get_model_confidence();
        metrics.temperature = read_temperature_sensor();
        metrics.input_data_quality = validate_input_data();

        // 安全检查
        safety_state_t state = safety_monitor_check(&monitor, &metrics);

        // 根据状态执行动作
        switch (state) {
            case SAFETY_STATE_NORMAL:
                execute_normal_control();
                break;
            case SAFETY_STATE_DEGRADED:
                execute_degraded_control();  // 保守策略
                break;
            case SAFETY_STATE_FAIL_SAFE:
                execute_fail_safe();         // 最小风险
                break;
            case SAFETY_STATE_EMERGENCY_STOP:
                emergency_stop();            // 立即停止
                return;
        }
    }
}
