/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\04_Dynamic_Programming_Basics.md
 * Line: 1305
 * Language: c
 * Block ID: 15037366
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 斐波那契 - 滚动数组优化
 * 将O(n)空间优化到O(1)
 */
unsigned long long fib_rolling(int n) {
    if (n <= 1) return n;

    unsigned long long a = 0, b = 1, c;
    for (int i = 2; i <= n; i++) {
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}

/**
 * @brief 二维DP滚动优化示例
 * 如果dp[i][j]只依赖上一行，可以用两行交替
 */
int dp_2d_rolling(int n, int m) {
    int *prev = calloc(m, sizeof(int));
    int *curr = calloc(m, sizeof(int));

    // 初始化第一行
    for (int j = 0; j < m; j++) {
        prev[j] = j;  // 示例初始化
    }

    for (int i = 1; i < n; i++) {
        for (int j = 0; j < m; j++) {
            // 使用prev计算curr
            curr[j] = prev[j] + (j > 0 ? curr[j - 1] : 0);
        }
        // 交换
        int *temp = prev;
        prev = curr;
        curr = temp;
    }

    int result = prev[m - 1];
    free(prev);
    free(curr);
    return result;
}
