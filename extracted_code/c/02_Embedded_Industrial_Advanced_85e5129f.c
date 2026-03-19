/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Industrial_Advanced.md
 * Line: 1651
 * Language: c
 * Block ID: 85e5129f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// DVFS: 根据负载动态调整CPU频率和电压
// 降低功耗和发热
// ============================================================

#include "esp_pm.h"
#include "driver/rtc_io.h"

// ESP32 DVFS配置
typedef enum {
    PERF_MODE_HIGH,     // 240MHz，最高性能
    PERF_MODE_NORMAL,   // 160MHz，平衡
    PERF_MODE_LOW,      // 80MHz，低功耗
    PERF_MODE_MIN       // 10MHz，最小功耗
} PerformanceMode;

void set_performance_mode(PerformanceMode mode) {
    esp_pm_config_esp32_t pm_config;

    switch (mode) {
        case PERF_MODE_HIGH:
            pm_config.max_freq_mhz = 240;
            pm_config.min_freq_mhz = 240;
            break;
        case PERF_MODE_NORMAL:
            pm_config.max_freq_mhz = 160;
            pm_config.min_freq_mhz = 80;
            break;
        case PERF_MODE_LOW:
            pm_config.max_freq_mhz = 80;
            pm_config.min_freq_mhz = 10;
            break;
        case PERF_MODE_MIN:
            pm_config.max_freq_mhz = 10;
            pm_config.min_freq_mhz = 10;
            break;
    }

    pm_config.light_sleep_enable = true;
    esp_pm_configure(&pm_config);
}

// 自适应频率调节
void adaptive_frequency_scaling(void) {
    static uint32_t idle_ticks = 0;
    static uint32_t busy_ticks = 0;

    // 统计CPU利用率
    // 通过监测空闲任务运行时间

    float cpu_usage = (float)busy_ticks / (idle_ticks + busy_ticks);

    if (cpu_usage > 0.8f) {
        set_performance_mode(PERF_MODE_HIGH);
    } else if (cpu_usage > 0.5f) {
        set_performance_mode(PERF_MODE_NORMAL);
    } else if (cpu_usage > 0.2f) {
        set_performance_mode(PERF_MODE_LOW);
    } else {
        set_performance_mode(PERF_MODE_MIN);
    }
}

// 温度监控与降频
#define TEMP_HIGH_THRESHOLD     75  // 75°C高温阈值
#define TEMP_CRITICAL_THRESHOLD 85  // 85°C临界阈值

void thermal_management(void) {
    int temp_c = read_internal_temperature();

    if (temp_c > TEMP_CRITICAL_THRESHOLD) {
        // 紧急降频
        set_performance_mode(PERF_MODE_MIN);
        // 触发报警
        trigger_overheat_alarm();
    } else if (temp_c > TEMP_HIGH_THRESHOLD) {
        // 降频降温
        set_performance_mode(PERF_MODE_LOW);
    }
}
