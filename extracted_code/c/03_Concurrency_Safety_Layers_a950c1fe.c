/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\03_Concurrency_Safety_Layers.md
 * Line: 212
 * Language: c
 * Block ID: a950c1fe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 读者优先 vs 写者优先
typedef struct {
    atomic_int readers;
    atomic_int writers_waiting;
    mtx_t write_lock;
} RWLock;

void rwlock_read_lock(RWLock *rw) {
    while (atomic_load(&rw->writers_waiting) > 0) {
        // 写者优先：等待写者完成
        thrd_yield();
    }
    atomic_fetch_add(&rw->readers, 1);
}

void rwlock_write_lock(RWLock *rw) {
    atomic_fetch_add(&rw->writers_waiting, 1);
    mtx_lock(&rw->write_lock);
    while (atomic_load(&rw->readers) > 0) {
        // 等待所有读者退出
    }
    atomic_fetch_sub(&rw->writers_waiting, 1);
}
