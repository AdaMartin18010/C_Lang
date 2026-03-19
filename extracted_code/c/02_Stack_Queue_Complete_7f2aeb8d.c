/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\02_Stack_Queue_Complete.md
 * Line: 1204
 * Language: c
 * Block ID: 7f2aeb8d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 滑动窗口最大值
 * 使用双端队列维护单调递减序列
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(k)
 */
int* max_sliding_window(int* nums, int numsSize, int k, int* returnSize) {
    if (numsSize == 0 || k == 0) {
        *returnSize = 0;
        return nullptr;
    }

    *returnSize = numsSize - k + 1;
    int* result = (int*)malloc(*returnSize * sizeof(int));

    // 双端队列存储索引，对应的值单调递减
    Deque deque;
    deque_init(&deque);

    for (int i = 0; i < numsSize; i++) {
        // 移除窗口外的元素
        if (!deque_is_empty(&deque)) {
            int front_idx;
            deque_front(&deque, &front_idx);
            if (front_idx <= i - k) {
                deque_pop_front(&deque, &front_idx);
            }
        }

        // 保持单调递减: 移除所有小于当前元素的
        while (!deque_is_empty(&deque)) {
            int back_idx;
            deque_back(&deque, &back_idx);
            if (nums[back_idx] < nums[i]) {
                deque_pop_back(&deque, &back_idx);
            } else {
                break;
            }
        }

        deque_push_back(&deque, i);

        // 记录窗口最大值
        if (i >= k - 1) {
            int max_idx;
            deque_front(&deque, &max_idx);
            result[i - k + 1] = nums[max_idx];
        }
    }

    deque_destroy(&deque);
    return result;
}
