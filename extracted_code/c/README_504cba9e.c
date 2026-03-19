/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Cryogenic_Superconducting\README.md
 * Line: 509
 * Language: c
 * Block ID: 504cba9e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 热负载类型
typedef struct {
    float conductive_load;      // 传导热负载
    float radiative_load;       // 辐射热负载
    float electrical_load;      // 焦耳热
    float measurement_load;     // 测量热负载
} ThermalLoad;

// 计算传导热负载
float calculate_conductive_load(float area, float length,
                                float k_thermal, float delta_t) {
    // Q = k * A * ΔT / L
    return k_thermal * area * delta_t / length;
}

// 计算辐射热负载
float calculate_radiative_load(float area, float emissivity1,
                               float emissivity2, float t_hot,
                               float t_cold) {
    float sigma = 5.67e-8;  // Stefan-Boltzmann 常数
    float e_eff = 1.0f / (1.0f/emissivity1 + 1.0f/emissivity2 - 1.0f);

    return e_eff * sigma * area * (powf(t_hot, 4) - powf(t_cold, 4));
}

// 总热负载预算
void calculate_total_heat_load(ThermalLoad *loads, int num_stages) {
    printf("Heat Load Budget:\n");
    printf("=================\n");

    for (int i = 0; i < num_stages; i++) {
        float total = loads[i].conductive_load +
                      loads[i].radiative_load +
                      loads[i].electrical_load +
                      loads[i].measurement_load;

        printf("Stage %d:\n", i);
        printf("  Conductive: %.2f mW\n", loads[i].conductive_load * 1000);
        printf("  Radiative:  %.2f mW\n", loads[i].radiative_load * 1000);
        printf("  Electrical: %.2f mW\n", loads[i].electrical_load * 1000);
        printf("  Total:      %.2f mW\n", total * 1000);
    }
}
