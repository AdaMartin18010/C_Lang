/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 898
 * Language: c
 * Block ID: 527f5763
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 找数组中第K小的元素
 * 使用大小为K的最大堆
 */
int find_kth_smallest(int* nums, int numsSize, int k) {
    Heap max_heap;
    heap_init(&max_heap, MAX_HEAP);

    for (int i = 0; i < numsSize; i++) {
        if (max_heap.size < (size_t)k) {
            heap_push(&max_heap, nums[i]);
        } else if (nums[i] < max_heap.data[0]) {
            max_heap.data[0] = nums[i];
            heap_sink(&max_heap, 0);
        }
    }

    int result = max_heap.data[0];
    heap_destroy(&max_heap);
    return result;
}
