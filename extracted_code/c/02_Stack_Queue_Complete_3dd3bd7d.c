/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\02_Stack_Queue_Complete.md
 * Line: 1316
 * Language: c
 * Block ID: 3dd3bd7d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 最小堆实现的优先队列
 */
typedef struct {
    int* data;           // 堆数组
    int* priorities;     // 优先级数组
    size_t capacity;
    size_t size;
} PriorityQueue;

#define PQ_INITIAL_CAPACITY 16

void pq_init(PriorityQueue* pq) {
    pq->capacity = PQ_INITIAL_CAPACITY;
    pq->data = (int*)malloc(pq->capacity * sizeof(int));
    pq->priorities = (int*)malloc(pq->capacity * sizeof(int));
    if (pq->data == nullptr || pq->priorities == nullptr) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    pq->size = 0;
}

void pq_destroy(PriorityQueue* pq) {
    free(pq->data);
    free(pq->priorities);
    pq->data = nullptr;
    pq->priorities = nullptr;
    pq->capacity = 0;
    pq->size = 0;
}

static inline bool pq_is_empty(const PriorityQueue* pq) {
    return pq->size == 0;
}

// 交换
static void pq_swap(PriorityQueue* pq, size_t i, size_t j) {
    int temp_data = pq->data[i];
    int temp_pri = pq->priorities[i];
    pq->data[i] = pq->data[j];
    pq->priorities[i] = pq->priorities[j];
    pq->data[j] = temp_data;
    pq->priorities[j] = temp_pri;
}

// 上浮
static void pq_swim(PriorityQueue* pq, size_t k) {
    while (k > 0) {
        size_t parent = (k - 1) / 2;
        if (pq->priorities[k] >= pq->priorities[parent]) break;
        pq_swap(pq, k, parent);
        k = parent;
    }
}

// 下沉
static void pq_sink(PriorityQueue* pq, size_t k) {
    while (2 * k + 1 < pq->size) {
        size_t left = 2 * k + 1;
        size_t right = 2 * k + 2;
        size_t smallest = left;

        if (right < pq->size && pq->priorities[right] < pq->priorities[left]) {
            smallest = right;
        }

        if (pq->priorities[k] <= pq->priorities[smallest]) break;

        pq_swap(pq, k, smallest);
        k = smallest;
    }
}

// 扩容
static void pq_resize(PriorityQueue* pq) {
    if (pq->size < pq->capacity) return;

    size_t new_capacity = pq->capacity * 2;
    pq->data = (int*)realloc(pq->data, new_capacity * sizeof(int));
    pq->priorities = (int*)realloc(pq->priorities, new_capacity * sizeof(int));
    pq->capacity = new_capacity;
}

// 入队
bool pq_enqueue(PriorityQueue* pq, int value, int priority) {
    pq_resize(pq);

    pq->data[pq->size] = value;
    pq->priorities[pq->size] = priority;
    pq_swim(pq, pq->size);
    pq->size++;
    return true;
}

// 出队
bool pq_dequeue(PriorityQueue* pq, int* out_value, int* out_priority) {
    if (pq_is_empty(pq)) {
        fprintf(stderr, "Error: Priority queue is empty\n");
        return false;
    }

    *out_value = pq->data[0];
    *out_priority = pq->priorities[0];

    pq->size--;
    pq->data[0] = pq->data[pq->size];
    pq->priorities[0] = pq->priorities[pq->size];

    pq_sink(pq, 0);
    return true;
}

// 查看队首
bool pq_peek(const PriorityQueue* pq, int* out_value, int* out_priority) {
    if (pq_is_empty(pq)) return false;
    *out_value = pq->data[0];
    *out_priority = pq->priorities[0];
    return true;
}

// 打印
void pq_print(const PriorityQueue* pq) {
    printf("PriorityQueue [size=%zu]: ", pq->size);
    for (size_t i = 0; i < pq->size; i++) {
        printf("(%d,p=%d)", pq->data[i], pq->priorities[i]);
        if (i < pq->size - 1) printf(" ");
    }
    printf("\n");
}
