/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\01_C11_Features.md
 * Line: 350
 * Language: c
 * Block ID: b95e2721
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 数据竞争：非原子变量多线程访问
int shared = 0;
void thread_func(void) {
    shared++;  // 未定义行为！
}

// ✅ 使用原子操作
_Atomic int safe_shared = 0;
void safe_thread_func(void) {
    atomic_fetch_add(&safe_shared, 1);
}
