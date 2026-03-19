/*
 * Auto-generated from: 06_Thinking_Representation\07_Knowledge_Graph\05_Concurrency_Knowledge_Graph.md
 * Line: 381
 * Language: c
 * Block ID: 2ccdedaa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

mtx_t mutex;
cnd_t cond;
_Atomic(int) ready = 0;

// 等待线程
void waiter() {
    mtx_lock(&mutex);
    while (atomic_load(&ready) == 0) {  // 必须用 while 防止虚假唤醒
        cnd_wait(&cond, &mutex);  // 自动释放 mutex，等待 signal
    }
    // 条件满足，继续执行
    mtx_unlock(&mutex);
}

// 通知线程
void notifier() {
    atomic_store(&ready, 1);
    mtx_lock(&mutex);
    cnd_signal(&cond);  // 唤醒等待线程
    mtx_unlock(&mutex);
}
