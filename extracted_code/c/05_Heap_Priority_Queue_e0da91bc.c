/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 867
 * Language: c
 * Block ID: e0da91bc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 找数组中第K大的元素
 * 使用大小为K的最小堆
 *
 * 时间复杂度: O(n log k)
 * 空间复杂度: O(k)
 */
int find_kth_largest(int* nums, int numsSize, int k) {
    // 最小堆 (存储最大的K个元素，堆顶是其中最小的)
    Heap min_heap;
    heap_init(&min_heap, MIN_HEAP);

    for (int i = 0; i < numsSize; i++) {
        if (min_heap.size < (size_t)k) {
            heap_push(&min_heap, nums[i]);
        } else if (nums[i] > min_heap.data[0]) {
            // 当前元素比堆顶大，替换
            min_heap.data[0] = nums[i];
            heap_sink(&min_heap, 0);
        }
    }

    int result = min_heap.data[0];
    heap_destroy(&min_heap);
    return result;
}
