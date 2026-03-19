/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\05_Greedy_Algorithms_Backtracking.md
 * Line: 1134
 * Language: c
 * Block ID: 747760b7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 旅行商问题 - 回溯+分支限界
 * @param graph 邻接矩阵
 * @param n 城市数
 * @return 最短回路长度
 */
int tsp_backtrack(int graph[][20], int n) {
    bool visited[20] = {false};
    int min_cost = INT_MAX;
    int current_path[20];

    void backtrack_tsp(int curr_pos, int count, int cost) {
        if (count == n && graph[curr_pos][0]) {
            // 回到起点
            int total_cost = cost + graph[curr_pos][0];
            if (total_cost < min_cost) {
                min_cost = total_cost;
            }
            return;
        }

        // 剪枝
        if (cost >= min_cost) return;

        for (int i = 0; i < n; i++) {
            if (!visited[i] && graph[curr_pos][i]) {
                visited[i] = true;
                backtrack_tsp(i, count + 1, cost + graph[curr_pos][i]);
                visited[i] = false;
            }
        }
    }

    visited[0] = true;  // 从城市0开始
    backtrack_tsp(0, 1, 0);

    return min_cost;
}
