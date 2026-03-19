/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\05_Greedy_Algorithms_Backtracking.md
 * Line: 591
 * Language: c
 * Block ID: 4d1415c9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 回溯算法经典问题
 * 文件名: backtracking.c
 * 编译: gcc -std=c23 -o backtracking backtracking.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// ==================== 全排列问题 ====================

/**
 * @brief 生成数组的全排列
 * @param nums 输入数组
 * @param n 数组长度
 * @param result 输出所有排列
 * @param result_count 输出排列数量
 * @param max_results 最大输出数量
 */
void backtrack_permute(int nums[], int n, bool used[], int current[],
                       int depth, int **result, int *result_count,
                       int max_results) {
    if (*result_count >= max_results) return;

    // 找到一个完整排列
    if (depth == n) {
        memcpy(result[*result_count], current, n * sizeof(int));
        (*result_count)++;
        return;
    }

    for (int i = 0; i < n; i++) {
        if (used[i]) continue;

        // 做出选择
        used[i] = true;
        current[depth] = nums[i];

        // 递归
        backtrack_permute(nums, n, used, current, depth + 1,
                          result, result_count, max_results);

        // 撤销选择（回溯）
        used[i] = false;
    }
}

int** permute(int nums[], int n, int *returnSize, int max_results) {
    // 计算排列数
    int total = 1;
    for (int i = 2; i <= n; i++) total *= i;
    if (total > max_results) total = max_results;

    // 分配内存
    int **result = malloc(total * sizeof(int *));
    for (int i = 0; i < total; i++) {
        result[i] = malloc(n * sizeof(int));
    }

    bool *used = calloc(n, sizeof(bool));
    int *current = malloc(n * sizeof(int));

    *returnSize = 0;
    backtrack_permute(nums, n, used, current, 0, result, returnSize, max_results);

    free(used);
    free(current);
    return result;
}

/**
 * @brief 全排列II - 包含重复元素
 */
void backtrack_permute_unique(int nums[], int n, bool used[], int current[],
                              int depth, int **result, int *result_count,
                              int max_results) {
    if (*result_count >= max_results) return;

    if (depth == n) {
        memcpy(result[*result_count], current, n * sizeof(int));
        (*result_count)++;
        return;
    }

    for (int i = 0; i < n; i++) {
        if (used[i]) continue;

        // 剪枝：跳过同一层重复的元素
        if (i > 0 && nums[i] == nums[i - 1] && !used[i - 1]) continue;

        used[i] = true;
        current[depth] = nums[i];

        backtrack_permute_unique(nums, n, used, current, depth + 1,
                                  result, result_count, max_results);

        used[i] = false;
    }
}

int** permute_unique(int nums[], int n, int *returnSize, int max_results) {
    // 先排序，方便剪枝
    int compare_int(const void *a, const void *b) {
        return *(int *)a - *(int *)b;
    }
    qsort(nums, n, sizeof(int), compare_int);

    int total = 1;
    for (int i = 2; i <= n; i++) total *= i;
    if (total > max_results) total = max_results;

    int **result = malloc(total * sizeof(int *));
    for (int i = 0; i < total; i++) {
        result[i] = malloc(n * sizeof(int));
    }

    bool *used = calloc(n, sizeof(bool));
    int *current = malloc(n * sizeof(int));

    *returnSize = 0;
    backtrack_permute_unique(nums, n, used, current, 0,
                             result, returnSize, max_results);

    free(used);
    free(current);
    return result;
}
