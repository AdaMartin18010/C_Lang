/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\04_Dynamic_Programming_Basics.md
 * Line: 736
 * Language: c
 * Block ID: 25b21b44
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 背包问题
 * 文件名: knapsack.c
 * 编译: gcc -std=c23 -o knapsack knapsack.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_N 1000
#define MAX_C 10000

/**
 * @brief 0/1背包 - 二维DP O(n*C)
 */
int knapsack_01_2d(const int w[], const int v[], int n, int C) {
    // dp[i][j] 表示前i个物品，容量j的最大价值
    int **dp = malloc((n + 1) * sizeof(int *));
    for (int i = 0; i <= n; i++) {
        dp[i] = calloc(C + 1, sizeof(int));
    }

    for (int i = 1; i <= n; i++) {
        for (int j = 0; j <= C; j++) {
            // 不选第i个物品
            dp[i][j] = dp[i - 1][j];

            // 选第i个物品
            if (j >= w[i - 1]) {
                int with_item = dp[i - 1][j - w[i - 1]] + v[i - 1];
                dp[i][j] = dp[i][j] > with_item ? dp[i][j] : with_item;
            }
        }
    }

    int result = dp[n][C];

    for (int i = 0; i <= n; i++) {
        free(dp[i]);
    }
    free(dp);

    return result;
}

/**
 * @brief 0/1背包 - 一维DP（空间优化）O(n*C)
 * @note 倒序遍历容量，避免覆盖
 */
int knapsack_01_1d(const int w[], const int v[], int n, int C) {
    int *dp = calloc(C + 1, sizeof(int));

    for (int i = 0; i < n; i++) {
        // 倒序遍历，防止重复选择
        for (int j = C; j >= w[i]; j--) {
            int with_item = dp[j - w[i]] + v[i];
            dp[j] = dp[j] > with_item ? dp[j] : with_item;
        }
    }

    int result = dp[C];
    free(dp);
    return result;
}

/**
 * @brief 0/1背包 - 记录选择的物品
 */
int knapsack_01_with_items(const int w[], const int v[], int n, int C,
                            int selected[]) {
    int **dp = malloc((n + 1) * sizeof(int *));
    for (int i = 0; i <= n; i++) {
        dp[i] = calloc(C + 1, sizeof(int));
    }

    // 填充DP表
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j <= C; j++) {
            dp[i][j] = dp[i - 1][j];
            if (j >= w[i - 1]) {
                int with_item = dp[i - 1][j - w[i - 1]] + v[i - 1];
                dp[i][j] = dp[i][j] > with_item ? dp[i][j] : with_item;
            }
        }
    }

    // 回溯找出选择的物品
    int j = C;
    int item_count = 0;
    for (int i = n; i > 0 && j > 0; i--) {
        if (dp[i][j] != dp[i - 1][j]) {
            selected[item_count++] = i - 1;  // 选择了第i-1个物品
            j -= w[i - 1];
        }
    }

    int result = dp[n][C];

    for (int i = 0; i <= n; i++) {
        free(dp[i]);
    }
    free(dp);

    return item_count;  // 返回选择的物品数量
}

/**
 * @brief 完全背包 - 每种物品有无限个
 * @note 正序遍历容量
 */
int knapsack_complete(const int w[], const int v[], int n, int C) {
    int *dp = calloc(C + 1, sizeof(int));

    for (int i = 0; i < n; i++) {
        // 正序遍历，允许重复选择
        for (int j = w[i]; j <= C; j++) {
            int with_item = dp[j - w[i]] + v[i];
            dp[j] = dp[j] > with_item ? dp[j] : with_item;
        }
    }

    int result = dp[C];
    free(dp);
    return result;
}

/**
 * @brief 多重背包 - 每种物品有数量限制
 * @param count count[i]表示第i种物品的数量
 */
int knapsack_multiple(const int w[], const int v[], const int count[],
                      int n, int C) {
    int *dp = calloc(C + 1, sizeof(int));

    for (int i = 0; i < n; i++) {
        // 二进制优化：将k个物品拆分为1, 2, 4, ..., k-2^m+1
        int k = count[i];
        int binary = 1;

        while (k > 0) {
            int use = (k < binary) ? k : binary;
            int weight = w[i] * use;
            int value = v[i] * use;

            // 0/1背包处理这一批
            for (int j = C; j >= weight; j--) {
                int with_item = dp[j - weight] + value;
                dp[j] = dp[j] > with_item ? dp[j] : with_item;
            }

            k -= use;
            binary *= 2;
        }
    }

    int result = dp[C];
    free(dp);
    return result;
}

// 主函数
int main(void) {
    int w[] = {2, 3, 4, 5};      // 重量
    int v[] = {3, 4, 5, 6};      // 价值
    int n = 4;
    int C = 8;                    // 背包容量

    printf("=== 背包问题演示 ===\n\n");
    printf("物品: 重量%v, 价值%v\n");
    for (int i = 0; i < n; i++) {
        printf("  物品%d: w=%d, v=%d\n", i, w[i], v[i]);
    }
    printf("背包容量: %d\n\n", C);

    printf("0/1背包(二维DP): 最大价值 = %d\n",
           knapsack_01_2d(w, v, n, C));
    printf("0/1背包(一维DP): 最大价值 = %d\n",
           knapsack_01_1d(w, v, n, C));

    int selected[10];
    int count = knapsack_01_with_items(w, v, n, C, selected);
    printf("\n选择的物品: ");
    for (int i = 0; i < count; i++) {
        printf("%d ", selected[i]);
    }
    printf("\n");

    printf("\n完全背包: 最大价值 = %d\n",
           knapsack_complete(w, v, n, C));

    int cnt[] = {2, 1, 3, 2};  // 每种物品的数量限制
    printf("多重背包: 最大价值 = %d\n",
           knapsack_multiple(w, v, cnt, n, C));

    return 0;
}
