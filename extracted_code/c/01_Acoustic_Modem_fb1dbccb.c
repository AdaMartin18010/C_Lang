/*
 * Auto-generated from: 04_Industrial_Scenarios\10_Deep_Sea\01_Acoustic_Modem.md
 * Line: 442
 * Language: c
 * Block ID: fb1dbccb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 整数倍采样点重采样精度不够
// 残余多普勒导致ICI (载波间干扰)

// ✅ 使用分数间隔重采样 + 残余频偏补偿
// 或使用多普勒不敏感波形
