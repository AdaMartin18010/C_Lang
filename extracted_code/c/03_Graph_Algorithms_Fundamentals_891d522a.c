/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\03_Graph_Algorithms_Fundamentals.md
 * Line: 1150
 * Language: c
 * Block ID: 891d522a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief Bellman-Ford算法 - 可处理负权边
 * @return 是否存在负权环
 */
bool bellman_ford(int graph[MAX_VERTICES][MAX_VERTICES],
                  int n, int start, int dist[], int parent[]) {
    // 初始化
    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }
    dist[start] = 0;

    // 松弛所有边 V-1 次
    for (int i = 0; i < n - 1; i++) {
        bool updated = false;
        for (int u = 0; u < n; u++) {
            for (int v = 0; v < n; v++) {
                if (graph[u][v] != INF && dist[u] != INF &&
                    dist[u] + graph[u][v] < dist[v]) {
                    dist[v] = dist[u] + graph[u][v];
                    parent[v] = u;
                    updated = true;
                }
            }
        }
        if (!updated) break;  // 提前终止
    }

    // 检查负权环
    for (int u = 0; u < n; u++) {
        for (int v = 0; v < n; v++) {
            if (graph[u][v] != INF && dist[u] != INF &&
                dist[u] + graph[u][v] < dist[v]) {
                return false;  // 存在负权环
            }
        }
    }

    return true;
}

/**
 * @brief SPFA算法 - Bellman-Ford的队列优化
 */
bool spfa(int graph[MAX_VERTICES][MAX_VERTICES],
          int n, int start, int dist[], int parent[]) {
    bool in_queue[MAX_VERTICES] = {false};
    int count[MAX_VERTICES] = {0};  // 记录入队次数

    // 简单队列
    int queue[MAX_VERTICES];
    int front = 0, rear = 0;

    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }
    dist[start] = 0;

    queue[rear++] = start;
    in_queue[start] = true;
    count[start]++;

    while (front != rear) {
        int u = queue[front++];
        in_queue[u] = false;

        for (int v = 0; v < n; v++) {
            if (graph[u][v] != INF && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;

                if (!in_queue[v]) {
                    queue[rear++] = v;
                    in_queue[v] = true;
                    count[v]++;

                    if (count[v] > n) {
                        return false;  // 存在负权环
                    }
                }
            }
        }
    }

    return true;
}
