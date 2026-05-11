/*
 * 生产者-消费者（条件变量）示例
 * 编译: gcc -Wall -Wextra -std=c11 -o producer_consumer producer_consumer.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#define BUFFER_SIZE 8

static int buffer[BUFFER_SIZE];
static size_t head = 0, tail = 0, count = 0;
static mtx_t lock;
static cnd_t not_full, not_empty;

void produce(int value) {
    mtx_lock(&lock);
    while (count == BUFFER_SIZE) {
        cnd_wait(&not_full, &lock);
    }
    buffer[head] = value;
    head = (head + 1) % BUFFER_SIZE;
    count++;
    cnd_signal(&not_empty);
    mtx_unlock(&lock);
}

int consume(void) {
    mtx_lock(&lock);
    while (count == 0) {
        cnd_wait(&not_empty, &lock);
    }
    int value = buffer[tail];
    tail = (tail + 1) % BUFFER_SIZE;
    count--;
    cnd_signal(&not_full);
    mtx_unlock(&lock);
    return value;
}

int producer_thread(void *arg) {
    (void)arg;
    for (int i = 0; i < 20; i++) {
        produce(i);
        printf("Produced: %d\n", i);
    }
    return 0;
}

int consumer_thread(void *arg) {
    (void)arg;
    for (int i = 0; i < 20; i++) {
        int v = consume();
        printf("Consumed: %d\n", v);
    }
    return 0;
}

int main(void) {
    mtx_init(&lock, mtx_plain);
    cnd_init(&not_full);
    cnd_init(&not_empty);
    
    thrd_t p, c;
    thrd_create(&p, producer_thread, NULL);
    thrd_create(&c, consumer_thread, NULL);
    
    thrd_join(p, NULL);
    thrd_join(c, NULL);
    
    mtx_destroy(&lock);
    cnd_destroy(&not_full);
    cnd_destroy(&not_empty);
    
    return 0;
}
