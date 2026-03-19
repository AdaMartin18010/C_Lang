/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\02_Stack_Queue_Complete.md
 * Line: 869
 * Language: c
 * Block ID: a5387c20
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * BFS 广度优先搜索
 * 使用队列实现层次遍历
 */
typedef struct {
    int row;
    int col;
} Position;

// 四个方向
const int dr[] = {-1, 1, 0, 0};
const int dc[] = {0, 0, -1, 1};

/**
 * 迷宫最短路径 - BFS
 * @param maze 迷宫 (0=通路, 1=墙)
 * @param rows 行数
 * @param cols 列数
 * @param start 起点
 * @param end 终点
 * @return 最短路径长度，不可达返回-1
 */
int bfs_shortest_path(int** maze, int rows, int cols,
                      Position start, Position end) {
    if (maze[start.row][start.col] == 1 || maze[end.row][end.col] == 1) {
        return -1;
    }

    // 访问标记
    bool** visited = (bool**)calloc(rows, sizeof(bool*));
    for (int i = 0; i < rows; i++) {
        visited[i] = (bool*)calloc(cols, sizeof(bool));
    }

    // BFS队列
    LinkedQueue queue;
    linked_queue_init(&queue);

    linked_queue_enqueue(&queue, start.row * cols + start.col);
    visited[start.row][start.col] = true;

    int distance = 0;

    while (!linked_queue_is_empty(&queue)) {
        int level_size = linked_queue_size(&queue);

        for (int i = 0; i < level_size; i++) {
            int encoded;
            linked_queue_dequeue(&queue, &encoded);
            int r = encoded / cols;
            int c = encoded % cols;

            if (r == end.row && c == end.col) {
                // 到达终点
                linked_queue_destroy(&queue);
                for (int j = 0; j < rows; j++) free(visited[j]);
                free(visited);
                return distance;
            }

            // 探索四个方向
            for (int d = 0; d < 4; d++) {
                int nr = r + dr[d];
                int nc = c + dc[d];

                if (nr >= 0 && nr < rows && nc >= 0 && nc < cols &&
                    !visited[nr][nc] && maze[nr][nc] == 0) {
                    visited[nr][nc] = true;
                    linked_queue_enqueue(&queue, nr * cols + nc);
                }
            }
        }
        distance++;
    }

    // 不可达
    linked_queue_destroy(&queue);
    for (int i = 0; i < rows; i++) free(visited[i]);
    free(visited);
    return -1;
}
