/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\13_Self_Modifying_Code\02_JIT_Physical_Constraints.md
 * Line: 748
 * Language: c
 * Block ID: dfb5cf86
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：持续高负载编译
void aggressive_compilation(void) {
    while (has_uncompiled_functions()) {
        compile_next_function(OPTIMIZED_TIER);  // ❌ 可能导致过热
    }
}

// 正确：考虑热约束
void thermal_aware_compilation(SystemConstraints *sys) {
    while (has_uncompiled_functions()) {
        if (sys->temperature > THERMAL_THRESHOLD) {
            // 暂停编译，等待冷却
            sleep(thermal_backoff_time(sys->temperature));
        }

        // 根据温度选择编译层级
        CompilationTier tier = (sys->temperature > WARM_THRESHOLD)
                               ? TIER_BASELINE
                               : TIER_OPTIMIZED;

        compile_next_function(tier);
    }
}
