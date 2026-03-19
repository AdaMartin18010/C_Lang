/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 1640
 * Language: c
 * Block ID: 8e20ceaa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 陷阱4：互相礼让导致活锁 */
typedef struct {
    pthread_mutex_t lock;
    int priority;
} resource_t;

resource_t res1 = { .priority = 1 };
resource_t res2 = { .priority = 2 };

void *polite_thread_a(void *arg) {
    while (1) {
        pthread_mutex_lock(&res1.lock);
        if (pthread_mutex_trylock(&res2.lock) == 0) {
            // 获取成功
            printf("A: Got both resources\n");
            pthread_mutex_unlock(&res2.lock);
            pthread_mutex_unlock(&res1.lock);
            break;
        }
        // 礼貌地释放并重试
        pthread_mutex_unlock(&res1.lock);
        printf("A: Yielding...\n");
        usleep(1000);
    }
    return NULL;
}

void *polite_thread_b(void *arg) {
    while (1) {
        pthread_mutex_lock(&res2.lock);
        if (pthread_mutex_trylock(&res1.lock) == 0) {
            printf("B: Got both resources\n");
            pthread_mutex_unlock(&res1.lock);
            pthread_mutex_unlock(&res2.lock);
            break;
        }
        pthread_mutex_unlock(&res2.lock);
        printf("B: Yielding...\n");
        usleep(1000);
    }
    return NULL;
}

/* 问题：两个线程同时检测到冲突，同时释放，同时重试，无限循环 */

/* 修复方案：退避策略 + 随机抖动 */
void *fixed_thread_a(void *arg) {
    int attempts = 0;
    while (1) {
        pthread_mutex_lock(&res1.lock);
        if (pthread_mutex_trylock(&res2.lock) == 0) {
            printf("A: Got both resources\n");
            pthread_mutex_unlock(&res2.lock);
            pthread_mutex_unlock(&res1.lock);
            break;
        }
        pthread_mutex_unlock(&res1.lock);

        // 指数退避 + 随机抖动
        unsigned int backoff = (1 << attempts) * 1000 + (rand() % 1000);
        usleep(backoff > 100000 ? 100000 : backoff);
        if (attempts < 10) attempts++;
    }
    return NULL;
}
