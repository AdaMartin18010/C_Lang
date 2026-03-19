/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 1473
 * Language: c
 * Block ID: 03b20c04
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * Dijkstra最短路径算法 - 使用优先队列
 *
 * 时间复杂度: O((V + E) log V)
 */

typedef struct {
    int vertex;
    int distance;
} GraphNode;

typedef struct {
    GraphNode* nodes;
    int* positions;  // 每个顶点在堆中的位置
    int size;
    int capacity;
} GraphPriorityQueue;

void gpq_init(GraphPriorityQueue* pq, int n_vertices) {
    pq->capacity = n_vertices;
    pq->nodes = (GraphNode*)malloc(n_vertices * sizeof(GraphNode));
    pq->positions = (int*)malloc(n_vertices * sizeof(int));
    for (int i = 0; i < n_vertices; i++) {
        pq->positions[i] = -1;
    }
    pq->size = 0;
}

// 判断队列是否包含某顶点
bool gpq_contains(const GraphPriorityQueue* pq, int vertex) {
    return pq->positions[vertex] >= 0;
}

// 获取顶点距离
int gpq_get_distance(const GraphPriorityQueue* pq, int vertex) {
    int pos = pq->positions[vertex];
    if (pos < 0) return -1;
    return pq->nodes[pos].distance;
}

// Dijkstra算法主函数
void dijkstra(const int** graph, int n, int src, int* dist) {
    // graph[i][j] = i到j的边权，-1表示无连接

    // 初始化距离
    for (int i = 0; i < n; i++) {
        dist[i] = (i == src) ? 0 : INT_MAX;
    }

    GraphPriorityQueue pq;
    gpq_init(&pq, n);

    // 将源点入队
    pq.nodes[0] = (GraphNode){src, 0};
    pq.positions[src] = 0;
    pq.size = 1;

    while (pq.size > 0) {
        // 弹出距离最小的顶点
        GraphNode min_node = pq.nodes[0];
        pq.positions[min_node.vertex] = -1;
        pq.size--;
        if (pq.size > 0) {
            pq.nodes[0] = pq.nodes[pq.size];
            pq.positions[pq.nodes[0].vertex] = 0;
            // 下沉调整...
        }

        // 更新邻接顶点
        for (int v = 0; v < n; v++) {
            if (graph[min_node.vertex][v] < 0) continue;

            int new_dist = min_node.distance + graph[min_node.vertex][v];
            if (new_dist < dist[v]) {
                dist[v] = new_dist;
                // 更新或插入优先队列...
            }
        }
    }

    free(pq.nodes);
    free(pq.positions);
}
