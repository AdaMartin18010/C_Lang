/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\03_C11_Library.md
 * Line: 235
 * Language: c
 * Block ID: 0902304f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 数据竞争
int shared = 0;

void thread_func(void) {
    shared++;  // 未定义行为！
}

// ✅ 使用原子操作
_Atomic int safe_shared = 0;
void safe_thread_func(void) {
    atomic_fetch_add(&safe_shared, 1);
}

// ✅ 或使用互斥锁
static mtx_t mutex;
static int mutex_shared = 0;

void mutex_thread_func(void) {
    mtx_lock(&mutex);
    mutex_shared++;
    mtx_unlock(&mutex);
}
