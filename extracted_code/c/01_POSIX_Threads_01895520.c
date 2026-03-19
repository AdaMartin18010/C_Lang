/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 1890
 * Language: c
 * Block ID: 01895520
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 陷阱8：不使用while循环检查条件 */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int ready = 0;

void *consumer_wrong(void *arg) {
    pthread_mutex_lock(&mutex);

    if (!ready) {  // BUG: 应该用while！
        pthread_cond_wait(&cond, &mutex);
    }

    // 消费数据
    printf("Consuming...\n");
    ready = 0;

    pthread_mutex_unlock(&mutex);
    return NULL;
}

/* 问题分析：
 * 1. 虚假唤醒（Spurious wakeup）：wait可能无故返回
 * 2. 条件盗用：另一个消费者可能先获取了数据
 * 3. 广播唤醒：多个等待者，只有一个应该继续
 */

/* 修复方案：始终使用while循环 */
void *consumer_fixed(void *arg) {
    pthread_mutex_lock(&mutex);

    while (!ready) {  // 正确：循环检查
        pthread_cond_wait(&cond, &mutex);
    }

    printf("Consuming...\n");
    ready = 0;

    pthread_mutex_unlock(&mutex);
    return NULL;
}
