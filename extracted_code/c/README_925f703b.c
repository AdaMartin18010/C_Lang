/*
 * Auto-generated from: 07_Modern_Toolchain\12_Modern_C_Standards\README.md
 * Line: 182
 * Language: c
 * Block ID: 925f703b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 1. 线程支持 */
#include <threads.h>

int thread_func(void* arg) {
    int* value = (int*)arg;
    printf("Thread: %d\n", *value);
    return 0;
}

int main(void) {
    thrd_t thread;
    int arg = 42;

    thrd_create(&thread, thread_func, &arg);
    thrd_join(thread, NULL);

    return 0;
}

/* 2. 互斥锁 */
#include <threads.h>

mtx_t mutex;
int counter = 0;

void increment(void) {
    mtx_lock(&mutex);
    counter++;
    mtx_unlock(&mutex);
}

/* 3. 条件变量 */
cnd_t condition;
mtx_t mutex;

void wait_for_signal(void) {
    mtx_lock(&mutex);
    cnd_wait(&condition, &mutex);
    mtx_unlock(&mutex);
}

void send_signal(void) {
    cnd_signal(&condition);
}

/* 4. 线程本地存储 */
_Thread_local int thread_local_var;  /* 每个线程独立 */
