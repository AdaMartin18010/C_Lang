/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\README.md
 * Line: 64
 * Language: c
 * Block ID: 4ffdbb4c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* 线程属性配置示例 */
pthread_attr_t configure_thread_attributes(void) {
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    /* 设置分离状态 */
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    /* 设置栈大小 */
    size_t stack_size = 1024 * 1024; /* 1MB */
    pthread_attr_setstacksize(&attr, stack_size);

    /* 设置调度策略 */
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

    struct sched_param param;
    param.sched_priority = 50;
    pthread_attr_setschedparam(&attr, &param);

    return attr;
}

/* 线程特定数据 (TLS) */
pthread_key_t tls_key;

void destructor(void *value) {
    free(value);
}

void init_tls(void) {
    pthread_key_create(&tls_key, destructor);
}

void set_thread_data(int value) {
    int *data = malloc(sizeof(int));
    *data = value;
    pthread_setspecific(tls_key, data);
}
