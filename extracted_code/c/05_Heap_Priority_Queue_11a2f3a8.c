/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 646
 * Language: c
 * Block ID: 11a2f3a8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 合并K个有序数组
 * 使用最小堆维护K个数组的当前最小值
 *
 * 时间复杂度: O(N log K)，N为总元素数，K为数组数
 */

typedef struct {
    int value;      // 当前值
    int array_idx;  // 来自哪个数组
    int elem_idx;   // 在数组中的位置
} HeapNode;

typedef struct {
    HeapNode* data;
    size_t size;
    size_t capacity;
} MinHeapPQ;

// 比较函数 (最小堆)
static bool pq_node_compare_less(const HeapNode* a, const HeapNode* b) {
    return a->value > b->value;  // 注意: 这里是 > 因为我们想要最小堆
}

// 上浮
static void pq_swim(MinHeapPQ* pq, size_t k) {
    while (k > 0) {
        size_t parent = (k - 1) / 2;
        if (!pq_node_compare_less(&pq->data[parent], &pq->data[k])) {
            break;
        }
        HeapNode temp = pq->data[parent];
        pq->data[parent] = pq->data[k];
        pq->data[k] = temp;
        k = parent;
    }
}

// 下沉
static void pq_sink(MinHeapPQ* pq, size_t k) {
    size_t n = pq->size;
    while (true) {
        size_t left = 2 * k + 1;
        size_t right = 2 * k + 2;
        size_t smallest = k;

        if (left < n && pq_node_compare_less(&pq->data[smallest], &pq->data[left])) {
            smallest = left;
        }
        if (right < n && pq_node_compare_less(&pq->data[smallest], &pq->data[right])) {
            smallest = right;
        }

        if (smallest == k) break;

        HeapNode temp = pq->data[k];
        pq->data[k] = pq->data[smallest];
        pq->data[smallest] = temp;
        k = smallest;
    }
}

// 出队
static bool pq_pop(MinHeapPQ* pq, HeapNode* out) {
    if (pq->size == 0) return false;

    *out = pq->data[0];
    pq->size--;
    if (pq->size > 0) {
        pq->data[0] = pq->data[pq->size];
        pq_sink(pq, 0);
    }
    return true;
}

// 入队
static void pq_push(MinHeapPQ* pq, HeapNode node) {
    pq->data[pq->size] = node;
    pq_swim(pq, pq->size);
    pq->size++;
}

int* merge_k_sorted_arrays(int** arrays, int* sizes, int k, int* returnSize) {
    // 计算总大小
    *returnSize = 0;
    for (int i = 0; i < k; i++) {
        *returnSize += sizes[i];
    }

    int* result = (int*)malloc(*returnSize * sizeof(int));
    int result_idx = 0;

    // 初始化优先队列
    MinHeapPQ pq;
    pq.capacity = k;
    pq.data = (HeapNode*)malloc(k * sizeof(HeapNode));
    pq.size = 0;

    // 将每个数组的第一个元素入队
    for (int i = 0; i < k; i++) {
        if (sizes[i] > 0) {
            HeapNode node = {arrays[i][0], i, 0};
            pq_push(&pq, node);
        }
    }

    // 合并
    HeapNode node;
    while (pq_pop(&pq, &node)) {
        result[result_idx++] = node.value;

        // 从同一数组取下一个元素
        node.elem_idx++;
        if (node.elem_idx < sizes[node.array_idx]) {
            node.value = arrays[node.array_idx][node.elem_idx];
            pq_push(&pq, node);
        }
    }

    free(pq.data);
    return result;
}
