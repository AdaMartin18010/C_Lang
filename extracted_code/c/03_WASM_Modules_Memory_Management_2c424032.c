/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 2075
 * Language: c
 * Block ID: 2c424032
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// shared_memory.c
#include <emscripten/emscripten.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

// 共享内存中的数据结构
#define QUEUE_SIZE 1024

typedef struct {
    volatile int head;
    volatile int tail;
    volatile int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
    float data[QUEUE_SIZE];
} SharedQueue;

// 在共享内存中分配
SharedQueue* queue;

void init_shared_queue() {
    queue = (SharedQueue*)malloc(sizeof(SharedQueue));
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->not_full, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
}

void enqueue(float value) {
    pthread_mutex_lock(&queue->mutex);

    while (queue->count >= QUEUE_SIZE) {
        pthread_cond_wait(&queue->not_full, &queue->mutex);
    }

    queue->data[queue->tail] = value;
    queue->tail = (queue->tail + 1) % QUEUE_SIZE;
    queue->count++;

    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
}

float dequeue() {
    pthread_mutex_lock(&queue->mutex);

    while (queue->count == 0) {
        pthread_cond_wait(&queue->not_empty, &queue->mutex);
    }

    float value = queue->data[queue->head];
    queue->head = (queue->head + 1) % QUEUE_SIZE;
    queue->count--;

    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->mutex);

    return value;
}
