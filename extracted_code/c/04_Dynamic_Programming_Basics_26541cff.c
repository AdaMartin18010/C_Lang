/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\04_Dynamic_Programming_Basics.md
 * Line: 1081
 * Language: c
 * Block ID: 26541cff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 矩阵链乘法 - 最优计算顺序
 * @param dims dims[i]表示第i个矩阵的行数，也是第i-1个矩阵的列数
 * @param n 矩阵个数
 * @return 最少乘法次数
 */
int matrix_chain_multiplication(const int dims[], int n) {
    // dp[i][j] = 计算矩阵i到j的最少乘法次数
    int **dp = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        dp[i] = calloc(n, sizeof(int));
    }

    // len是链的长度
    for (int len = 2; len < n; len++) {
        for (int i = 1; i < n - len + 1; i++) {
            int j = i + len - 1;
            dp[i][j] = INT_MAX;

            // 尝试所有分割点
            for (int k = i; k < j; k++) {
                int cost = dp[i][k] + dp[k + 1][j] +
                          dims[i - 1] * dims[k] * dims[j];
                if (cost < dp[i][j]) {
                    dp[i][j] = cost;
                }
            }
        }
    }

    int result = dp[1][n - 1];

    for (int i = 0; i < n; i++) {
        free(dp[i]);
    }
    free(dp);

    return result;
}
