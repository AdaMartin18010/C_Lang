/*
 * C11 <threads.h> 线程与互斥量示例
 * 编译: gcc -Wall -Wextra -std=c11 -o c11_threads c11_threads.c
 */
#include <stdio.h>
#include <threads.h>

#define NUM_THREADS 4
#define ITERATIONS 100000

static int counter = 0;
static mtx_t counter_mutex;

int worker(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        mtx_lock(&counter_mutex);
        counter++;
        mtx_unlock(&counter_mutex);
    }
    printf("Thread %d done\n", id);
    return 0;
}

int main(void) {
    thrd_t threads[NUM_THREADS];
    int ids[NUM_THREADS];
    
    mtx_init(&counter_mutex, mtx_plain);
    
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        thrd_create(&threads[i], worker, &ids[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        thrd_join(threads[i], NULL);
    }
    
    mtx_destroy(&counter_mutex);
    printf("Expected: %d, Actual: %d\n", NUM_THREADS * ITERATIONS, counter);
    return 0;
}
