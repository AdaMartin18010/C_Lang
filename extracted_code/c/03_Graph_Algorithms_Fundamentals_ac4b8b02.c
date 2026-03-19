/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\03_Graph_Algorithms_Fundamentals.md
 * Line: 1823
 * Language: c
 * Block ID: ac4b8b02
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 计算共同好友数
 */
int common_friends(int graph[MAX_VERTICES][MAX_VERTICES],
                   int n, int u, int v) {
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (graph[u][i] && graph[v][i]) {
            count++;
        }
    }
    return count;
}

/**
 * @brief 推荐好友（基于共同好友数）
 */
void recommend_friends(int graph[MAX_VERTICES][MAX_VERTICES],
                       int n, int user, int recommendations[],
                       int *count) {
    int common_count[MAX_VERTICES] = {0};

    for (int i = 0; i < n; i++) {
        if (i != user && !graph[user][i]) {
            common_count[i] = common_friends(graph, n, user, i);
        }
    }

    // 排序推荐（简化版）
    *count = 0;
    for (int i = 0; i < n && *count < 10; i++) {
        if (common_count[i] > 0) {
            recommendations[(*count)++] = i;
        }
    }
}
