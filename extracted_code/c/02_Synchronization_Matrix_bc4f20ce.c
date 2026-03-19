/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\02_Synchronization_Matrix.md
 * Line: 573
 * Language: c
 * Block ID: bc4f20ce
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// semaphore_demo.c
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 10
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 3
#define ITEMS_TO_PRODUCE 100

// 有界缓冲区
typedef struct {
    int buffer[BUFFER_SIZE];
    int in;
    int out;
} bounded_buffer_t;

bounded_buffer_t bb = {.in = 0, .out = 0};

// 信号量
sem_t empty;   // 空槽位计数
sem_t full;    // 已用槽位计数
sem_t mutex;   // 互斥访问缓冲区

// 计数信号量示例 - 资源池
#define MAX_CONNECTIONS 5
sem_t connection_pool;
int connections[MAX_CONNECTIONS];
int next_connection = 0;

int acquire_connection(void) {
    sem_wait(&connection_pool);  // P操作
    int conn = connections[next_connection++ % MAX_CONNECTIONS];
    printf("Acquired connection %d\n", conn);
    return conn;
}

void release_connection(int conn) {
    printf("Releasing connection %d\n", conn);
    sem_post(&connection_pool);  // V操作
}

void *producer(void *arg) {
    int id = *(int*)arg;

    for (int i = 0; i < ITEMS_TO_PRODUCE / NUM_PRODUCERS; i++) {
        int item = id * 1000 + i;

        sem_wait(&empty);   // 等待空槽位
        sem_wait(&mutex);   // 进入临界区

        // 生产数据
        bb.buffer[bb.in] = item;
        bb.in = (bb.in + 1) % BUFFER_SIZE;
        printf("Producer %d: produced %d\n", id, item);

        sem_post(&mutex);   // 离开临界区
        sem_post(&full);    // 增加已用槽位

        usleep(rand() % 10000);
    }

    return NULL;
}

void *consumer(void *arg) {
    int id = *(int*)arg;

    for (int i = 0; i < ITEMS_TO_PRODUCE / NUM_CONSUMERS; i++) {
        sem_wait(&full);    // 等待数据
        sem_wait(&mutex);   // 进入临界区

        // 消费数据
        int item = bb.buffer[bb.out];
        bb.out = (bb.out + 1) % BUFFER_SIZE;
        printf("Consumer %d: consumed %d\n", id, item);

        sem_post(&mutex);   // 离开临界区
        sem_post(&empty);   // 增加空槽位

        usleep(rand() % 15000);
    }

    return NULL;
}

// 二元信号量 (类似互斥锁)
sem_t binary_sem;
int shared_counter = 0;

void *binary_sem_worker(void *arg) {
    (void)arg;

    for (int i = 0; i < 100000; i++) {
        sem_wait(&binary_sem);  // P操作，类似lock
        shared_counter++;
        sem_post(&binary_sem);  // V操作，类似unlock
    }

    return NULL;
}

int main(void) {
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];
    int producer_ids[NUM_PRODUCERS];
    int consumer_ids[NUM_CONSUMERS];

    printf("=== 信号量演示 ===\n");

    // 初始化信号量
    sem_init(&empty, 0, BUFFER_SIZE);  // 初始空槽位=BUFFER_SIZE
    sem_init(&full, 0, 0);              // 初始已用槽位=0
    sem_init(&mutex, 0, 1);             // 二元信号量

    // 初始化连接池信号量
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        connections[i] = i + 1;
    }
    sem_init(&connection_pool, 0, MAX_CONNECTIONS);

    printf("\n1. 生产者-消费者模型\n");

    // 创建生产者
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producer_ids[i] = i;
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }

    // 创建消费者
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        consumer_ids[i] = i;
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }

    // 等待完成
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    printf("\n2. 连接池示例\n");
    int c1 = acquire_connection();
    int c2 = acquire_connection();
    release_connection(c1);
    release_connection(c2);

    // 清理
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);
    sem_destroy(&connection_pool);

    return 0;
}
