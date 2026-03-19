/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\02_Stack_Queue_Complete.md
 * Line: 1058
 * Language: c
 * Block ID: 9d93090d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 双端队列 - 循环数组实现
 */
typedef struct {
    int* data;
    size_t capacity;
    size_t front;    // 队首索引
    size_t size;     // 当前元素数量
} Deque;

#define DEQUE_INITIAL_CAPACITY 16

void deque_init(Deque* deque) {
    deque->capacity = DEQUE_INITIAL_CAPACITY;
    deque->data = (int*)malloc(deque->capacity * sizeof(int));
    if (deque->data == nullptr) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    deque->front = 0;
    deque->size = 0;
}

void deque_destroy(Deque* deque) {
    free(deque->data);
    deque->data = nullptr;
    deque->capacity = 0;
    deque->front = 0;
    deque->size = 0;
}

static inline bool deque_is_empty(const Deque* deque) {
    return deque->size == 0;
}

static inline size_t deque_get_size(const Deque* deque) {
    return deque->size;
}

// 获取物理索引
static inline size_t deque_idx(const Deque* deque, size_t logical_idx) {
    return (deque->front + logical_idx) % deque->capacity;
}

// 扩容
static void deque_resize(Deque* deque) {
    size_t new_capacity = deque->capacity * 2;
    int* new_data = (int*)malloc(new_capacity * sizeof(int));
    if (new_data == nullptr) {
        fprintf(stderr, "Error: Failed to resize deque\n");
        exit(EXIT_FAILURE);
    }

    // 重新排列
    for (size_t i = 0; i < deque->size; i++) {
        new_data[i] = deque->data[deque_idx(deque, i)];
    }

    free(deque->data);
    deque->data = new_data;
    deque->capacity = new_capacity;
    deque->front = 0;
}

// 前端插入
bool deque_push_front(Deque* deque, int value) {
    if (deque->size >= deque->capacity) {
        deque_resize(deque);
    }

    deque->front = (deque->front + deque->capacity - 1) % deque->capacity;
    deque->data[deque->front] = value;
    deque->size++;
    return true;
}

// 后端插入
bool deque_push_back(Deque* deque, int value) {
    if (deque->size >= deque->capacity) {
        deque_resize(deque);
    }

    size_t back = deque_idx(deque, deque->size);
    deque->data[back] = value;
    deque->size++;
    return true;
}

// 前端删除
bool deque_pop_front(Deque* deque, int* out_value) {
    if (deque_is_empty(deque)) {
        fprintf(stderr, "Error: Deque is empty\n");
        return false;
    }

    *out_value = deque->data[deque->front];
    deque->front = (deque->front + 1) % deque->capacity;
    deque->size--;
    return true;
}

// 后端删除
bool deque_pop_back(Deque* deque, int* out_value) {
    if (deque_is_empty(deque)) {
        fprintf(stderr, "Error: Deque is empty\n");
        return false;
    }

    size_t back_idx = deque_idx(deque, deque->size - 1);
    *out_value = deque->data[back_idx];
    deque->size--;
    return true;
}

// 查看前端
bool deque_front(const Deque* deque, int* out_value) {
    if (deque_is_empty(deque)) return false;
    *out_value = deque->data[deque->front];
    return true;
}

// 查看后端
bool deque_back(const Deque* deque, int* out_value) {
    if (deque_is_empty(deque)) return false;
    size_t back_idx = deque_idx(deque, deque->size - 1);
    *out_value = deque->data[back_idx];
    return true;
}

// 打印双端队列
void deque_print(const Deque* deque) {
    printf("Deque [size=%zu, capacity=%zu]: front [",
           deque->size, deque->capacity);
    for (size_t i = 0; i < deque->size; i++) {
        printf("%d", deque->data[deque_idx(deque, i)]);
        if (i < deque->size - 1) printf(", ");
    }
    printf("] back\n");
}
