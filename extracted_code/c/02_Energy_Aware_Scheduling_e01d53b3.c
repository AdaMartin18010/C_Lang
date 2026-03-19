/*
 * Auto-generated from: 04_Industrial_Scenarios\10_Deep_Sea\02_Energy_Aware_Scheduling.md
 * Line: 357
 * Language: c
 * Block ID: e01d53b3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 自适应传感器采样
// 根据能量状态动态调整采样率
// ============================================================================

typedef struct {
    uint32_t sensor_id;
    float min_interval_s;
    float max_interval_s;
    float current_interval_s;
    float energy_per_sample_mj;

    // 数据变化率估计
    float last_value;
    float change_rate;

    // 采样函数
    float (*read_sensor)(void);
} AdaptiveSensor;

// 计算最优采样间隔
float compute_optimal_interval(AdaptiveSensor *sensor,
                                EnergyManager *mgr,
                                float priority) {
    // 能量充足时，根据数据变化率调整
    float energy_ratio = mgr->total_stored_mj / mgr->total_capacity_mj;

    if (energy_ratio > 0.7f) {
        // 能量充足 - 基于变化率
        if (sensor->change_rate > 0.5f) {
            return sensor->min_interval_s;
        } else {
            return sensor->max_interval_s * 0.5f;
        }
    } else if (energy_ratio > 0.3f) {
        // 中等能量 - 标准间隔
        return (sensor->min_interval_s + sensor->max_interval_s) / 2.0f;
    } else {
        // 能量紧张 - 最大间隔
        return sensor->max_interval_s;
    }
}

// 自适应采样
void adaptive_sample(AdaptiveSensor *sensor, EnergyManager *mgr) {
    float value = sensor->read_sensor();

    // 更新变化率
    sensor->change_rate = fabsf(value - sensor->last_value) /
                          sensor->current_interval_s;
    sensor->last_value = value;

    // 计算下次采样间隔
    sensor->current_interval_s = compute_optimal_interval(sensor, mgr, 1.0f);
}
