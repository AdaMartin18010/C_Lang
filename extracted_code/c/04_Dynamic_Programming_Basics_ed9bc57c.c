/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\04_Dynamic_Programming_Basics.md
 * Line: 1464
 * Language: c
 * Block ID: ed9bc57c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 正则表达式匹配
 * @param s 字符串
 * @param p 模式，支持.和*
 * @return 是否匹配
 */
bool is_match(const char *s, const char *p) {
    int m = (int)strlen(s);
    int n = (int)strlen(p);

    // dp[i][j] = s[0..i-1]与p[0..j-1]是否匹配
    bool **dp = malloc((m + 1) * sizeof(bool *));
    for (int i = 0; i <= m; i++) {
        dp[i] = calloc(n + 1, sizeof(bool));
    }

    dp[0][0] = true;  // 空串匹配空模式

    // 初始化：s为空，p为a*b*c*这种情况
    for (int j = 2; j <= n; j++) {
        if (p[j - 1] == '*') {
            dp[0][j] = dp[0][j - 2];
        }
    }

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (p[j - 1] == '.' || p[j - 1] == s[i - 1]) {
                // 直接匹配
                dp[i][j] = dp[i - 1][j - 1];
            } else if (p[j - 1] == '*') {
                // *匹配0次或多次
                // 0次：dp[i][j-2]
                // 多次：dp[i-1][j]（如果前一个字符匹配）
                dp[i][j] = dp[i][j - 2];
                if (p[j - 2] == '.' || p[j - 2] == s[i - 1]) {
                    dp[i][j] = dp[i][j] || dp[i - 1][j];
                }
            }
        }
    }

    bool result = dp[m][n];

    for (int i = 0; i <= m; i++) {
        free(dp[i]);
    }
    free(dp);

    return result;
}
