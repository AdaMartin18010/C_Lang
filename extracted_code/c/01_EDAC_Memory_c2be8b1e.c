/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_EDAC_Memory.md
 * Line: 418
 * Language: c
 * Block ID: c2be8b1e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 计数器溢出导致遗漏严重问题
if (edac_stats.correctable_errors > THRESHOLD) {
    trigger_alarm();  // 溢出后永远达不到
}

// ✅ 使用饱和计数或定期重置
if (edac_stats.correctable_errors > THRESHOLD ||
    edac_stats.error_rate > RATE_THRESHOLD) {
    trigger_alarm();
}
