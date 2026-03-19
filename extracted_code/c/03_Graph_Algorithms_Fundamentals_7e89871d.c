/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\03_Graph_Algorithms_Fundamentals.md
 * Line: 1379
 * Language: c
 * Block ID: 7e89871d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 最小生成树算法
 * 文件名: minimum_spanning_tree.c
 * 编译: gcc -std=c23 -o minimum_spanning_tree minimum_spanning_tree.c
 */

#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_VERTICES 100
#define INF INT_MAX

// ==================== Prim算法 ====================

/**
 * @brief Prim算法 - 基础版本 O(V²)
 */
int prim_basic(int graph[MAX_VERTICES][MAX_VERTICES],
               int n, int mst[MAX_VERTICES][2]) {
    int parent[MAX_VERTICES];
    int key[MAX_VERTICES];
    bool in_mst[MAX_VERTICES] = {false};

    // 初始化
    for (int i = 0; i < n; i++) {
        key[i] = INF;
        parent[i] = -1;
    }
    key[0] = 0;

    int mst_weight = 0;
    int edge_count = 0;

    for (int count = 0; count < n; count++) {
        // 找最小key值顶点
        int u = -1;
        int min_key = INF;
        for (int i = 0; i < n; i++) {
            if (!in_mst[i] && key[i] < min_key) {
                min_key = key[i];
                u = i;
            }
        }

        if (u == -1) break;

        in_mst[u] = true;
        mst_weight += key[u];

        if (parent[u] != -1) {
            mst[edge_count][0] = parent[u];
            mst[edge_count][1] = u;
            edge_count++;
        }

        // 更新邻接点
        for (int v = 0; v < n; v++) {
            if (!in_mst[v] && graph[u][v] != INF &&
                graph[u][v] < key[v]) {
                key[v] = graph[u][v];
                parent[v] = u;
            }
        }
    }

    return mst_weight;
}

// 最小堆节点
typedef struct {
    int vertex;
    int key;
} HeapNode;

typedef struct {
    HeapNode nodes[MAX_VERTICES];
    int size;
    int pos[MAX_VERTICES];
} MinHeap;

void heap_init(MinHeap *h, int n) {
    h->size = 0;
    for (int i = 0; i < n; i++) h->pos[i] = -1;
}

void heap_swap(MinHeap *h, int i, int j) {
    HeapNode t = h->nodes[i];
    h->nodes[i] = h->nodes[j];
    h->nodes[j] = t;
    h->pos[h->nodes[i].vertex] = i;
    h->pos[h->nodes[j].vertex] = j;
}

void heapify_up(MinHeap *h, int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (h->nodes[p].key <= h->nodes[i].key) break;
        heap_swap(h, p, i);
        i = p;
    }
}

void heapify_down(MinHeap *h, int i) {
    while (true) {
        int min = i, l = 2*i+1, r = 2*i+2;
        if (l < h->size && h->nodes[l].key < h->nodes[min].key) min = l;
        if (r < h->size && h->nodes[r].key < h->nodes[min].key) min = r;
        if (min == i) break;
        heap_swap(h, i, min);
        i = min;
    }
}

void heap_push(MinHeap *h, int v, int k) {
    h->nodes[h->size] = (HeapNode){v, k};
    h->pos[v] = h->size;
    heapify_up(h, h->size++);
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

void heap_decrease(MinHeap *h, int v, int k) {
    int i = h->pos[v];
    if (i == -1) return;
    h->nodes[i].key = k;
    heapify_up(h, i);
}

/**
 * @brief Prim算法 - 堆优化 O(E log V)
 */
int prim_heap(int graph[MAX_VERTICES][MAX_VERTICES],
              int n, int mst[MAX_VERTICES][2]) {
    int parent[MAX_VERTICES];
    MinHeap heap;
    heap_init(&heap, n);

    for (int i = 0; i < n; i++) {
        heap_push(&heap, i, INF);
        parent[i] = -1;
    }
    heap_decrease(&heap, 0, 0);

    int weight = 0, edge_count = 0;

    while (heap.size > 0) {
        HeapNode min = heap_pop(&heap);
        int u = min.vertex;

        if (parent[u] != -1) {
            mst[edge_count][0] = parent[u];
            mst[edge_count][1] = u;
            edge_count++;
        }
        weight += min.key;

        for (int v = 0; v < n; v++) {
            if (graph[u][v] != INF && heap.pos[v] != -1 &&
                graph[u][v] < heap.nodes[heap.pos[v]].key) {
                parent[v] = u;
                heap_decrease(&heap, v, graph[u][v]);
            }
        }
    }

    return weight;
}
