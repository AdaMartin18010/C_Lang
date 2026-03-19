/*
 * Auto-generated from: 04_Industrial_Scenarios\10_Deep_Sea\02_Energy_Aware_Scheduling.md
 * Line: 431
 * Language: c
 * Block ID: 2a4e63c2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 只考虑运行时功耗
if (energy > task_power * duration)

// ✅ 包括启动和关闭能耗
if (energy > startup_energy + task_power * duration + shutdown_energy)
