/*
 * Auto-generated from: 12_Practice_Exercises\08_Concurrency_Parallelism_Labs.md
 * Line: 75
 * Language: c
 * Block ID: b144b83a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t read_cond;
    pthread_cond_t write_cond;
    int readers;
    int writers_waiting;
    bool writing;
    // 配置
    bool prefer_readers;  // true=读优先, false=写优先
} RWLock;

void rwlock_init(RWLock *lock, bool prefer_readers);
void rwlock_destroy(RWLock *lock);

void rwlock_read_lock(RWLock *lock);
bool rwlock_read_trylock(RWLock *lock);
void rwlock_read_unlock(RWLock *lock);

void rwlock_write_lock(RWLock *lock);
bool rwlock_write_trylock(RWLock *lock);
void rwlock_write_unlock(RWLock *lock);
