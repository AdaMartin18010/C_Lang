/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_Radiation_Hardening.md
 * Line: 407
 * Language: c
 * Block ID: 75ebad11
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 假设特定执行时间
void critical_task(void) {
    disable_interrupts();
    // 关键操作（可能被延迟）
    enable_interrupts();
}

// ✅ 使用时限检查
void critical_task_safe(void) {
    uint32_t start = get_tick_us();

    disable_interrupts();
    // 关键操作
    enable_interrupts();

    if (get_tick_us() - start > MAX_CRITICAL_TIME_US) {
        log_timing_violation();
    }
}
