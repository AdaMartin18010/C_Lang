/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 1320
 * Language: c
 * Block ID: d59df142
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 索引堆 - 支持快速修改任意位置元素的堆
 * 通过索引数组间接访问数据
 */

typedef struct {
    int* data;          // 实际数据
    int* indices;       // 索引: indices[i]表示堆中第i个位置的元素在data中的索引
    int* reverse;       // 反向索引: reverse[i]表示data[i]在堆中的位置
    size_t size;
    size_t capacity;
    bool is_max_heap;
} IndexHeap;

void index_heap_init(IndexHeap* heap, size_t capacity, bool is_max) {
    heap->capacity = capacity;
    heap->data = (int*)malloc(capacity * sizeof(int));
    heap->indices = (int*)malloc(capacity * sizeof(int));
    heap->reverse = (int*)malloc(capacity * sizeof(int));
    for (size_t i = 0; i < capacity; i++) {
        heap->reverse[i] = -1;  // -1表示不在堆中
    }
    heap->size = 0;
    heap->is_max_heap = is_max;
}

// 检查位置i的元素是否小于位置j的元素
static bool index_heap_less(const IndexHeap* heap, int i, int j) {
    int idx_i = heap->indices[i];
    int idx_j = heap->indices[j];
    if (heap->is_max_heap) {
        return heap->data[idx_i] < heap->data[idx_j];
    } else {
        return heap->data[idx_i] > heap->data[idx_j];
    }
}

// 交换堆中的两个位置
static void index_heap_swap(IndexHeap* heap, int i, int j) {
    int idx_i = heap->indices[i];
    int idx_j = heap->indices[j];

    heap->reverse[idx_i] = j;
    heap->reverse[idx_j] = i;

    int temp = heap->indices[i];
    heap->indices[i] = heap->indices[j];
    heap->indices[j] = temp;
}

// 上浮
static void index_heap_swim(IndexHeap* heap, int k) {
    while (k > 0) {
        int parent = (k - 1) / 2;
        if (!index_heap_less(heap, parent, k)) break;
        index_heap_swap(heap, k, parent);
        k = parent;
    }
}

// 下沉
static void index_heap_sink(IndexHeap* heap, int k) {
    int n = (int)heap->size;
    while (true) {
        int left = 2 * k + 1;
        int right = 2 * k + 2;
        int target = k;

        if (left < n && index_heap_less(heap, target, left)) {
            target = left;
        }
        if (right < n && index_heap_less(heap, target, right)) {
            target = right;
        }
        if (target == k) break;

        index_heap_swap(heap, k, target);
        k = target;
    }
}

// 修改指定索引的元素值
void index_heap_change(IndexHeap* heap, int data_idx, int new_value) {
    heap->data[data_idx] = new_value;

    int heap_pos = heap->reverse[data_idx];
    if (heap_pos < 0) return;  // 不在堆中

    // 尝试上浮或下沉
    index_heap_swim(heap, heap_pos);
    index_heap_sink(heap, heap_pos);
}

// 检查是否在堆中
bool index_heap_contains(const IndexHeap* heap, int data_idx) {
    return heap->reverse[data_idx] >= 0;
}
