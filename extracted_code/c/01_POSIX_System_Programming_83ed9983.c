/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\POSIX_1_2024\01_POSIX_System_Programming.md
 * Line: 2929
 * Language: c
 * Block ID: 83ed9983
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file posix_mutex.c
 * @brief POSIX互斥锁
 * @description 互斥锁的创建、锁定、解锁、属性设置
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

static int shared_counter = 0;
static pthread_mutex_t mutex;

/**
 * @brief 基本互斥锁演示
 */
void *mutex_thread(void *arg) {
    int num = *(int *)arg;

    for (int i = 0; i < 1000; i++) {
        /* 锁定互斥锁 */
        pthread_mutex_lock(&mutex);

        /* 临界区：访问共享资源 */
        int temp = shared_counter;
        temp++;
        usleep(1);  /* 模拟一些工作 */
        shared_counter = temp;

        /* 解锁 */
        pthread_mutex_unlock(&mutex);
    }

    printf("线程 %d 完成\n", num);
    return NULL;
}

void basic_mutex_demo(void) {
    pthread_t threads[4];
    int nums[4] = {1, 2, 3, 4};

    printf("\n=== 基本互斥锁 ===\n");

    /* 初始化互斥锁（默认属性） */
    pthread_mutex_init(&mutex, NULL);

    shared_counter = 0;

    /* 创建多个线程 */
    for (int i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, mutex_thread, &nums[i]);
    }

    /* 等待所有线程 */
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("最终计数器值: %d (期望: 4000)\n", shared_counter);

    pthread_mutex_destroy(&mutex);
}

/**
 * @brief 递归互斥锁演示
 */
void recursive_mutex_demo(void) {
    pthread_mutex_t rec_mutex;
    pthread_mutexattr_t attr;

    printf("\n=== 递归互斥锁 ===\n");

    /* 初始化属性 */
    pthread_mutexattr_init(&attr);
    /* 设置为递归类型 */
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    /* 用属性初始化互斥锁 */
    pthread_mutex_init(&rec_mutex, &attr);

    /* 同一线程多次锁定 */
    printf("第一次锁定...\n");
    pthread_mutex_lock(&rec_mutex);

    printf("第二次锁定（递归）...\n");
    pthread_mutex_lock(&rec_mutex);

    printf("第三次锁定（递归）...\n");
    pthread_mutex_lock(&rec_mutex);

    /* 需要相同次数的解锁 */
    pthread_mutex_unlock(&rec_mutex);
    pthread_mutex_unlock(&rec_mutex);
    pthread_mutex_unlock(&rec_mutex);

    printf("递归互斥锁解锁完成\n");

    pthread_mutexattr_destroy(&attr);
    pthread_mutex_destroy(&rec_mutex);
}

/**
 * @brief 带超时的互斥锁
 */
void timed_mutex_demo(void) {
    pthread_mutex_t timed_mutex;
    struct timespec timeout;

    printf("\n=== 超时互斥锁 ===\n");

    pthread_mutex_init(&timed_mutex, NULL);
    pthread_mutex_lock(&timed_mutex);  /* 先锁定 */

    /* 计算超时时间 */
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 2;  /* 2秒超时 */

    printf("尝试在2秒内获取已锁定的互斥锁...\n");
    int ret = pthread_mutex_timedlock(&timed_mutex, &timeout);

    if (ret == ETIMEDOUT) {
        printf("超时，未能获取互斥锁\n");
    }

    pthread_mutex_unlock(&timed_mutex);
    pthread_mutex_destroy(&timed_mutex);
}

/**
 * @brief 优先级继承协议
 */
void priority_inheritance_demo(void) {
    pthread_mutex_t pi_mutex;
    pthread_mutexattr_t attr;
    int protocol;

    printf("\n=== 优先级继承协议 ===\n");

    pthread_mutexattr_init(&attr);

    /* 设置优先级继承协议 */
    pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);

    /* 获取并验证 */
    pthread_mutexattr_getprotocol(&attr, &protocol);
    printf("互斥锁协议: ");
    switch (protocol) {
        case PTHREAD_PRIO_NONE:     printf("无\n"); break;
        case PTHREAD_PRIO_INHERIT:  printf("优先级继承\n"); break;
        case PTHREAD_PRIO_PROTECT:  printf("优先级保护\n"); break;
        default: printf("未知\n");
    }

    pthread_mutex_init(&pi_mutex, &attr);

    pthread_mutexattr_destroy(&attr);
    pthread_mutex_destroy(&pi_mutex);
}

int main(void) {
    printf("=== POSIX互斥锁演示 ===\n");

    basic_mutex_demo();
    recursive_mutex_demo();
    timed_mutex_demo();
    priority_inheritance_demo();

    printf("\n=== 互斥锁演示完成 ===\n");
    return 0;
}
