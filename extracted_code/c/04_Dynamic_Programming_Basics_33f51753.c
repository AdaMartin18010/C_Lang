/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\04_Dynamic_Programming_Basics.md
 * Line: 447
 * Language: c
 * Block ID: 33f51753
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 最大子数组和 - 基础DP
 */
int max_subarray_sum(const int nums[], int n) {
    if (n == 0) return 0;

    int *dp = malloc(n * sizeof(int));
    dp[0] = nums[0];
    int max_sum = dp[0];

    for (int i = 1; i < n; i++) {
        // 状态转移：要么重新开始，要么延续
        dp[i] = nums[i] > dp[i - 1] + nums[i] ?
                nums[i] : dp[i - 1] + nums[i];

        if (dp[i] > max_sum) {
            max_sum = dp[i];
        }
    }

    free(dp);
    return max_sum;
}

/**
 * @brief 最大子数组和 - Kadane算法（空间优化）
 */
int max_subarray_kadane(const int nums[], int n) {
    if (n == 0) return 0;

    int current_sum = nums[0];
    int max_sum = nums[0];

    for (int i = 1; i < n; i++) {
        // 如果当前和为负，从当前元素重新开始
        current_sum = (current_sum > 0) ? current_sum + nums[i] : nums[i];

        if (current_sum > max_sum) {
            max_sum = current_sum;
        }
    }

    return max_sum;
}

/**
 * @brief 最大子数组和 - 同时返回子数组位置
 */
typedef struct {
    int sum;
    int start;
    int end;
} MaxSubarrayResult;

MaxSubarrayResult max_subarray_with_indices(const int nums[], int n) {
    MaxSubarrayResult result = {nums[0], 0, 0};

    int current_sum = nums[0];
    int current_start = 0;

    for (int i = 1; i < n; i++) {
        if (current_sum < 0) {
            current_sum = nums[i];
            current_start = i;
        } else {
            current_sum += nums[i];
        }

        if (current_sum > result.sum) {
            result.sum = current_sum;
            result.start = current_start;
            result.end = i;
        }
    }

    return result;
}

/**
 * @brief 环形子数组的最大和
 * @note 最大子数组可能在环形连接处
 */
int max_subarray_circular(const int nums[], int n) {
    // 情况1：最大子数组不跨边界 - 普通Kadane
    int max_normal = max_subarray_kadane(nums, n);

    // 情况2：最大子数组跨边界
    // 等价于：总和 - 最小子数组和
    int total_sum = 0;
    for (int i = 0; i < n; i++) {
        total_sum += nums[i];
    }

    // 取反求最大子数组和（即原数组最小子数组和的相反数）
    int *inverted = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        inverted[i] = -nums[i];
    }
    int max_inverted = max_subarray_kadane(inverted, n);
    int min_sum = -max_inverted;
    free(inverted);

    // 特殊情况：如果所有数都是负数
    if (min_sum == total_sum) return max_normal;

    int max_circular = total_sum - min_sum;

    return max_normal > max_circular ? max_normal : max_circular;
}
