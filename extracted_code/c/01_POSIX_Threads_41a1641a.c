/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 218
 * Language: c
 * Block ID: 41a1641a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 竞态条件示例：非确定性结果
int counter = 0;

void* increment(void* arg) {
    for (int i = 0; i < 100000; i++) {
        counter++;  // 非原子操作: LOAD → ADD → STORE
    }
    return NULL;
}
// 两个线程执行后，counter 值不确定，在 [100000, 200000] 范围内
