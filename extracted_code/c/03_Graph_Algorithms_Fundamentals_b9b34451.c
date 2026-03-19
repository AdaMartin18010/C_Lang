/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\03_Graph_Algorithms_Fundamentals.md
 * Line: 1714
 * Language: c
 * Block ID: b9b34451
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 拓扑排序
 * 文件名: topological_sort.c
 */

#include <stdio.h>
#include <stdbool.h>

#define MAX_VERTICES 100

// 计算入度
void calculate_indegree(int graph[MAX_VERTICES][MAX_VERTICES],
                        int n, int indegree[]) {
    for (int i = 0; i < n; i++) {
        indegree[i] = 0;
        for (int j = 0; j < n; j++) {
            if (graph[j][i] != 0 && graph[j][i] != INT_MAX) {
                indegree[i]++;
            }
        }
    }
}

/**
 * @brief Kahn算法 - BFS实现拓扑排序
 * @return 是否存在环（true=无环，false=有环）
 */
bool kahn_topological_sort(int graph[MAX_VERTICES][MAX_VERTICES],
                           int n, int result[]) {
    int indegree[MAX_VERTICES];
    calculate_indegree(graph, n, indegree);

    // 队列
    int queue[MAX_VERTICES];
    int front = 0, rear = 0;

    // 入度为0的顶点入队
    for (int i = 0; i < n; i++) {
        if (indegree[i] == 0) {
            queue[rear++] = i;
        }
    }

    int count = 0;
    while (front != rear) {
        int u = queue[front++];
        result[count++] = u;

        // 减少邻接点的入度
        for (int v = 0; v < n; v++) {
            if (graph[u][v] != 0 && graph[u][v] != INT_MAX) {
                indegree[v]--;
                if (indegree[v] == 0) {
                    queue[rear++] = v;
                }
            }
        }
    }

    return count == n;  // 如果处理完所有顶点则无环
}
