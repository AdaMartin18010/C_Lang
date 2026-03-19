/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\01_C11_Features.md
 * Line: 366
 * Language: c
 * Block ID: c8bb266a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 死锁：锁顺序不一致
mtx_t lock_a, lock_b;

void thread1(void) {
    mtx_lock(&lock_a);
    mtx_lock(&lock_b);  // 等待thread2释放b
    // ...
}

void thread2(void) {
    mtx_lock(&lock_b);
    mtx_lock(&lock_a);  // 等待thread1释放a
    // ... 死锁！
}

// ✅ 解决方案：全局锁顺序
void safe_lock_both(mtx_t *first, mtx_t *second) {
    if (first < second) {  // 地址比较确定顺序
        mtx_lock(first);
        mtx_lock(second);
    } else {
        mtx_lock(second);
        mtx_lock(first);
    }
}
