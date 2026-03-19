/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\03_Advanced_Learning_Path.md
 * Line: 118
 * Language: c
 * Block ID: ffd348e0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stddef.h>
#include <stdbool.h>

typedef struct ThreadPool ThreadPool;
typedef void (*TaskFunc)(void *arg);

// 线程池配置
typedef struct {
    size_t min_threads;      // 最小线程数
    size_t max_threads;      // 最大线程数
    size_t queue_capacity;   // 任务队列容量
    size_t keepalive_time;   // 空闲线程存活时间(ms)
    bool enable_metrics;     // 是否启用统计
} TPConfig;

// 统计信息
typedef struct {
    size_t active_threads;
    size_t idle_threads;
    size_t total_tasks;
    size_t completed_tasks;
    size_t rejected_tasks;
    double avg_wait_time;    // 平均等待时间(ms)
    double avg_exec_time;    // 平均执行时间(ms)
} TPMetrics;

// API
ThreadPool* tp_create(const TPConfig *config);
void tp_destroy(ThreadPool *pool, bool wait_pending);
int tp_submit(ThreadPool *pool, TaskFunc func, void *arg,
              size_t timeout_ms);
bool tp_get_metrics(ThreadPool *pool, TPMetrics *metrics);
void tp_shutdown(ThreadPool *pool);

#endif
