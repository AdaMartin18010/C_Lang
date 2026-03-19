/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\05_Greedy_Algorithms_Backtracking.md
 * Line: 808
 * Language: c
 * Block ID: 5c84be42
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ==================== 子集问题 ====================

/**
 * @brief 生成所有子集
 */
void backtrack_subsets(int nums[], int n, int current[], int current_size,
                       int **result, int *result_count, int max_results) {
    if (*result_count >= max_results) return;

    // 记录当前子集
    memcpy(result[*result_count], current, current_size * sizeof(int));
    (*result_count)++;

    // 从start开始选择元素
    for (int i = current_size; i < n; i++) {
        current[current_size] = nums[i];
        backtrack_subsets(nums, n, current, current_size + 1,
                          result, result_count, max_results);
        // 回溯（不需要显式撤销）
    }
}

/**
 * @brief 子集II - 包含重复元素
 */
void backtrack_subsets_unique(int nums[], int n, int start,
                              int current[], int current_size,
                              int **result, int *result_count,
                              int max_results) {
    if (*result_count >= max_results) return;

    memcpy(result[*result_count], current, current_size * sizeof(int));
    (*result_count)++;

    for (int i = start; i < n; i++) {
        // 剪枝：跳过重复元素
        if (i > start && nums[i] == nums[i - 1]) continue;

        current[current_size] = nums[i];
        backtrack_subsets_unique(nums, n, i + 1, current, current_size + 1,
                                  result, result_count, max_results);
    }
}

/**
 * @brief 组合总和
 * @note 找出所有和为target的组合，每个数可重复使用
 */
void backtrack_combination_sum(int candidates[], int n, int target,
                               int current[], int current_size,
                               int start, int **result, int *result_count,
                               int max_results) {
    if (*result_count >= max_results) return;

    if (target == 0) {
        memcpy(result[*result_count], current, current_size * sizeof(int));
        (*result_count)++;
        return;
    }

    if (target < 0) return;

    for (int i = start; i < n; i++) {
        current[current_size] = candidates[i];
        backtrack_combination_sum(candidates, n, target - candidates[i],
                                  current, current_size + 1, i,
                                  result, result_count, max_results);
        // 回溯
    }
}
