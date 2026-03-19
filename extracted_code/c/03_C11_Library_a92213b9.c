/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\03_C11_Library.md
 * Line: 262
 * Language: c
 * Block ID: a92213b9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 锁顺序不一致导致死锁
mtx_t lock_a, lock_b;

void thread1(void) {
    mtx_lock(&lock_a);
    mtx_lock(&lock_b);  // 等待thread2释放b
}

void thread2(void) {
    mtx_lock(&lock_b);
    mtx_lock(&lock_a);  // 等待thread1释放a
}

// ✅ 全局锁顺序
void safe_lock_both(void) {
    if ((uintptr_t)&lock_a < (uintptr_t)&lock_b) {
        mtx_lock(&lock_a);
        mtx_lock(&lock_b);
    } else {
        mtx_lock(&lock_b);
        mtx_lock(&lock_a);
    }
}
