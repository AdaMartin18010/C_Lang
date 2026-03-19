/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\10_Threads_C11.md
 * Line: 1048
 * Language: c
 * Block ID: b91af392
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <string.h>

/* TSS键 */
static tss_t g_thread_log_key;
static tss_t g_thread_id_key;

/* 线程日志缓冲区 */
typedef struct {
    char* buffer;
    size_t size;
    size_t used;
    int thread_id;
} ThreadLog;

/* TSS析构函数 - 线程退出时自动调用 */
void thread_log_dtor(void* ptr) {
    ThreadLog* log = (ThreadLog*)ptr;
    if (log) {
        printf("[TSS Cleanup] Thread %d log: %zu bytes, content: %s\n",
               log->thread_id, log->used, log->buffer);
        free(log->buffer);
        free(log);
    }
}

/* 获取当前线程的日志缓冲区 */
ThreadLog* get_thread_log(void) {
    ThreadLog* log = (ThreadLog*)tss_get(g_thread_log_key);
    if (!log) {
        /* 首次使用，创建新的日志缓冲区 */
        log = malloc(sizeof(ThreadLog));
        log->buffer = malloc(1024);
        log->size = 1024;
        log->used = 0;
        log->buffer[0] = '\0';

        int* tid = (int*)tss_get(g_thread_id_key);
        log->thread_id = tid ? *tid : -1;

        tss_set(g_thread_log_key, log);
    }
    return log;
}

/* 线程安全的日志记录 */
void thread_safe_log(const char* msg) {
    ThreadLog* log = get_thread_log();
    size_t len = strlen(msg);

    if (log->used + len + 2 >= log->size) {
        /* 扩容 */
        log->size *= 2;
        log->buffer = realloc(log->buffer, log->size);
    }

    strcat(log->buffer + log->used, msg);
    log->used += len;
    strcat(log->buffer + log->used, ";");
    log->used++;
}

/* 工作线程 */
int worker_with_tss(void *arg) {
    int id = *(int*)arg;

    /* 设置线程ID */
    int* tid = malloc(sizeof(int));
    *tid = id;
    tss_set(g_thread_id_key, tid);

    char msg[64];
    snprintf(msg, sizeof(msg), "Thread %d starting", id);
    thread_safe_log(msg);

    /* 模拟工作 */
    for (int i = 0; i < 5; i++) {
        snprintf(msg, sizeof(msg), "Task %d.%d completed", id, i);
        thread_safe_log(msg);
        thrd_sleep(&(struct timespec){.tv_nsec = 10000000}, NULL);
    }

    snprintf(msg, sizeof(msg), "Thread %d finished", id);
    thread_safe_log(msg);

    /* 注意：tid需要在另一个析构函数中释放，
       或者可以使用复合结构体 */
    free(tid);
    tss_set(g_thread_id_key, NULL);

    return 0;
}

/* 使用线程本地存储的errno替代方案 */
typedef struct {
    int code;
    char msg[256];
} ThreadError;

static tss_t g_error_key;

void set_thread_error(int code, const char* msg) {
    ThreadError* err = (ThreadError*)tss_get(g_error_key);
    if (!err) {
        err = malloc(sizeof(ThreadError));
        tss_set(g_error_key, err);
    }
    err->code = code;
    strncpy(err->msg, msg, sizeof(err->msg) - 1);
    err->msg[sizeof(err->msg) - 1] = '\0';
}

ThreadError* get_thread_error(void) {
    return (ThreadError*)tss_get(g_error_key);
}

/* 错误TSS析构 */
void error_dtor(void* ptr) {
    free(ptr);
}

int main(void) {
    printf("=== Thread-Specific Storage Demo ===\n\n");

    /* 初始化TSS键 */
    if (tss_create(&g_thread_log_key, thread_log_dtor) != thrd_success) {
        fprintf(stderr, "Failed to create TSS key\n");
        return 1;
    }

    if (tss_create(&g_error_key, error_dtor) != thrd_success) {
        fprintf(stderr, "Failed to create error TSS key\n");
        return 1;
    }

    /* 创建多个线程 */
    thrd_t threads[3];
    int ids[3] = {1, 2, 3};

    for (int i = 0; i < 3; i++) {
        thrd_create(&threads[i], worker_with_tss, &ids[i]);
    }

    for (int i = 0; i < 3; i++) {
        thrd_join(threads[i], NULL);
    }

    /* 清理TSS键 */
    tss_delete(g_thread_log_key);
    tss_delete(g_error_key);

    printf("\n=== TSS Demo Complete ===\n");
    return 0;
}
