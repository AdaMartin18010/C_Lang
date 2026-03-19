/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\04_Dynamic_Programming_Basics.md
 * Line: 363
 * Language: c
 * Block ID: 76366895
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 爬楼梯问题
 * @param n 台阶数
 * @return 方法数
 */
unsigned long long climb_stairs(int n) {
    if (n <= 2) return n;

    unsigned long long prev2 = 1;  // dp[i-2]
    unsigned long long prev1 = 2;  // dp[i-1]
    unsigned long long current;

    for (int i = 3; i <= n; i++) {
        current = prev1 + prev2;
        prev2 = prev1;
        prev1 = current;
    }

    return prev1;
}

/**
 * @brief 扩展：每次可以爬1~k级台阶
 */
unsigned long long climb_stairs_k(int n, int k) {
    if (n <= 1) return 1;

    unsigned long long *dp = calloc(n + 1, sizeof(unsigned long long));
    dp[0] = 1;  // 地面有1种方法（不动）

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= k && j <= i; j++) {
            dp[i] += dp[i - j];
        }
    }

    unsigned long long result = dp[n];
    free(dp);
    return result;
}

/**
 * @brief 带耗费的爬楼梯 - 最小耗费
 * @param cost cost[i]表示在第i级台阶耗费的体力
 * @return 到达顶部的最小耗费
 */
int min_cost_climbing_stairs(const int cost[], int n) {
    // dp[i] = 到达第i级的最小耗费
    int prev2 = cost[0];
    int prev1 = cost[1];

    for (int i = 2; i < n; i++) {
        int current = cost[i] + (prev1 < prev2 ? prev1 : prev2);
        prev2 = prev1;
        prev1 = current;
    }

    // 最后一步可以从n-1或n-2跨过去
    return prev1 < prev2 ? prev1 : prev2;
}
