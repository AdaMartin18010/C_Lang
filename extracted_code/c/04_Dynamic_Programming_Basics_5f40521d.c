/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\04_Dynamic_Programming_Basics.md
 * Line: 1381
 * Language: c
 * Block ID: 5f40521d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 买卖股票的最佳时机 - 只能买卖一次
 */
int max_profit_once(const int prices[], int n) {
    if (n < 2) return 0;

    int min_price = prices[0];
    int max_profit = 0;

    for (int i = 1; i < n; i++) {
        // 更新最大利润
        int profit = prices[i] - min_price;
        if (profit > max_profit) {
            max_profit = profit;
        }
        // 更新最小价格
        if (prices[i] < min_price) {
            min_price = prices[i];
        }
    }

    return max_profit;
}

/**
 * @brief 买卖股票的最佳时机 - 可以买卖多次
 */
int max_profit_multiple(const int prices[], int n) {
    int total_profit = 0;

    for (int i = 1; i < n; i++) {
        // 只要今天比昨天高，就计入利润
        if (prices[i] > prices[i - 1]) {
            total_profit += prices[i] - prices[i - 1];
        }
    }

    return total_profit;
}

/**
 * @brief 买卖股票的最佳时机 - 最多k次交易
 */
int max_profit_k_transactions(const int prices[], int n, int k) {
    if (n < 2 || k == 0) return 0;

    // 如果k很大，等价于无限次交易
    if (k >= n / 2) {
        return max_profit_multiple(prices, n);
    }

    // dp[i][j] = 最多i次交易，第j天结束时的最大利润
    // dp[i][j] = max(dp[i][j-1], prices[j] + max(dp[i-1][t] - prices[t]))

    int **dp = malloc((k + 1) * sizeof(int *));
    for (int i = 0; i <= k; i++) {
        dp[i] = calloc(n, sizeof(int));
    }

    for (int i = 1; i <= k; i++) {
        int max_diff = -prices[0];  // max(dp[i-1][t] - prices[t])
        for (int j = 1; j < n; j++) {
            dp[i][j] = dp[i][j - 1] > prices[j] + max_diff ?
                       dp[i][j - 1] : prices[j] + max_diff;
            max_diff = max_diff > dp[i - 1][j] - prices[j] ?
                       max_diff : dp[i - 1][j] - prices[j];
        }
    }

    int result = dp[k][n - 1];

    for (int i = 0; i <= k; i++) {
        free(dp[i]);
    }
    free(dp);

    return result;
}
