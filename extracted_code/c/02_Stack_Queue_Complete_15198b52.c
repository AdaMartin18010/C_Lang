/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\02_Stack_Queue_Complete.md
 * Line: 955
 * Language: c
 * Block ID: 15198b52
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 简单的任务调度器
 * 使用队列管理待执行的任务
 */

typedef enum {
    TASK_HIGH,
    TASK_NORMAL,
    TASK_LOW
} TaskPriority;

typedef struct Task {
    int id;
    TaskPriority priority;
    char name[64];
    void (*execute)(struct Task*);
} Task;

typedef struct {
    LinkedQueue high_queue;
    LinkedQueue normal_queue;
    LinkedQueue low_queue;
    int task_count;
} TaskScheduler;

void scheduler_init(TaskScheduler* scheduler) {
    linked_queue_init(&scheduler->high_queue);
    linked_queue_init(&scheduler->normal_queue);
    linked_queue_init(&scheduler->low_queue);
    scheduler->task_count = 0;
}

void scheduler_submit(TaskScheduler* scheduler, Task* task) {
    int encoded = (int)(intptr_t)task;

    switch (task->priority) {
        case TASK_HIGH:
            linked_queue_enqueue(&scheduler->high_queue, encoded);
            break;
        case TASK_NORMAL:
            linked_queue_enqueue(&scheduler->normal_queue, encoded);
            break;
        case TASK_LOW:
            linked_queue_enqueue(&scheduler->low_queue, encoded);
            break;
    }
    scheduler->task_count++;
    printf("Task '%s' submitted (priority: %d)\n",
           task->name, task->priority);
}

Task* scheduler_next(TaskScheduler* scheduler) {
    int encoded = 0;
    LinkedQueue* queue = nullptr;

    if (!linked_queue_is_empty(&scheduler->high_queue)) {
        queue = &scheduler->high_queue;
    } else if (!linked_queue_is_empty(&scheduler->normal_queue)) {
        queue = &scheduler->normal_queue;
    } else if (!linked_queue_is_empty(&scheduler->low_queue)) {
        queue = &scheduler->low_queue;
    }

    if (queue == nullptr) return nullptr;

    linked_queue_dequeue(queue, &encoded);
    scheduler->task_count--;
    return (Task*)(intptr_t)encoded;
}

void scheduler_run_all(TaskScheduler* scheduler) {
    Task* task;
    while ((task = scheduler_next(scheduler)) != nullptr) {
        printf("Executing task: %s\n", task->name);
        if (task->execute) {
            task->execute(task);
        }
    }
}
