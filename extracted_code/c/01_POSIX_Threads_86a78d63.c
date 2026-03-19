/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 1016
 * Language: c
 * Block ID: 86a78d63
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 写者优先策略：写者到达后阻塞新读者，防止写者饿死 */
typedef struct {
    pthread_mutex_t lock;
    pthread_mutex_t write_lock;
    pthread_cond_t reader_queue;   // 读者等待队列
    int reader_count;
    int writer_waiting;            // 等待的写者数量
    int writer_active;             // 活跃的写者
} write_prefer_rw_t;

void write_prefer_read_lock(write_prefer_rw_t *rw) {
    pthread_mutex_lock(&rw->lock);
    // 等待所有写者完成
    while (rw->writer_active || rw->writer_waiting > 0) {
        pthread_cond_wait(&rw->reader_queue, &rw->lock);
    }
    rw->reader_count++;
    pthread_mutex_unlock(&rw->lock);
}

void write_prefer_write_lock(write_prefer_rw_t *rw) {
    pthread_mutex_lock(&rw->lock);
    rw->writer_waiting++;
    // 等待所有读者和其他写者完成
    while (rw->reader_count > 0 || rw->writer_active) {
        pthread_mutex_unlock(&rw->lock);
        pthread_mutex_lock(&rw->write_lock);
        pthread_mutex_lock(&rw->lock);
    }
    rw->writer_waiting--;
    rw->writer_active = 1;
    pthread_mutex_unlock(&rw->lock);
}

void write_prefer_write_unlock(write_prefer_rw_t *rw) {
    pthread_mutex_lock(&rw->lock);
    rw->writer_active = 0;
    pthread_cond_broadcast(&rw->reader_queue);  // 唤醒等待的读者
    pthread_mutex_unlock(&rw->lock);
    pthread_mutex_unlock(&rw->write_lock);
}
