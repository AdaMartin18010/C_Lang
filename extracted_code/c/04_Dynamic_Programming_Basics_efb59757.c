/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\04_Dynamic_Programming_Basics.md
 * Line: 1033
 * Language: c
 * Block ID: efb59757
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 编辑距离（Levenshtein距离）
 * @note 将一个字符串转换成另一个字符串的最少操作数
 *       操作：插入、删除、替换
 */
int min_distance(const char *word1, const char *word2) {
    int m = (int)strlen(word1);
    int n = (int)strlen(word2);

    int **dp = malloc((m + 1) * sizeof(int *));
    for (int i = 0; i <= m; i++) {
        dp[i] = malloc((n + 1) * sizeof(int));
        dp[i][0] = i;  // word1前i个字符变成空串需要i次删除
    }
    for (int j = 0; j <= n; j++) {
        dp[0][j] = j;  // 空串变成word2前j个字符需要j次插入
    }

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (word1[i - 1] == word2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];  // 字符相同，无需操作
            } else {
                int insert_op = dp[i][j - 1] + 1;    // 插入
                int delete_op = dp[i - 1][j] + 1;    // 删除
                int replace_op = dp[i - 1][j - 1] + 1; // 替换

                dp[i][j] = insert_op;
                if (delete_op < dp[i][j]) dp[i][j] = delete_op;
                if (replace_op < dp[i][j]) dp[i][j] = replace_op;
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
