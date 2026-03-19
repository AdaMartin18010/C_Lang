/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 773
 * Language: c
 * Block ID: d31d689c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 任务调度器 - 使用优先队列
 * 每次执行优先级最高的任务
 */

typedef struct {
    int task_id;
    int priority;      // 数值越大优先级越高
    int duration;      // 执行时长
    char name[64];
} Task;

// 任务队列 (最大堆)
typedef struct {
    Task* tasks;
    size_t size;
    size_t capacity;
} TaskQueue;

void task_queue_init(TaskQueue* queue) {
    queue->capacity = 16;
    queue->tasks = (Task*)malloc(queue->capacity * sizeof(Task));
    queue->size = 0;
}

void task_queue_push(TaskQueue* queue, Task task) {
    if (queue->size >= queue->capacity) {
        queue->capacity *= 2;
        queue->tasks = (Task*)realloc(queue->tasks, queue->capacity * sizeof(Task));
    }

    size_t k = queue->size;
    queue->tasks[k] = task;

    // 上浮 (按优先级)
    while (k > 0) {
        size_t parent = (k - 1) / 2;
        if (queue->tasks[parent].priority >= queue->tasks[k].priority) {
            break;
        }
        Task temp = queue->tasks[parent];
        queue->tasks[parent] = queue->tasks[k];
        queue->tasks[k] = temp;
        k = parent;
    }

    queue->size++;
}

bool task_queue_pop(TaskQueue* queue, Task* out) {
    if (queue->size == 0) return false;

    *out = queue->tasks[0];
    queue->size--;

    if (queue->size > 0) {
        queue->tasks[0] = queue->tasks[queue->size];

        // 下沉
        size_t k = 0;
        while (true) {
            size_t left = 2 * k + 1;
            size_t right = 2 * k + 2;
            size_t largest = k;

            if (left < queue->size &&
                queue->tasks[left].priority > queue->tasks[largest].priority) {
                largest = left;
            }
            if (right < queue->size &&
                queue->tasks[right].priority > queue->tasks[largest].priority) {
                largest = right;
            }

            if (largest == k) break;

            Task temp = queue->tasks[k];
            queue->tasks[k] = queue->tasks[largest];
            queue->tasks[largest] = temp;
            k = largest;
        }
    }

    return true;
}
