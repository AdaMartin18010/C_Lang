/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_TLA_Plus\05_C_Verification_with_TLA.md
 * Line: 832
 * Language: c
 * Block ID: 9c5ca01e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C实现: 有界缓冲区 */
/* bounded_buffer.c */

#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 10

typedef struct {
    int buffer[BUFFER_SIZE];
    int count;
    int head;
    int tail;
    pthread_mutex_t mutex;
    sem_t not_full;
    sem_t not_empty;
} BoundedBuffer;

void buffer_init(BoundedBuffer* b) {
    b->count = 0;
    b->head = 0;
    b->tail = 0;
    pthread_mutex_init(&b->mutex, NULL);
    sem_init(&b->not_full, 0, BUFFER_SIZE);
    sem_init(&b->not_empty, 0, 0);
}

void buffer_put(BoundedBuffer* b, int item) {
    sem_wait(&b->not_full);          // P(not_full)
    pthread_mutex_lock(&b->mutex);    // P(mutex)

    // 断言: 缓冲区未满
    assert(b->count < BUFFER_SIZE);

    b->buffer[b->tail] = item;
    b->tail = (b->tail + 1) % BUFFER_SIZE;
    b->count++;

    pthread_mutex_unlock(&b->mutex);  // V(mutex)
    sem_post(&b->not_empty);          // V(not_empty)
}

int buffer_get(BoundedBuffer* b) {
    sem_wait(&b->not_empty);          // P(not_empty)
    pthread_mutex_lock(&b->mutex);    // P(mutex)

    // 断言: 缓冲区非空
    assert(b->count > 0);

    int item = b->buffer[b->head];
    b->head = (b->head + 1) % BUFFER_SIZE;
    b->count--;

    pthread_mutex_unlock(&b->mutex);  // V(mutex)
    sem_post(&b->not_full);           // V(not_full)

    return item;
}
