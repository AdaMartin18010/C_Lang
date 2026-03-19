/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\04_Dynamic_Programming_Basics.md
 * Line: 964
 * Language: c
 * Block ID: 0d802d54
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 最长公共子序列
 */
int longest_common_subsequence(const char *text1, const char *text2) {
    int m = (int)strlen(text1);
    int n = (int)strlen(text2);

    // dp[i][j]表示text1[0..i-1]和text2[0..j-1]的LCS长度
    int **dp = malloc((m + 1) * sizeof(int *));
    for (int i = 0; i <= m; i++) {
        dp[i] = calloc(n + 1, sizeof(int));
    }

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (text1[i - 1] == text2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                dp[i][j] = dp[i - 1][j] > dp[i][j - 1] ?
                           dp[i - 1][j] : dp[i][j - 1];
            }
        }
    }

    int result = dp[m][n];

    for (int i = 0; i <= m; i++) {
        free(dp[i]);
    }
    free(dp);

    return result;
}

/**
 * @brief LCS - 空间优化
 */
int lcs_optimized(const char *text1, const char *text2) {
    int m = (int)strlen(text1);
    int n = (int)strlen(text2);

    // 只保留两行
    int *prev = calloc(n + 1, sizeof(int));
    int *curr = calloc(n + 1, sizeof(int));

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (text1[i - 1] == text2[j - 1]) {
                curr[j] = prev[j - 1] + 1;
            } else {
                curr[j] = prev[j] > curr[j - 1] ? prev[j] : curr[j - 1];
            }
        }
        // 交换prev和curr
        int *temp = prev;
        prev = curr;
        curr = temp;
    }

    int result = prev[n];
    free(prev);
    free(curr);
    return result;
}
