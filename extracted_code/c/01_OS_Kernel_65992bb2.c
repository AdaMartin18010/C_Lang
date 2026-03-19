/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\01_OS_Kernel.md
 * Line: 458
 * Language: c
 * Block ID: 65992bb2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 无保护的共享数据
int counter = 0;

void unsafe_inc(void) {
    counter++;  // 非原子操作，可能丢失更新
}

// ✅ 原子操作
atomic_t safe_counter = ATOMIC_INIT(0);
void safe_inc(void) {
    atomic_inc(&safe_counter);
}

// ✅ 或自旋锁保护
static DEFINE_SPINLOCK(counter_lock);
static int locked_counter = 0;

void locked_inc(void) {
    spin_lock(&counter_lock);
    locked_counter++;
    spin_unlock(&counter_lock);
}
