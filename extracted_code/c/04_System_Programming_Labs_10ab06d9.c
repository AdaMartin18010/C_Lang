/*
 * Auto-generated from: 12_Practice_Exercises\04_System_Programming_Labs.md
 * Line: 246
 * Language: c
 * Block ID: 10ab06d9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct ProcessPool ProcessPool;
typedef void (*TaskFunc)(void *arg);

typedef struct {
    TaskFunc func;
    void *arg;
    void *result;
} Task;

ProcessPool* pool_create(int num_workers);
void pool_destroy(ProcessPool *pool);

// 提交任务
int pool_submit(ProcessPool *pool, TaskFunc func, void *arg);

// 获取结果
void* pool_get_result(ProcessPool *pool, int task_id);

// 等待所有任务完成
void pool_wait_all(ProcessPool *pool);
