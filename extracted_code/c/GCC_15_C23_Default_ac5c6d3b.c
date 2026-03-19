/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 1482
 * Language: c
 * Block ID: ac5c6d3b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C23 线程安全时间处理 */
#include <stdio.h>
#include <time.h>
#include <pthread.h>

/* 线程数据 */
typedef struct {
    int thread_id;
    time_t timestamp;
} thread_data_t;

/* 线程函数 */
void* time_worker(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;

    /* 每个线程有自己的 tm 结构体 */
    struct tm timeinfo;

    /* 使用线程安全的 gmtime_r */
    if (gmtime_r(&data->timestamp, &timeinfo)) {
        char buffer[80];
        strftime(buffer, sizeof(buffer),
                 "%Y-%m-%d %H:%M:%S UTC", &timeinfo);
        printf("Thread %d: %s\n", data->thread_id, buffer);
    }

    /* 使用线程安全的 localtime_r */
    if (localtime_r(&data->timestamp, &timeinfo)) {
        char buffer[80];
        strftime(buffer, sizeof(buffer),
                 "%Y-%m-%d %H:%M:%S Local", &timeinfo);
        printf("Thread %d: %s\n", data->thread_id, buffer);
    }

    return NULL;
}

/* 非线程安全 vs 线程安全对比 */
void compare_safety(void) {
    time_t now = time(NULL);

    /* 非线程安全 (C89) - 返回静态缓冲区指针 */
    struct tm* unsafe = gmtime(&now);
    printf("Unsafe: %d-%02d-%02d\n",
           unsafe->tm_year + 1900,
           unsafe->tm_mon + 1,
           unsafe->tm_mday);

    /* 如果有另一个线程调用 gmtime()，上面的数据可能被覆盖！ */

    /* 线程安全 (C23/POSIX) - 使用调用者提供的缓冲区 */
    struct tm safe;
    if (gmtime_r(&now, &safe)) {
        printf("Safe: %d-%02d-%02d\n",
               safe.tm_year + 1900,
               safe.tm_mon + 1,
               safe.tm_mday);
    }
    /* safe 是局部变量，不会被其他线程影响 */
}

int main(void) {
    pthread_t threads[3];
    thread_data_t data[3];
    time_t now = time(NULL);

    /* 创建多个线程 */
    for (int i = 0; i < 3; i++) {
        data[i].thread_id = i;
        data[i].timestamp = now + i * 3600;  /* 不同时间 */
        pthread_create(&threads[i], NULL, time_worker, &data[i]);
    }

    /* 等待线程完成 */
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n--- Safety Comparison ---\n");
    compare_safety();

    return 0;
}
