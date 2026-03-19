/*
 * Auto-generated from: 04_Industrial_Scenarios\10_Deep_Sea\README.md
 * Line: 265
 * Language: c
 * Block ID: 8a398b91
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 电子舱热设计
typedef struct {
    float ambient_temp;      // 环境温度 (K)
    float max_component_temp; // 元件最高温度
    float power_dissipation;  // 功耗 (W)
    float thermal_resistance_case_ambient;
    float thermal_resistance_junction_case;
} ThermalDesign;

// 计算所需散热能力
float calculate_required_thermal_resistance(ThermalDesign *design) {
    float delta_t = design->max_component_temp - design->ambient_temp;
    return delta_t / design->power_dissipation;
}

// 油补偿设计
#define COMPENSATION_OIL "硅油"
#define OIL_COMPRESSIBILITY 1.0e-9  // Pa^-1

typedef struct {
    float internal_volume;       // 内部容积
    float piston_area;           // 补偿活塞面积
    float max_piston_travel;     // 最大行程
    float spring_constant;       // 弹簧刚度
} OilCompensator;

// 计算活塞位移
float calculate_piston_displacement(OilCompensator *comp,
                                     float pressure_delta) {
    // 油液体积压缩
    float volume_change = comp->internal_volume *
                          OIL_COMPRESSIBILITY * pressure_delta;

    // 弹簧压缩
    float spring_compression = pressure_delta * comp->piston_area /
                               comp->spring_constant;

    // 总位移
    return volume_change / comp->piston_area + spring_compression;
}
