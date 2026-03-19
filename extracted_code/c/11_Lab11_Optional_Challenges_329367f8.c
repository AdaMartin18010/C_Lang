/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\11_Lab11_Optional_Challenges.md
 * Line: 34
 * Language: c
 * Block ID: 329367f8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 当前xv6: 单线程内核
// 改进: 多线程内核

struct kernel_thread {
    struct context context;
    void (*func)(void*);
    void *arg;
    enum { KT_READY, KT_RUNNING, KT_SLEEPING } state;
};

// 线程调度器
void kernel_thread_scheduler() {
    while (1) {
        for (each kernel thread) {
            if (thread->state == KT_READY) {
                switch_to(thread);
            }
        }
    }
}
