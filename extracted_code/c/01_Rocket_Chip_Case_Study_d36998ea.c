/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\01_Rocket_Chip_Case_Study.md
 * Line: 1003
 * Language: c
 * Block ID: d36998ea
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：频繁的自旋锁
void critical_section(void) {
    while (__sync_lock_test_and_set(&lock, 1));  // 总线风暴
    // ... 临界区
    __sync_lock_release(&lock);
}

// 优化：使用暂停指令+指数退避
void critical_section_optimized(void) {
    int backoff = 1;
    while (__sync_lock_test_and_set(&lock, 1)) {
        for (int i = 0; i < backoff; i++) {
            asm volatile("pause");  // 降低功耗
        }
        backoff = min(backoff * 2, 1024);
    }
    // ... 临界区
    __sync_lock_release(&lock);
}
