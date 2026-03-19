/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Cryogenic_Superconducting\02_Subthreshold_Optimization.md
 * Line: 424
 * Language: c
 * Block ID: 19e7481a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 电压过低导致功能失效
if (energy < min_energy) {
    vdd -= 0.01f;  // 持续降压
}
// 可能降到噪声无法容忍的水平

// ✅ 设置硬下限并检查功能
#define VDD_MIN_FUNCTIONAL  0.18f
#define VDD_MIN_NOISE       (3.0f * vnoise_rms)

float vdd_floor = fmaxf(VDD_MIN_FUNCTIONAL, VDD_MIN_NOISE);
if (vdd > vdd_floor && energy < min_energy) {
    vdd -= 0.01f;
}
