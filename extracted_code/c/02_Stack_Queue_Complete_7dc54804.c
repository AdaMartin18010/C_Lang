/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\02_Stack_Queue_Complete.md
 * Line: 629
 * Language: c
 * Block ID: 7dc54804
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 循环队列 (Circular Queue)
 * 解决普通数组队列的"假溢出"问题
 *
 * 队空条件: front == rear
 * 队满条件: (rear + 1) % capacity == front
 * 留一个空位区分队空和队满
 */
typedef struct {
    int* data;
    size_t capacity;     // 最大容量 (实际可用 capacity - 1)
    size_t front;        // 队首索引
    size_t rear;         // 队尾索引 (指向下一个插入位置)
} CircularQueue;

#define QUEUE_INITIAL_CAPACITY 16

// 初始化
void circular_queue_init(CircularQueue* queue) {
    queue->capacity = QUEUE_INITIAL_CAPACITY;
    queue->data = (int*)malloc(queue->capacity * sizeof(int));
    if (queue->data == nullptr) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    queue->front = 0;
    queue->rear = 0;
}

// 销毁
void circular_queue_destroy(CircularQueue* queue) {
    free(queue->data);
    queue->data = nullptr;
    queue->capacity = 0;
    queue->front = queue->rear = 0;
}

// 判断是否为空
static inline bool circular_queue_is_empty(const CircularQueue* queue) {
    return queue->front == queue->rear;
}

// 判断是否已满
static inline bool circular_queue_is_full(const CircularQueue* queue) {
    return (queue->rear + 1) % queue->capacity == queue->front;
}

// 获取大小
static inline size_t circular_queue_size(const CircularQueue* queue) {
    return (queue->rear + queue->capacity - queue->front) % queue->capacity;
}

// 扩容
static void circular_queue_resize(CircularQueue* queue) {
    size_t old_capacity = queue->capacity;
    size_t new_capacity = old_capacity * 2;
    int* new_data = (int*)malloc(new_capacity * sizeof(int));
    if (new_data == nullptr) {
        fprintf(stderr, "Error: Failed to resize queue\n");
        exit(EXIT_FAILURE);
    }

    // 重新排列元素 (可能跨越数组末尾)
    size_t size = circular_queue_size(queue);
    for (size_t i = 0; i < size; i++) {
        new_data[i] = queue->data[(queue->front + i) % old_capacity];
    }

    free(queue->data);
    queue->data = new_data;
    queue->capacity = new_capacity;
    queue->front = 0;
    queue->rear = size;

    printf("Queue resized: %zu -> %zu\n", old_capacity, new_capacity);
}

// 入队
bool circular_queue_enqueue(CircularQueue* queue, int value) {
    if (circular_queue_is_full(queue)) {
        circular_queue_resize(queue);
    }

    queue->data[queue->rear] = value;
    queue->rear = (queue->rear + 1) % queue->capacity;
    return true;
}

// 出队
bool circular_queue_dequeue(CircularQueue* queue, int* out_value) {
    if (circular_queue_is_empty(queue)) {
        fprintf(stderr, "Error: Queue is empty\n");
        return false;
    }

    *out_value = queue->data[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    return true;
}

// 查看队首
bool circular_queue_front(const CircularQueue* queue, int* out_value) {
    if (circular_queue_is_empty(queue)) {
        fprintf(stderr, "Error: Queue is empty\n");
        return false;
    }
    *out_value = queue->data[queue->front];
    return true;
}

// 打印队列
void circular_queue_print(const CircularQueue* queue) {
    printf("CircularQueue [size=%zu, capacity=%zu]: front [",
           circular_queue_size(queue), queue->capacity);

    size_t size = circular_queue_size(queue);
    for (size_t i = 0; i < size; i++) {
        size_t idx = (queue->front + i) % queue->capacity;
        printf("%d", queue->data[idx]);
        if (i < size - 1) printf(", ");
    }
    printf("] rear\n");

    // 显示物理存储
    printf("  Physical: [");
    for (size_t i = 0; i < queue->capacity; i++) {
        if (i == queue->front) printf("F:");
        if (i == queue->rear) printf("R:");
        printf("%d", queue->data[i]);
        if (i < queue->capacity - 1) printf(", ");
    }
    printf("]\n");
}
