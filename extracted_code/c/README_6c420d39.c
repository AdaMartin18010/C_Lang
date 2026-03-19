/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Cryogenic_Superconducting\README.md
 * Line: 227
 * Language: c
 * Block ID: 6c420d39
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 低温 MOSFET 参数模型
typedef struct {
    float width;
    float length;
    float oxide_thickness;
    float threshold_voltage_300k;
    float mobility_300k;
    float temperature;
} CryoMOSFET;

// 阈值电压温度特性
// Vth(T) = Vth(300K) + α * (T - 300)
// α 约为 1-2 mV/K（对于体硅 CMOS）
float calculate_threshold_voltage(CryoMOSFET *mosfet) {
    float alpha = 0.0015f;  // V/K
    return mosfet->threshold_voltage_300k +
           alpha * (mosfet->temperature - 300.0f);
}

// 迁移率温度特性
// μ(T) = μ(300K) * (T/300)^(-2.5)
float calculate_mobility(CryoMOSFET *mosfet) {
    return mosfet->mobility_300k *
           powf(mosfet->temperature / 300.0f, -2.5f);
}

// 亚阈值摆幅
// S = ln(10) * kT/q * (1 + Cd/Cox)
// 低温下亚阈值摆幅显著改善
float calculate_subthreshold_swing(CryoMOSFET *mosfet) {
    float k = 1.38e-23;  // 玻尔兹曼常数
    float q = 1.602e-19; // 元电荷
    float Cox = EPSILON_SIO2 * mosfet->width * mosfet->length /
                mosfet->oxide_thickness;

    return logf(10) * k * mosfet->temperature / q *
           (1 + 0.1f);  // 简化体效应因子
}
