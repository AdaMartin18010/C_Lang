/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 973
 * Language: c
 * Block ID: 00e9f73e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 读者优先策略：读者可以插队，写者可能饿死 */
typedef struct {
    pthread_mutex_t lock;
    pthread_mutex_t write_lock;  // 写者互斥
    int reader_count;
} read_prefer_rw_t;

void read_prefer_init(read_prefer_rw_t *rw) {
    pthread_mutex_init(&rw->lock, NULL);
    pthread_mutex_init(&rw->write_lock, NULL);
    rw->reader_count = 0;
}

void read_prefer_read_lock(read_prefer_rw_t *rw) {
    pthread_mutex_lock(&rw->lock);
    rw->reader_count++;
    if (rw->reader_count == 1) {
        pthread_mutex_lock(&rw->write_lock);  // 第一个读者阻止写者
    }
    pthread_mutex_unlock(&rw->lock);
}

void read_prefer_read_unlock(read_prefer_rw_t *rw) {
    pthread_mutex_lock(&rw->lock);
    rw->reader_count--;
    if (rw->reader_count == 0) {
        pthread_mutex_unlock(&rw->write_lock);  // 最后一个读者允许写者
    }
    pthread_mutex_unlock(&rw->lock);
}

void read_prefer_write_lock(read_prefer_rw_t *rw) {
    pthread_mutex_lock(&rw->write_lock);  // 独占访问
}

void read_prefer_write_unlock(read_prefer_rw_t *rw) {
    pthread_mutex_unlock(&rw->write_lock);
}
