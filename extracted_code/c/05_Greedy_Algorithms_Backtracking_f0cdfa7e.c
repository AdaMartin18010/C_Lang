/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\05_Greedy_Algorithms_Backtracking.md
 * Line: 1079
 * Language: c
 * Block ID: f0cdfa7e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 迷宫求解 - 回溯算法
 * @param maze 迷宫，0表示通路，1表示墙
 * @param m, n 迷宫大小
 * @param path 输出路径
 * @return 是否找到路径
 */
bool solve_maze(int maze[][100], int m, int n,
                int start_x, int start_y,
                int end_x, int end_y,
                bool visited[][100], int path[][2], int *path_len) {
    // 到达终点
    if (start_x == end_x && start_y == end_y) {
        path[*path_len][0] = start_x;
        path[*path_len][1] = start_y;
        (*path_len)++;
        return true;
    }

    // 检查边界和有效性
    if (start_x < 0 || start_x >= m || start_y < 0 || start_y >= n ||
        maze[start_x][start_y] == 1 || visited[start_x][start_y]) {
        return false;
    }

    // 标记访问
    visited[start_x][start_y] = true;
    path[*path_len][0] = start_x;
    path[*path_len][1] = start_y;
    (*path_len)++;

    // 四个方向搜索
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    for (int i = 0; i < 4; i++) {
        int new_x = start_x + dx[i];
        int new_y = start_y + dy[i];

        if (solve_maze(maze, m, n, new_x, new_y, end_x, end_y,
                       visited, path, path_len)) {
            return true;
        }
    }

    // 回溯
    (*path_len)--;
    visited[start_x][start_y] = false;
    return false;
}
