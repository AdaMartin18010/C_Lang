/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 1710
 * Language: c
 * Block ID: ae57858a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 陷阱5：读者饥饿写者 */
pthread_rwlock_t rwlock;
int data = 0;

void *greedy_reader(void *arg) {
    while (1) {
        pthread_rwlock_rdlock(&rwlock);  // 读者可以共享
        // 读取数据
        printf("Reader: data=%d\n", data);
        pthread_rwlock_unlock(&rwlock);
        usleep(100);  // 短暂休息后立即回来
    }
    return NULL;
}

void *starved_writer(void *arg) {
    while (1) {
        pthread_rwlock_wrlock(&rwlock);  // 等待所有读者离开
        // 如果读者不断到达，写者永远拿不到锁！
        data++;
        printf("Writer: updated to %d\n", data);
        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }
    return NULL;
}

/* 修复方案：使用写者优先的读写锁或限制读者并发 */
typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t reader_wait;
    pthread_cond_t writer_wait;
    int readers_active;
    int writers_waiting;
    int writer_active;
} fair_rwlock_t;

void fair_write_lock(fair_rwlock_t *rw) {
    pthread_mutex_lock(&rw->lock);
    rw->writers_waiting++;
    while (rw->readers_active > 0 || rw->writer_active) {
        pthread_cond_wait(&rw->writer_wait, &rw->lock);
    }
    rw->writers_waiting--;
    rw->writer_active = 1;
    pthread_mutex_unlock(&rw->lock);
}

void fair_read_lock(fair_rwlock_t *rw) {
    pthread_mutex_lock(&rw->lock);
    // 有写者等待时，新读者阻塞
    while (rw->writer_active || rw->writers_waiting > 0) {
        pthread_cond_wait(&rw->reader_wait, &rw->lock);
    }
    rw->readers_active++;
    pthread_mutex_unlock(&rw->lock);
}
