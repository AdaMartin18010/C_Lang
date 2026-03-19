/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Cryogenic_Superconducting\02_Subthreshold_Optimization.md
 * Line: 411
 * Language: c
 * Block ID: f9d1229e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 固定温度假设
float vt = 0.0258f;  // 300K假设

// ✅ 温度自适应
float calculate_vt(float temp_k) {
    return K_B * temp_k / Q_e;
}
// 亚阈值电流每度变化约8%
