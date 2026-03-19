/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\02_Hard_RealTime.md
 * Line: 793
 * Language: c
 * Block ID: bd6caba8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 直接OS延迟可能不准确
void task(void) {
    while (1) {
        do_work();
        os_delay(10);  // 实际间隔可能>10ms
    }
}

// ✅ 使用绝对时间调度
void deterministic_task(void) {
    uint32_t next_wakeup = get_tick();
    while (1) {
        do_work();
        next_wakeup += 10;  // 绝对时间
        sleep_until(next_wakeup);  // 等待到指定时间点
    }
}
