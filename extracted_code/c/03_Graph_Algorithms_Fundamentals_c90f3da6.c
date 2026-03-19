/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\03_Graph_Algorithms_Fundamentals.md
 * Line: 942
 * Language: c
 * Block ID: c90f3da6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 最短路径算法
 * 文件名: shortest_path.c
 * 编译: gcc -std=c23 -o shortest_path shortest_path.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_VERTICES 100
#define INF INT_MAX

// ==================== Dijkstra算法 ====================

/**
 * @brief 基础Dijkstra - 使用数组找最小值 O(V²)
 */
void dijkstra_basic(int graph[MAX_VERTICES][MAX_VERTICES],
                    int n, int start, int dist[], int parent[]) {
    bool visited[MAX_VERTICES] = {false};

    // 初始化
    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }
    dist[start] = 0;

    // 遍历所有顶点
    for (int count = 0; count < n - 1; count++) {
        // 找未访问的最小距离顶点
        int u = -1;
        int min_dist = INF;
        for (int i = 0; i < n; i++) {
            if (!visited[i] && dist[i] < min_dist) {
                min_dist = dist[i];
                u = i;
            }
        }

        if (u == -1) break;  // 剩余顶点不可达
        visited[u] = true;

        // 更新邻接点距离
        for (int v = 0; v < n; v++) {
            if (!visited[v] && graph[u][v] != INF &&
                dist[u] != INF &&
                dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }
}

// 最小堆节点
typedef struct {
    int vertex;
    int dist;
} HeapNode;

typedef struct {
    HeapNode nodes[MAX_VERTICES];
    int size;
    int pos[MAX_VERTICES];  // 顶点在堆中的位置
} MinHeap;

void heap_init(MinHeap *h, int n) {
    h->size = 0;
    for (int i = 0; i < n; i++) {
        h->pos[i] = -1;
    }
}

void heap_swap(MinHeap *h, int i, int j) {
    HeapNode temp = h->nodes[i];
    h->nodes[i] = h->nodes[j];
    h->nodes[j] = temp;

    h->pos[h->nodes[i].vertex] = i;
    h->pos[h->nodes[j].vertex] = j;
}

void heapify_up(MinHeap *h, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (h->nodes[parent].dist <= h->nodes[idx].dist) break;
        heap_swap(h, parent, idx);
        idx = parent;
    }
}

void heapify_down(MinHeap *h, int idx) {
    while (true) {
        int min = idx;
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;

        if (left < h->size && h->nodes[left].dist < h->nodes[min].dist) {
            min = left;
        }
        if (right < h->size && h->nodes[right].dist < h->nodes[min].dist) {
            min = right;
        }

        if (min == idx) break;
        heap_swap(h, idx, min);
        idx = min;
    }
}

void heap_push(MinHeap *h, int vertex, int dist) {
    h->nodes[h->size].vertex = vertex;
    h->nodes[h->size].dist = dist;
    h->pos[vertex] = h->size;
    heapify_up(h, h->size);
    h->size++;
}

HeapNode heap_pop(MinHeap *h) {
    HeapNode min = h->nodes[0];
    h->pos[min.vertex] = -1;

    h->nodes[0] = h->nodes[--h->size];
    if (h->size > 0) {
        h->pos[h->nodes[0].vertex] = 0;
        heapify_down(h, 0);
    }

    return min;
}

void heap_decrease_key(MinHeap *h, int vertex, int new_dist) {
    int idx = h->pos[vertex];
    if (idx == -1) return;

    h->nodes[idx].dist = new_dist;
    heapify_up(h, idx);
}

bool heap_contains(MinHeap *h, int vertex) {
    return h->pos[vertex] != -1;
}

/**
 * @brief 优化Dijkstra - 使用最小堆 O((V+E)log V)
 */
void dijkstra_heap(int graph[MAX_VERTICES][MAX_VERTICES],
                   int n, int start, int dist[], int parent[]) {
    MinHeap heap;
    heap_init(&heap, n);

    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }
    dist[start] = 0;

    heap_push(&heap, start, 0);

    while (heap.size > 0) {
        HeapNode min = heap_pop(&heap);
        int u = min.vertex;

        // 更新邻接点
        for (int v = 0; v < n; v++) {
            if (graph[u][v] != INF && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;

                if (heap_contains(&heap, v)) {
                    heap_decrease_key(&heap, v, dist[v]);
                } else {
                    heap_push(&heap, v, dist[v]);
                }
            }
        }
    }
}

void print_shortest_paths(int dist[], int parent[], int n,
                          int start, const char names[]) {
    printf("从 %c 出发的最短路径:\n", names[start]);
    for (int i = 0; i < n; i++) {
        printf("到 %c: 距离=%d, 路径=", names[i], dist[i]);

        // 回溯打印路径
        int path[MAX_VERTICES];
        int path_len = 0;
        int curr = i;
        while (curr != -1) {
            path[path_len++] = curr;
            curr = parent[curr];
        }

        for (int j = path_len - 1; j >= 0; j--) {
            printf("%c ", names[path[j]]);
        }
        printf("\n");
    }
}
