/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Cryogenic_Superconducting\README.md
 * Line: 270
 * Language: c
 * Block ID: ffb2a4a5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 低温 SRAM 单元
typedef struct {
    CryoMOSFET access_transistors[2];
    CryoMOSFET pull_down[2];
    CryoMOSFET pull_up[2];
} CryoSRAM_Cell;

// 读取裕度计算
float calculate_read_margin(CryoSRAM_Cell *cell) {
    float vth = calculate_threshold_voltage(&cell->pull_down[0]);
    float vdd = 0.5f;  // 低温下可降低供电电压

    // 读干扰裕度
    float read_margin = vdd - 2 * vth;
    return read_margin;
}

// 保持裕度计算
float calculate_hold_margin(CryoSRAM_Cell *cell) {
    // 低温下漏电减小，保持特性改善
    float leakage_reduction = powf(4.2f / 300.0f, 1.5f);
    return 100.0f * leakage_reduction;  // 归一化保持时间
}

// 低温优化电路库单元
void optimize_standard_cells_for_cryo(void) {
    // 减小晶体管尺寸（迁移率提高）
    float mobility_boost = calculate_mobility(&(CryoMOSFET){.temperature=4.2f}) /
                           calculate_mobility(&(CryoMOSFET){.temperature=300.0f});

    // 可降低供电电压
    float vdd_300k = 1.0f;
    float vdd_4k = 0.3f;  // 可降至 0.3V

    printf("Cryo optimization:\n");
    printf("  Mobility boost: %.1fx\n", mobility_boost);
    printf("  VDD reduction: %.1fV -> %.1fV\n", vdd_300k, vdd_4k);
    printf("  Power reduction: %.2fx\n",
           powf(vdd_4k/vdd_300k, 2) / mobility_boost);
}
