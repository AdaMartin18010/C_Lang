/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\05_Greedy_Algorithms_Backtracking.md
 * Line: 1177
 * Language: c
 * Block ID: a9267861
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 任务分配问题 - 回溯（小规模）
 * @param cost cost[i][j] = 人员i完成任务j的成本
 * @param n 任务/人员数量
 * @return 最小总成本
 */
int assignment_problem(int cost[][20], int n) {
    bool assigned[20] = {false};
    int min_cost = INT_MAX;

    void backtrack_assign(int person, int current_cost) {
        if (person == n) {
            if (current_cost < min_cost) {
                min_cost = current_cost;
            }
            return;
        }

        // 剪枝
        if (current_cost >= min_cost) return;

        for (int task = 0; task < n; task++) {
            if (!assigned[task]) {
                assigned[task] = true;
                backtrack_assign(person + 1, current_cost + cost[person][task]);
                assigned[task] = false;
            }
        }
    }

    backtrack_assign(0, 0);
    return min_cost;
}
