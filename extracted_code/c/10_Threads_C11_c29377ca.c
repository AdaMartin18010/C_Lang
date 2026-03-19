/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\10_Threads_C11.md
 * Line: 188
 * Language: c
 * Block ID: c29377ca
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>
#include <string.h>

/* 线程参数结构体 */
typedef struct {
    int id;
    int priority;
    const char* name;
    int workload;
} ThreadArg;

/* 线程结果结构体 */
typedef struct {
    int id;
    int status;
    long items_processed;
    double elapsed_ms;
} ThreadResult;

/* 工作线程函数 */
int worker_thread(void *arg) {
    ThreadArg* params = (ThreadArg*)arg;
    ThreadResult* result = malloc(sizeof(ThreadResult));

    if (!result) {
        fprintf(stderr, "Thread %d: Failed to allocate result\n", params->id);
        thrd_exit(thrd_nomem);
    }

    result->id = params->id;
    result->status = thrd_success;
    result->items_processed = 0;

    struct timespec start, end;
    timespec_get(&start, TIME_UTC);

    printf("[Thread %d] '%s' started, workload=%d\n",
           params->id, params->name, params->workload);

    /* 模拟工作 */
    for (int i = 0; i < params->workload; i++) {
        /* 实际计算任务 */
        volatile double sum = 0.0;
        for (int j = 0; j < 10000; j++) {
            sum += j * 0.001;
        }
        result->items_processed++;

        /* 每处理10项让出CPU */
        if (i % 10 == 0) {
            thrd_yield();
        }
    }

    timespec_get(&end, TIME_UTC);
    result->elapsed_ms = (end.tv_sec - start.tv_sec) * 1000.0 +
                         (end.tv_nsec - start.tv_nsec) / 1000000.0;

    printf("[Thread %d] Completed: %ld items in %.2f ms\n",
           params->id, result->items_processed, result->elapsed_ms);

    /* 通过thrd_exit返回结果指针 */
    thrd_exit((int)(uintptr_t)result);
    return 0; /* 永远不会执行 */
}

/* 演示线程创建、连接和分离 */
int demonstrate_thread_lifecycle(void) {
    #define NUM_THREADS 5

    thrd_t threads[NUM_THREADS];
    ThreadArg args[NUM_THREADS];
    ThreadResult* results[NUM_THREADS];

    /* 初始化线程参数 */
    const char* names[] = {"Worker-A", "Worker-B", "Worker-C",
                           "Worker-D", "Worker-E"};
    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].id = i + 1;
        args[i].priority = i % 3;
        args[i].name = names[i];
        args[i].workload = 50 + (i * 25); /* 递增工作量 */
    }

    printf("=== Phase 1: Creating %d threads ===\n", NUM_THREADS);

    /* 创建线程 */
    for (int i = 0; i < NUM_THREADS; i++) {
        int rc = thrd_create(&threads[i], worker_thread, &args[i]);
        if (rc != thrd_success) {
            fprintf(stderr, "Failed to create thread %d: %d\n", i, rc);
            /* 清理已创建的线程 */
            for (int j = 0; j < i; j++) {
                thrd_detach(threads[j]);
            }
            return -1;
        }
        printf("Created thread %d (thrd_t handle obtained)\n", args[i].id);
    }

    printf("\n=== Phase 2: Joining first 3 threads ===\n");

    /* 连接前3个线程，获取结果 */
    for (int i = 0; i < 3; i++) {
        int res_code;
        int rc = thrd_join(threads[i], &res_code);

        if (rc != thrd_success) {
            fprintf(stderr, "thrd_join failed for thread %d: %d\n",
                    args[i].id, rc);
            continue;
        }

        /* 恢复返回的结果指针 */
        results[i] = (ThreadResult*)(uintptr_t)res_code;
        printf("Thread %d joined: status=%d, processed=%ld, time=%.2fms\n",
               results[i]->id, results[i]->status,
               results[i]->items_processed, results[i]->elapsed_ms);

        free(results[i]);
    }

    printf("\n=== Phase 3: Detaching remaining threads ===\n");

    /* 分离后2个线程（让它们自动清理） */
    for (int i = 3; i < NUM_THREADS; i++) {
        int rc = thrd_detach(threads[i]);
        if (rc != thrd_success) {
            fprintf(stderr, "thrd_detach failed for thread %d: %d\n",
                    args[i].id, rc);
        } else {
            printf("Thread %d detached (will auto-cleanup)\n", args[i].id);
        }
    }

    /* 给分离的线程时间完成 */
    thrd_sleep(&(struct timespec){.tv_sec = 2}, NULL);

    printf("\n=== All threads completed ===\n");
    return 0;
}

/* 线程退出方式对比 */
int thread_exit_demo(void *arg) {
    int mode = *(int*)arg;

    switch (mode) {
        case 0:
            /* 方式1: return 退出 */
            printf("Thread exiting via return\n");
            return 42;

        case 1:
            /* 方式2: thrd_exit 退出 */
            printf("Thread exiting via thrd_exit\n");
            thrd_exit(100);
            printf("This will never print!\n");
            break;

        case 2:
            /* 方式3: main返回导致所有线程终止 */
            /* 注意: 在worker线程中调用exit会终止整个进程 */
            printf("Thread calling thrd_exit(0) for clean exit\n");
            thrd_exit(0);
    }
    return -1;
}

int main(void) {
    printf("C11 Thread Lifecycle Demonstration\n");
    printf("==================================\n\n");

    demonstrate_thread_lifecycle();

    printf("\n\n=== Thread Exit Demo ===\n");
    thrd_t t1, t2;
    int mode1 = 0, mode2 = 1;

    thrd_create(&t1, thread_exit_demo, &mode1);
    thrd_create(&t2, thread_exit_demo, &mode2);

    int res1, res2;
    thrd_join(t1, &res1);
    thrd_join(t2, &res2);

    printf("Thread 1 return value: %d\n", res1);
    printf("Thread 2 return value: %d\n", res2);

    return 0;
}
