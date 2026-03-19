/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\03_Graph_Algorithms_Fundamentals.md
 * Line: 1561
 * Language: c
 * Block ID: 664e61c9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ==================== 并查集 ====================

typedef struct {
    int parent[MAX_VERTICES];
    int rank[MAX_VERTICES];
} UnionFind;

void uf_init(UnionFind *uf, int n) {
    for (int i = 0; i < n; i++) {
        uf->parent[i] = i;
        uf->rank[i] = 0;
    }
}

int uf_find(UnionFind *uf, int x) {
    if (uf->parent[x] != x) {
        uf->parent[x] = uf_find(uf, uf->parent[x]);
    }
    return uf->parent[x];
}

void uf_union(UnionFind *uf, int x, int y) {
    int px = uf_find(uf, x);
    int py = uf_find(uf, y);

    if (px == py) return;

    if (uf->rank[px] < uf->rank[py]) {
        uf->parent[px] = py;
    } else if (uf->rank[px] > uf->rank[py]) {
        uf->parent[py] = px;
    } else {
        uf->parent[py] = px;
        uf->rank[px]++;
    }
}

// ==================== Kruskal算法 ====================

typedef struct {
    int u, v, weight;
} Edge;

// 边比较函数
int compare_edges(const void *a, const void *b) {
    return ((Edge *)a)->weight - ((Edge *)b)->weight;
}

/**
 * @brief Kruskal算法 O(E log E)
 */
int kruskal(Edge edges[], int num_edges, int n,
            int mst[MAX_VERTICES][2]) {
    // 按权重排序
    qsort(edges, num_edges, sizeof(Edge), compare_edges);

    UnionFind uf;
    uf_init(&uf, n);

    int weight = 0;
    int edge_count = 0;

    for (int i = 0; i < num_edges && edge_count < n - 1; i++) {
        int u = edges[i].u;
        int v = edges[i].v;

        if (uf_find(&uf, u) != uf_find(&uf, v)) {
            uf_union(&uf, u, v);
            mst[edge_count][0] = u;
            mst[edge_count][1] = v;
            edge_count++;
            weight += edges[i].weight;
        }
    }

    return weight;
}

// 主函数
int main(void) {
    int n = 5;
    char names[] = {'A', 'B', 'C', 'D', 'E'};

    int graph[MAX_VERTICES][MAX_VERTICES];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            graph[i][j] = (i == j) ? 0 : INF;
        }
    }

    // 添加边
    graph[0][1] = graph[1][0] = 2;  // A-B: 2
    graph[0][3] = graph[3][0] = 6;  // A-D: 6
    graph[1][2] = graph[2][1] = 3;  // B-C: 3
    graph[1][3] = graph[3][1] = 8;  // B-D: 8
    graph[1][4] = graph[4][1] = 5;  // B-E: 5
    graph[2][4] = graph[4][2] = 7;  // C-E: 7
    graph[3][4] = graph[4][3] = 9;  // D-E: 9

    printf("=== 最小生成树演示 ===\n\n");

    int mst[MAX_VERTICES][2];

    printf("Prim算法:\n");
    int weight1 = prim_basic(graph, n, mst);
    printf("总权重: %d\n", weight1);
    printf("边: ");
    for (int i = 0; i < n - 1; i++) {
        printf("(%c,%c) ", names[mst[i][0]], names[mst[i][1]]);
    }
    printf("\n");

    // 收集所有边用于Kruskal
    Edge edges[20];
    int edge_count = 0;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (graph[i][j] != INF) {
                edges[edge_count++] = (Edge){i, j, graph[i][j]};
            }
        }
    }

    printf("\nKruskal算法:\n");
    int weight2 = kruskal(edges, edge_count, n, mst);
    printf("总权重: %d\n", weight2);
    printf("边: ");
    for (int i = 0; i < n - 1; i++) {
        printf("(%c,%c) ", names[mst[i][0]], names[mst[i][1]]);
    }
    printf("\n");

    return 0;
}
