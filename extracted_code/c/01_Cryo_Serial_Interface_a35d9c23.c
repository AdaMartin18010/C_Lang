/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Cryogenic_Superconducting\01_Cryo_Serial_Interface.md
 * Line: 263
 * Language: c
 * Block ID: a35d9c23
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 热负载计算器
// 评估接口对制冷系统的影响
// ============================================================================

typedef struct {
    // 线缆参数
    float length_m;
    float diameter_mm;
    float thermal_conductivity;  // W/(m·K)
    float resistance_per_m;      // Ohm/m

    // 温度梯度
    float t_hot_k;      // 热端温度
    float t_cold_k;     // 冷端温度
} ThermalLink;

// 计算传导热负载 (W)
float calculate_conduction_load(const ThermalLink *link) {
    float area = M_PI * powf(link->diameter_mm / 2000.0f, 2);
    float delta_t = link->t_hot_k - link->t_cold_k;

    return link->thermal_conductivity * area * delta_t / link->length_m;
}

// 计算焦耳热负载 (W)
float calculate_joule_heating(const ThermalLink *link, float current_ma) {
    float resistance = link->resistance_per_m * link->length_m;
    float current_a = current_ma / 1000.0f;

    return resistance * current_a * current_a;
}

// ============================================================================
// 信号完整性分析
// ============================================================================

// 计算信号衰减 (低温下)
float calculate_attenuation_cryo(float freq_ghz, float length_m) {
    // 低温下铜的电阻率降低，衰减减小
    // 但介质损耗仍存在

    float alpha_copper = 0.01f * sqrtf(freq_ghz);  // dB/m @ 300K
    float alpha_cryo = alpha_copper * 0.3f;         // 低温下约30%

    return alpha_cryo * length_m;
}

// 计算总抖动
float calculate_total_jitter(float rj_ps, float dj_ps, float sj_ps) {
    // RJ: 随机抖动 (高斯)
    // DJ: 确定性抖动
    // SJ: 正弦抖动

    // 总抖动 (14 sigma近似)
    return 14.0f * rj_ps + dj_ps + sj_ps;
}
