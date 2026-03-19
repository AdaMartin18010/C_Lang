/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Cryogenic_Superconducting\01_Cryo_Serial_Interface.md
 * Line: 329
 * Language: c
 * Block ID: d152cebb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 使用室温参数设计
#define COPPER_RESISTIVITY  1.7e-8  // @ 300K

// ✅ 低温电阻率低10-100倍
#define COPPER_RESISTIVITY_CRYO  2.0e-10  // @ 4K
// 会影响阻抗匹配和衰减计算
