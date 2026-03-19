/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\04_Dynamic_Programming_Basics.md
 * Line: 1145
 * Language: c
 * Block ID: ff090bfe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 记忆化搜索
 * 文件名: memoization.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ==================== 记忆化搜索示例 ====================

/**
 * @brief 带障碍物的不同路径数
 * @param obstacleGrid 1表示障碍物，0表示空地
 * @param m 行数
 * @param n 列数
 */
int unique_paths_with_obstacles_memo(int **grid, int m, int n,
                                      int **memo, int i, int j) {
    // 越界或障碍物
    if (i >= m || j >= n || grid[i][j] == 1) {
        return 0;
    }

    // 到达终点
    if (i == m - 1 && j == n - 1) {
        return 1;
    }

    // 检查缓存
    if (memo[i][j] != -1) {
        return memo[i][j];
    }

    // 计算并缓存
    memo[i][j] = unique_paths_with_obstacles_memo(grid, m, n, memo, i + 1, j) +
                 unique_paths_with_obstacles_memo(grid, m, n, memo, i, j + 1);

    return memo[i][j];
}

int unique_paths_with_obstacles(int **grid, int m, int n) {
    // 初始化记忆化数组
    int **memo = malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++) {
        memo[i] = malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) {
            memo[i][j] = -1;
        }
    }

    int result = unique_paths_with_obstacles_memo(grid, m, n, memo, 0, 0);

    for (int i = 0; i < m; i++) {
        free(memo[i]);
    }
    free(memo);

    return result;
}

/**
 * @brief 相同的递推DP实现
 */
int unique_paths_with_obstacles_dp(int **grid, int m, int n) {
    // dp[i][j] = 到达(i,j)的路径数
    int **dp = malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++) {
        dp[i] = calloc(n, sizeof(int));
    }

    // 初始化起点
    dp[0][0] = (grid[0][0] == 0) ? 1 : 0;

    // 初始化第一行
    for (int j = 1; j < n; j++) {
        dp[0][j] = (grid[0][j] == 0 && dp[0][j - 1] == 1) ? 1 : 0;
    }

    // 初始化第一列
    for (int i = 1; i < m; i++) {
        dp[i][0] = (grid[i][0] == 0 && dp[i - 1][0] == 1) ? 1 : 0;
    }

    // 填充DP表
    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            if (grid[i][j] == 0) {
                dp[i][j] = dp[i - 1][j] + dp[i][j - 1];
            }
        }
    }

    int result = dp[m - 1][n - 1];

    for (int i = 0; i < m; i++) {
        free(dp[i]);
    }
    free(dp);

    return result;
}

/**
 * @brief 记忆化搜索：目标和问题
 * @param nums 正整数数组
 * @param n 数组长度
 * @param target 目标和
 * @return 添加+或-使表达式等于target的方法数
 */
int find_target_sum_ways_memo(int *nums, int n, int target,
                               int **memo, int idx, int current_sum) {
    if (idx == n) {
        return (current_sum == target) ? 1 : 0;
    }

    // 偏移处理负数索引
    int memo_idx = current_sum + 1000;
    if (memo[idx][memo_idx] != INT_MIN) {
        return memo[idx][memo_idx];
    }

    // 加号或减号
    int add = find_target_sum_ways_memo(nums, n, target, memo,
                                         idx + 1, current_sum + nums[idx]);
    int subtract = find_target_sum_ways_memo(nums, n, target, memo,
                                              idx + 1, current_sum - nums[idx]);

    memo[idx][memo_idx] = add + subtract;
    return memo[idx][memo_idx];
}

int find_target_sum_ways(int *nums, int n, int target) {
    // 假设sum的范围在[-1000, 1000]
    int **memo = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        memo[i] = malloc(2001 * sizeof(int));
        for (int j = 0; j < 2001; j++) {
            memo[i][j] = INT_MIN;
        }
    }

    int result = find_target_sum_ways_memo(nums, n, target, memo, 0, 0);

    for (int i = 0; i < n; i++) {
        free(memo[i]);
    }
    free(memo);

    return result;
}
