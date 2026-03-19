/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\02_Synchronization_Matrix.md
 * Line: 239
 * Language: c
 * Block ID: 99621348
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// rwlock_demo.c
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_READERS 8
#define NUM_WRITERS 2
#define OPERATIONS 100000

typedef struct {
    pthread_rwlock_t rwlock;
    int data;
    int read_count;
    int write_count;
} database_t;

database_t db = {
    .rwlock = PTHREAD_RWLOCK_INITIALIZER,
    .data = 0,
    .read_count = 0,
    .write_count = 0
};

// 读者线程
void *reader_thread(void *arg) {
    int id = *(int*)arg;
    long local_reads = 0;

    for (int i = 0; i < OPERATIONS; i++) {
        // 获取读锁 (多个读者可同时持有)
        pthread_rwlock_rdlock(&db.rwlock);

        // 读取数据
        int value = db.data;
        local_reads++;
        (void)value;  // 使用数据，避免优化

        // 模拟读取处理
        if (i % 10000 == 0) {
            usleep(1);  // 偶尔休眠
        }

        pthread_rwlock_unlock(&db.rwlock);
    }

    printf("Reader %d completed %ld reads\n", id, local_reads);
    return NULL;
}

// 写者线程
void *writer_thread(void *arg) {
    int id = *(int*)arg;
    long local_writes = 0;

    for (int i = 0; i < OPERATIONS / 10; i++) {  // 写操作较少
        // 获取写锁 (独占)
        pthread_rwlock_wrlock(&db.rwlock);

        // 修改数据
        db.data = id * 1000 + i;
        db.write_count++;
        local_writes++;

        // 模拟写入处理
        usleep(10);

        pthread_rwlock_unlock(&db.rwlock);

        // 写之间休息
        usleep(100);
    }

    printf("Writer %d completed %ld writes\n", id, local_writes);
    return NULL;
}

// 尝试获取读写锁
void *try_rwlock_thread(void *arg) {
    (void)arg;
    int rd_success = 0, wr_success = 0;

    for (int i = 0; i < 1000; i++) {
        // 尝试获取读锁
        if (pthread_rwlock_tryrdlock(&db.rwlock) == 0) {
            rd_success++;
            pthread_rwlock_unlock(&db.rwlock);
        }

        // 尝试获取写锁
        if (pthread_rwlock_trywrlock(&db.rwlock) == 0) {
            wr_success++;
            pthread_rwlock_unlock(&db.rwlock);
        }
    }

    printf("Trylock: read=%d, write=%d\n", rd_success, wr_success);
    return NULL;
}

// 偏好设置
void demonstrate_preference(void) {
    pthread_rwlockattr_t attr;
    pthread_rwlock_t pref_rwlock;

    pthread_rwlockattr_init(&attr);

    // 设置读者优先 (默认)
    // pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_READER_NP);

    // 设置写者优先
    #ifdef __linux__
    pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NP);
    #endif

    pthread_rwlock_init(&pref_rwlock, &attr);

    printf("\n读写锁偏好设置完成\n");

    pthread_rwlock_destroy(&pref_rwlock);
    pthread_rwlockattr_destroy(&attr);
}

int main(void) {
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];
    int reader_ids[NUM_READERS];
    int writer_ids[NUM_WRITERS];

    printf("=== 读写锁演示 ===\n");
    printf("Readers: %d, Writers: %d\n", NUM_READERS, NUM_WRITERS);

    // 创建读者线程
    for (int i = 0; i < NUM_READERS; i++) {
        reader_ids[i] = i;
        pthread_create(&readers[i], NULL, reader_thread, &reader_ids[i]);
    }

    // 创建写者线程
    for (int i = 0; i < NUM_WRITERS; i++) {
        writer_ids[i] = i;
        pthread_create(&writers[i], NULL, writer_thread, &writer_ids[i]);
    }

    // 等待所有线程完成
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    printf("\nFinal data: %d, Write count: %d\n", db.data, db.write_count);

    demonstrate_preference();

    pthread_rwlock_destroy(&db.rwlock);
    return 0;
}
