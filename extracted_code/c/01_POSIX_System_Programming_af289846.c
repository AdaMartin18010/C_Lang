/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\POSIX_1_2024\01_POSIX_System_Programming.md
 * Line: 2679
 * Language: c
 * Block ID: af289846
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file posix_threads.c
 * @brief POSIX线程(pthread)编程
 * @description 线程创建、终止、连接、分离等操作
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

/**
 * @brief 简单线程函数
 * @param arg 线程参数
 * @return 返回值
 */
void *simple_thread(void *arg) {
    int num = *(int *)arg;
    printf("线程 %d: 启动, TID=%lu\n", num, (unsigned long)pthread_self());

    /* 模拟工作 */
    for (int i = 0; i < 3; i++) {
        printf("线程 %d: 工作中... %d\n", num, i + 1);
        usleep(500000);  /* 500ms */
    }

    printf("线程 %d: 结束\n", num);

    /* 返回状态 */
    int *result = malloc(sizeof(int));
    *result = num * 10;
    pthread_exit(result);
}

/**
 * @brief 基本线程创建演示
 */
void basic_thread_demo(void) {
    pthread_t threads[3];
    int thread_nums[3] = {1, 2, 3};
    void *status;

    printf("\n=== 基本线程创建 ===\n");
    printf("主线程: TID=%lu\n", (unsigned long)pthread_self());

    /* 创建多个线程 */
    for (int i = 0; i < 3; i++) {
        int ret = pthread_create(&threads[i], NULL, simple_thread, &thread_nums[i]);
        if (ret != 0) {
            fprintf(stderr, "创建线程失败: %s\n", strerror(ret));
            exit(1);
        }
        printf("主线程: 已创建线程 %d, TID=%lu\n",
               i + 1, (unsigned long)threads[i]);
    }

    /* 等待所有线程完成 */
    for (int i = 0; i < 3; i++) {
        int ret = pthread_join(threads[i], &status);
        if (ret != 0) {
            fprintf(stderr, "等待线程失败: %s\n", strerror(ret));
        } else {
            if (status != NULL) {
                printf("主线程: 线程 %d 返回 %d\n", i + 1, *(int *)status);
                free(status);
            }
        }
    }

    printf("主线程: 所有线程已结束\n");
}

/**
 * @brief 线程分离演示
 */
void detached_thread_demo(void) {
    pthread_t thread;
    pthread_attr_t attr;

    printf("\n=== 分离线程 ===\n");

    /* 初始化线程属性 */
    pthread_attr_init(&attr);
    /* 设置分离状态 */
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    /* 创建分离线程 */
    int ret = pthread_create(&thread, &attr, simple_thread, &(int){4});
    if (ret != 0) {
        fprintf(stderr, "创建线程失败: %s\n", strerror(ret));
    } else {
        printf("分离线程已创建，不能join\n");
    }

    /* 销毁属性 */
    pthread_attr_destroy(&attr);

    /* 等待分离线程完成 */
    sleep(3);
    printf("分离线程演示结束\n");
}

/**
 * @brief 线程取消演示
 */
void *cancelable_thread(void *arg) {
    int oldstate;

    printf("线程: 启动\n");

    /* 设置取消状态为启用 */
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);
    /* 设置取消类型为延迟取消 */
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    /* 设置取消点 */
    pthread_testcancel();

    printf("线程: 进入长时间操作...\n");

    /* 模拟长时间操作，包含取消点 */
    for (int i = 0; i < 10; i++) {
        sleep(1);  /* sleep是一个取消点 */
        printf("线程: 工作中 %d/10\n", i + 1);
    }

    printf("线程: 正常结束\n");
    return NULL;
}

void thread_cancel_demo(void) {
    pthread_t thread;
    void *status;

    printf("\n=== 线程取消 ===\n");

    pthread_create(&thread, NULL, cancelable_thread, NULL);

    /* 2秒后取消线程 */
    sleep(2);
    printf("主线程: 请求取消工作线程\n");
    pthread_cancel(thread);

    int ret = pthread_join(thread, &status);
    if (ret == 0) {
        if (status == PTHREAD_CANCELED) {
            printf("主线程: 工作线程已被取消\n");
        } else {
            printf("主线程: 工作线程正常返回\n");
        }
    }
}

/**
 * @brief 线程特定数据（TLS）
 */
static pthread_key_t thread_key;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;

void make_key(void) {
    pthread_key_create(&thread_key, free);
}

void *tls_thread(void *arg) {
    int num = *(int *)arg;

    /* 确保key只创建一次 */
    pthread_once(&key_once, make_key);

    /* 分配线程特定数据 */
    int *data = malloc(sizeof(int));
    *data = num * 100;
    pthread_setspecific(thread_key, data);

    printf("线程 %d: 设置TLS值 = %d\n", num, *data);

    /* 读取TLS */
    int *tls_value = pthread_getspecific(thread_key);
    printf("线程 %d: 读取TLS值 = %d\n", num, *tls_value);

    return NULL;
}

void thread_specific_data_demo(void) {
    pthread_t threads[3];
    int nums[3] = {1, 2, 3};

    printf("\n=== 线程特定数据(TLS) ===\n");

    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, tls_thread, &nums[i]);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_key_delete(thread_key);
}

/**
 * @brief 线程栈大小设置
 */
void thread_stack_demo(void) {
    pthread_t thread;
    pthread_attr_t attr;
    size_t stacksize;

    printf("\n=== 线程栈大小 ===\n");

    pthread_attr_init(&attr);

    /* 获取默认栈大小 */
    pthread_attr_getstacksize(&attr, &stacksize);
    printf("默认栈大小: %zu KB\n", stacksize / 1024);

    /* 设置新栈大小（最小1MB） */
    size_t new_stacksize = 2 * 1024 * 1024;  /* 2MB */
    pthread_attr_setstacksize(&attr, new_stacksize);

    pthread_attr_getstacksize(&attr, &stacksize);
    printf("新栈大小: %zu KB\n", stacksize / 1024);

    pthread_attr_destroy(&attr);
}

int main(void) {
    printf("=== POSIX线程管理演示 ===\n");

    basic_thread_demo();
    detached_thread_demo();
    thread_cancel_demo();
    thread_specific_data_demo();
    thread_stack_demo();

    printf("\n=== 线程管理演示完成 ===\n");
    return 0;
}
