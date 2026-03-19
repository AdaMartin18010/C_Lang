/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\02_Stack_Queue_Complete.md
 * Line: 1453
 * Language: c
 * Block ID: b85832b8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 合并K个有序数组
 * 使用优先队列维护每个数组的当前最小值
 *
 * 时间复杂度: O(N log K)，N为总元素数，K为数组数
 */
typedef struct {
    int value;      // 当前值
    int array_idx;  // 来自哪个数组
    int elem_idx;   // 在数组中的位置
} HeapNode;

int* merge_k_sorted_arrays(int** arrays, int* sizes, int k, int* returnSize) {
    // 计算总大小
    *returnSize = 0;
    for (int i = 0; i < k; i++) {
        *returnSize += sizes[i];
    }

    int* result = (int*)malloc(*returnSize * sizeof(int));
    int result_idx = 0;

    // 使用优先队列 (简化版)
    PriorityQueue pq;
    pq_init(&pq);

    // 将每个数组的第一个元素入队
    for (int i = 0; i < k; i++) {
        if (sizes[i] > 0) {
            // 编码: value * (k+1) + array_idx
            int encoded = arrays[i][0] * (k + 1) + i;
            pq_enqueue(&pq, encoded, arrays[i][0]);
        }
    }

    // 跟踪每个数组的当前索引
    int* indices = (int*)calloc(k, sizeof(int));

    while (!pq_is_empty(&pq)) {
        int encoded, priority;
        pq_dequeue(&pq, &encoded, &priority);

        int array_idx = encoded % (k + 1);
        result[result_idx++] = priority;

        indices[array_idx]++;
        if (indices[array_idx] < sizes[array_idx]) {
            int next_val = arrays[array_idx][indices[array_idx]];
            int next_encoded = next_val * (k + 1) + array_idx;
            pq_enqueue(&pq, next_encoded, next_val);
        }
    }

    free(indices);
    pq_destroy(&pq);
    return result;
}
