/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\06_Lab6_Multithreading.md
 * Line: 568
 * Language: c
 * Block ID: c788f4e0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 添加线程调试信息
#define DEBUG_THREAD 1

#if DEBUG_THREAD
#define thread_debug(fmt, ...) printf("[T%d] " fmt "\n", \
    current_thread - threads, ##__VA_ARGS__)
#else
#define thread_debug(fmt, ...)
#endif

// 使用示例
void thread_function() {
    thread_debug("starting");
    // ...
    thread_debug("yielding");
    thread_yield();
    thread_debug("resumed");
}
