/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Cryogenic_Superconducting\02_Subthreshold_Optimization.md
 * Line: 195
 * Language: c
 * Block ID: d76ba66d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 能量最优工作点计算
// ============================================================================

typedef struct {
    float supply_voltage;
    float clock_freq;
    float dynamic_power;
    float leakage_power;
    float total_energy_per_op;
    float delay;
} OperatingPoint;

// 计算给定工作点的能耗
OperatingPoint calculate_energy_point(const ProcessParams *proc,
                                       float vdd, float activity_factor,
                                       float cap_load, float w, float l) {
    OperatingPoint point;
    point.supply_voltage = vdd;

    // 亚阈值区延迟模型
    // τ ∝ (n·VT / (Vdd - Vth)) · (C·Vdd / I0)
    float vt = VT_300K;
    float ion = subthreshold_current(proc, vdd, vdd, 0.0f, w, l, true);

    point.delay = (proc->n_subthreshold * vt / (vdd - proc->vth_n)) *
                  (cap_load * vdd / ion);

    point.clock_freq = 1.0f / (2.0f * point.delay);  // 简化为2级反相器链

    // 动态功耗
    point.dynamic_power = activity_factor * cap_load * vdd * vdd * point.clock_freq;

    // 漏电流功耗
    float ileak = subthreshold_current(proc, 0.0f, vdd, 0.0f, w, l, true);
    point.leakage_power = vdd * ileak;

    // 每操作能量
    point.total_energy_per_op = (point.dynamic_power + point.leakage_power) /
                                 point.clock_freq;

    return point;
}

// 搜索能量最优工作点
float find_energy_optimal_vdd(const ProcessParams *proc,
                               float activity_factor,
                               float cap_load, float w, float l,
                               float vdd_min, float vdd_max) {
    float optimal_vdd = vdd_min;
    float min_energy = 1e10f;

    float vdd_step = 0.01f;  // 10mV步进

    for (float vdd = vdd_min; vdd <= vdd_max; vdd += vdd_step) {
        OperatingPoint point = calculate_energy_point(proc, vdd, activity_factor,
                                                       cap_load, w, l);

        if (point.total_energy_per_op < min_energy) {
            min_energy = point.total_energy_per_op;
            optimal_vdd = vdd;
        }
    }

    return optimal_vdd;
}
