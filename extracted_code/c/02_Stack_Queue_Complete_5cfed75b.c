/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\02_Stack_Queue_Complete.md
 * Line: 1515
 * Language: c
 * Block ID: 5cfed75b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 找数组中第K大的元素
 * 使用最小堆维护最大的K个元素
 */
int find_kth_largest(int* nums, int numsSize, int k) {
    // 维护大小为k的最小堆
    PriorityQueue pq;
    pq_init(&pq);

    for (int i = 0; i < numsSize; i++) {
        if (pq.size < (size_t)k) {
            pq_enqueue(&pq, nums[i], nums[i]);
        } else if (nums[i] > pq.priorities[0]) {
            // 当前元素比堆顶大，替换
            pq.data[0] = nums[i];
            pq.priorities[0] = nums[i];
            pq_sink(&pq, 0);
        }
    }

    int result = pq.priorities[0];
    pq_destroy(&pq);
    return result;
}
