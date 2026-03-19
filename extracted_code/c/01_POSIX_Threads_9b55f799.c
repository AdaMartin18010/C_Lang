/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 1062
 * Language: c
 * Block ID: 9b55f799
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 使用标准POSIX读写锁（通常实现公平策略） */
#include <pthread.h>

typedef pthread_rwlock_t fair_rw_t;

void fair_rw_init(fair_rw_t *rw) {
    pthread_rwlockattr_t attr;
    pthread_rwlockattr_init(&attr);
    // 设置写者优先（如果支持）
    #ifdef PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP
    pthread_rwlockattr_setkind_np(&attr,
        PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
    #endif
    pthread_rwlock_init(rw, &attr);
    pthread_rwlockattr_destroy(&attr);
}

void fair_read_lock(fair_rw_t *rw) {
    pthread_rwlock_rdlock(rw);
}

void fair_write_lock(fair_rw_t *rw) {
    pthread_rwlock_wrlock(rw);
}

void fair_unlock(fair_rw_t *rw) {
    pthread_rwlock_unlock(rw);
}
