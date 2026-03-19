/*
 * Auto-generated from: 04_Industrial_Scenarios\10_Deep_Sea\02_Energy_Aware_Scheduling.md
 * Line: 71
 * Language: c
 * Block ID: 81390a52
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 深海能量管理系统
// ============================================================================

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

// 能量来源类型
typedef enum {
    SOURCE_BATTERY,     // 主电池
    SOURCE_TIDAL,       // 潮汐能收集
    SOURCE_THERMAL,     // 温差能
    SOURCE_HARVESTING   // 其他收集
} EnergySourceType;

// 能量源
typedef struct {
    EnergySourceType type;
    float current_power_mw;     // 当前功率 (mW)
    float capacity_mj;          // 容量 (mJ)
    float stored_energy_mj;     // 存储能量 (mJ)
    float efficiency;           // 转换效率
    float min_voltage;          // 最小工作电压
    float max_voltage;          // 最大电压
    uint32_t cycle_count;       // 充放电循环
} EnergySource;

// 能量预算管理器
typedef struct {
    EnergySource sources[4];
    uint8_t num_sources;

    float total_capacity_mj;
    float total_stored_mj;
    float consumption_rate_mw;
    float harvest_rate_mw;

    // 历史数据 (用于预测)
    float power_history[24];    // 24小时历史
    uint8_t history_index;
} EnergyManager;

// 初始化能量管理器
void energy_manager_init(EnergyManager *mgr) {
    memset(mgr, 0, sizeof(EnergyManager));
    mgr->num_sources = 0;
    mgr->total_capacity_mj = 0;
    mgr->total_stored_mj = 0;
}

// 添加能量源
void energy_add_source(EnergyManager *mgr, EnergySource *source) {
    if (mgr->num_sources >= 4) return;

    mgr->sources[mgr->num_sources] = *source;
    mgr->total_capacity_mj += source->capacity_mj;
    mgr->total_stored_mj += source->stored_energy_mj;
    mgr->num_sources++;
}

// 更新能量状态 (定期调用，如每分钟)
void energy_update(EnergyManager *mgr, float dt_s) {
    // 收集能量
    for (int i = 0; i < mgr->num_sources; i++) {
        EnergySource *src = &mgr->sources[i];

        if (src->type == SOURCE_BATTERY) {
            // 电池放电
            src->stored_energy_mj -= src->current_power_mw * dt_s / 1000.0f;
        } else {
            // 收集器充电
            float harvested = src->current_power_mw * src->efficiency * dt_s / 1000.0f;
            src->stored_energy_mj += harvested;
        }

        // 限制范围
        if (src->stored_energy_mj < 0) src->stored_energy_mj = 0;
        if (src->stored_energy_mj > src->capacity_mj) {
            src->stored_energy_mj = src->capacity_mj;
        }
    }

    // 更新总量
    mgr->total_stored_mj = 0;
    mgr->harvest_rate_mw = 0;
    for (int i = 0; i < mgr->num_sources; i++) {
        mgr->total_stored_mj += mgr->sources[i].stored_energy_mj;
        if (mgr->sources[i].type != SOURCE_BATTERY) {
            mgr->harvest_rate_mw += mgr->sources[i].current_power_mw;
        }
    }

    // 记录历史
    mgr->power_history[mgr->history_index] = mgr->harvest_rate_mw;
    mgr->history_index = (mgr->history_index + 1) % 24;
}

// 预测未来可用能量
float energy_predict_available(EnergyManager *mgr, float hours_ahead) {
    // 简单预测: 基于历史平均
    float avg_harvest = 0;
    for (int i = 0; i < 24; i++) {
        avg_harvest += mgr->power_history[i];
    }
    avg_harvest /= 24.0f;

    float predicted = mgr->total_stored_mj +
                      avg_harvest * hours_ahead * 3600.0f / 1000.0f;

    return fminf(predicted, mgr->total_capacity_mj);
}
