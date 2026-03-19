/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\03_Graph_Algorithms_Fundamentals.md
 * Line: 1243
 * Language: c
 * Block ID: 8a373f62
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief Floyd-Warshall算法 - 全源最短路径
 * @param dist 距离矩阵，同时也是输出
 */
void floyd_warshall(int dist[MAX_VERTICES][MAX_VERTICES],
                    int next[MAX_VERTICES][MAX_VERTICES], int n) {
    // 初始化next矩阵用于路径重建
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (dist[i][j] != INF && i != j) {
                next[i][j] = j;
            } else {
                next[i][j] = -1;
            }
        }
    }

    // 动态规划
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (dist[i][k] != INF && dist[k][j] != INF &&
                    dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }
}

void print_floyd_path(int next[MAX_VERTICES][MAX_VERTICES],
                      int start, int end, const char names[]) {
    if (next[start][end] == -1) {
        printf("无路径\n");
        return;
    }

    printf("%c ", names[start]);
    int curr = start;
    while (curr != end) {
        curr = next[curr][end];
        printf("-> %c ", names[curr]);
    }
    printf("\n");
}

// 主函数
int main(void) {
    int n = 5;
    int graph[MAX_VERTICES][MAX_VERTICES];
    char names[] = {'A', 'B', 'C', 'D', 'E'};

    // 初始化
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            graph[i][j] = (i == j) ? 0 : INF;
        }
    }

    // 添加边
    graph[0][1] = 4;  // A-B: 4
    graph[0][2] = 2;  // A-C: 2
    graph[1][2] = 1;  // B-C: 1
    graph[1][3] = 5;  // B-D: 5
    graph[2][3] = 8;  // C-D: 8
    graph[2][4] = 10; // C-E: 10
    graph[3][4] = 2;  // D-E: 2

    // 无向图
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (graph[i][j] != INF) {
                graph[j][i] = graph[i][j];
            }
        }
    }

    printf("=== 最短路径算法演示 ===\n\n");

    int dist[MAX_VERTICES];
    int parent[MAX_VERTICES];

    printf("Dijkstra算法 (基础):\n");
    dijkstra_basic(graph, n, 0, dist, parent);
    print_shortest_paths(dist, parent, n, 0, names);

    printf("\nDijkstra算法 (堆优化):\n");
    dijkstra_heap(graph, n, 0, dist, parent);
    print_shortest_paths(dist, parent, n, 0, names);

    printf("\nFloyd-Warshall算法:\n");
    int fw_dist[MAX_VERTICES][MAX_VERTICES];
    int next[MAX_VERTICES][MAX_VERTICES];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fw_dist[i][j] = graph[i][j];
        }
    }
    floyd_warshall(fw_dist, next, n);

    printf("全源最短路径矩阵:\n");
    printf("    A   B   C   D   E\n");
    for (int i = 0; i < n; i++) {
        printf(" %c ", names[i]);
        for (int j = 0; j < n; j++) {
            if (fw_dist[i][j] == INF) {
                printf("  ∞ ");
            } else {
                printf("%3d ", fw_dist[i][j]);
            }
        }
        printf("\n");
    }

    return 0;
}
