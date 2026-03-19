/*
 * Auto-generated from: 10_WebAssembly_C\02_Emscripten_Advanced_Usage.md
 * Line: 2506
 * Language: c
 * Block ID: e57e98af
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ========== 多线程计算示例 ==========

#include <pthread.h>
#include <emscripten/emscripten.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_THREADS 4

// 线程参数结构
typedef struct {
    int thread_id;
    int start;
    int end;
    float* input;
    float* output;
} ThreadData;

// 线程函数：并行数组处理
void* process_chunk(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    for (int i = data->start; i < data->end; i++) {
        // 计算密集型操作
        data->output[i] = sqrt(data->input[i] * data->input[i] +
                               data->input[i] * 0.5f);
    }

    return NULL;
}

// 并行处理数组
EMSCRIPTEN_KEEPALIVE
void parallel_process(float* input, float* output, int size) {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    int chunk_size = size / NUM_THREADS;

    // 创建线程
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == NUM_THREADS - 1) ? size : (i + 1) * chunk_size;
        thread_data[i].input = input;
        thread_data[i].output = output;

        pthread_create(&threads[i], NULL, process_chunk, &thread_data[i]);
    }

    // 等待所有线程完成
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
}

// ========== 线程池任务处理 ==========

#define TASK_QUEUE_SIZE 100

typedef struct {
    void (*function)(void*);
    void* argument;
} Task;

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    Task tasks[TASK_QUEUE_SIZE];
    int head;
    int tail;
    int count;
    int shutdown;
} TaskQueue;

static TaskQueue queue;
static pthread_t* workers = NULL;
static int num_workers = 0;

// Worker 线程函数
void* worker_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&queue.mutex);

        while (queue.count == 0 && !queue.shutdown) {
            pthread_cond_wait(&queue.cond, &queue.mutex);
        }

        if (queue.shutdown) {
            pthread_mutex_unlock(&queue.mutex);
            break;
        }

        Task task = queue.tasks[queue.head];
        queue.head = (queue.head + 1) % TASK_QUEUE_SIZE;
        queue.count--;

        pthread_mutex_unlock(&queue.mutex);

        // 执行任务
        task.function(task.argument);
    }

    return NULL;
}

// 初始化线程池
EMSCRIPTEN_KEEPALIVE
int thread_pool_init(int num_threads) {
    pthread_mutex_init(&queue.mutex, NULL);
    pthread_cond_init(&queue.cond, NULL);
    queue.head = 0;
    queue.tail = 0;
    queue.count = 0;
    queue.shutdown = 0;

    workers = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    num_workers = num_threads;

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&workers[i], NULL, worker_thread, NULL);
    }

    return 0;
}

// 提交任务
EMSCRIPTEN_KEEPALIVE
int thread_pool_submit(void (*function)(void*), void* argument) {
    pthread_mutex_lock(&queue.mutex);

    if (queue.count >= TASK_QUEUE_SIZE) {
        pthread_mutex_unlock(&queue.mutex);
        return -1;  // 队列满
    }

    queue.tasks[queue.tail].function = function;
    queue.tasks[queue.tail].argument = argument;
    queue.tail = (queue.tail + 1) % TASK_QUEUE_SIZE;
    queue.count++;

    pthread_cond_signal(&queue.cond);
    pthread_mutex_unlock(&queue.mutex);

    return 0;
}

// 关闭线程池
EMSCRIPTEN_KEEPALIVE
void thread_pool_shutdown() {
    pthread_mutex_lock(&queue.mutex);
    queue.shutdown = 1;
    pthread_cond_broadcast(&queue.cond);
    pthread_mutex_unlock(&queue.mutex);

    for (int i = 0; i < num_workers; i++) {
        pthread_join(workers[i], NULL);
    }

    free(workers);
    workers = NULL;
    num_workers = 0;

    pthread_mutex_destroy(&queue.mutex);
    pthread_cond_destroy(&queue.cond);
}

// ========== 同步原语示例 ==========

// 互斥锁示例
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
static int shared_counter = 0;

EMSCRIPTEN_KEEPALIVE
void increment_counter() {
    pthread_mutex_lock(&counter_mutex);
    shared_counter++;
    pthread_mutex_unlock(&counter_mutex);
}

EMSCRIPTEN_KEEPALIVE
int get_counter() {
    int value;
    pthread_mutex_lock(&counter_mutex);
    value = shared_counter;
    pthread_mutex_unlock(&counter_mutex);
    return value;
}

// 条件变量示例
static pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;
static int condition_met = 0;

EMSCRIPTEN_KEEPALIVE
void wait_for_condition() {
    pthread_mutex_lock(&cond_mutex);
    while (!condition_met) {
        pthread_cond_wait(&cond_var, &cond_mutex);
    }
    pthread_mutex_unlock(&cond_mutex);
}

EMSCRIPTEN_KEEPALIVE
void signal_condition() {
    pthread_mutex_lock(&cond_mutex);
    condition_met = 1;
    pthread_cond_broadcast(&cond_var);
    pthread_mutex_unlock(&cond_mutex);
}
