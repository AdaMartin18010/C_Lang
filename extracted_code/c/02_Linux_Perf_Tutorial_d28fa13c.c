/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\02_Linux_Perf_Tutorial.md
 * Line: 1452
 * Language: c
 * Block ID: d28fa13c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* lock_contention.c - 锁竞争测试 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 8
#define ITERATIONS 10000000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
volatile long shared_counter = 0;

void *worker(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);
        shared_counter++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    printf("Starting lock contention test with %d threads...\n", NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final counter: %ld (expected: %d)\n",
           shared_counter, NUM_THREADS * ITERATIONS);

    return 0;
}
