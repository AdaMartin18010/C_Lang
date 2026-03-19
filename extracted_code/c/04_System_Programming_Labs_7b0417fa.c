/*
 * Auto-generated from: 12_Practice_Exercises\04_System_Programming_Labs.md
 * Line: 273
 * Language: c
 * Block ID: 7b0417fa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    Task task_queue[QUEUE_SIZE];
    int head, tail;
    sem_t empty_slots;
    sem_t filled_slots;
    pthread_mutex_t mutex;
} SharedQueue;

// 主进程: 将任务放入队列
// 工作进程: 从队列取出任务执行
