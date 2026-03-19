/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\03_Graph_Algorithms_Fundamentals.md
 * Line: 726
 * Language: c
 * Block ID: 5cdb11d0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ==================== BFS实现 ====================

// 队列实现
typedef struct {
    int items[MAX_VERTICES];
    int front, rear;
} Queue;

void queue_init(Queue *q) { q->front = q->rear = 0; }
bool queue_empty(Queue *q) { return q->front == q->rear; }
void queue_enqueue(Queue *q, int v) { q->items[q->rear++] = v; }
int queue_dequeue(Queue *q) { return q->items[q->front++]; }

/**
 * @brief 基础BFS
 */
void bfs(Graph *g, int start) {
    Queue q;
    queue_init(&q);

    visited[start] = true;
    printf("%c ", g->vertices[start].name);
    queue_enqueue(&q, start);

    while (!queue_empty(&q)) {
        int v = queue_dequeue(&q);

        EdgeNode *curr = g->vertices[v].head;
        while (curr) {
            if (!visited[curr->dest]) {
                visited[curr->dest] = true;
                printf("%c ", g->vertices[curr->dest].name);
                queue_enqueue(&q, curr->dest);
            }
            curr = curr->next;
        }
    }
}

/**
 * @brief BFS最短路径（无权图）
 */
void bfs_shortest_path(Graph *g, int start, int dist[], int parent[]) {
    Queue q;
    queue_init(&q);

    for (int i = 0; i < g->n; i++) {
        dist[i] = -1;
        parent[i] = -1;
    }

    dist[start] = 0;
    queue_enqueue(&q, start);

    while (!queue_empty(&q)) {
        int v = queue_dequeue(&q);

        EdgeNode *curr = g->vertices[v].head;
        while (curr) {
            if (dist[curr->dest] == -1) {
                dist[curr->dest] = dist[v] + 1;
                parent[curr->dest] = v;
                queue_enqueue(&q, curr->dest);
            }
            curr = curr->next;
        }
    }
}

/**
 * @brief 打印从start到v的路径
 */
void print_path(Graph *g, int parent[], int v) {
    if (v == -1) return;
    print_path(g, parent, parent[v]);
    printf("%c ", g->vertices[v].name);
}

/**
 * @brief BFS层序遍历
 */
void bfs_level_order(Graph *g, int start) {
    Queue q;
    queue_init(&q);

    visited[start] = true;
    queue_enqueue(&q, start);
    queue_enqueue(&q, -1);  // 层标记

    printf("层 0: ");
    int level = 0;

    while (!queue_empty(&q)) {
        int v = queue_dequeue(&q);

        if (v == -1) {
            if (!queue_empty(&q)) {
                level++;
                printf("\n层 %d: ", level);
                queue_enqueue(&q, -1);
            }
            continue;
        }

        printf("%c ", g->vertices[v].name);

        EdgeNode *curr = g->vertices[v].head;
        while (curr) {
            if (!visited[curr->dest]) {
                visited[curr->dest] = true;
                queue_enqueue(&q, curr->dest);
            }
            curr = curr->next;
        }
    }
    printf("\n");
}

// 测试函数
void reset_visited(int n) {
    for (int i = 0; i < n; i++) {
        visited[i] = false;
    }
}

void free_graph(Graph *g) {
    for (int i = 0; i < g->n; i++) {
        EdgeNode *curr = g->vertices[i].head;
        while (curr) {
            EdgeNode *temp = curr;
            curr = curr->next;
            free(temp);
        }
    }
}

int main(void) {
    Graph g;
    init_graph(&g, 6);

    // 构建图
    add_undirected_edge(&g, 0, 1, 1);  // A-B
    add_undirected_edge(&g, 0, 2, 1);  // A-C
    add_undirected_edge(&g, 1, 3, 1);  // B-D
    add_undirected_edge(&g, 2, 3, 1);  // C-D
    add_undirected_edge(&g, 2, 4, 1);  // C-E
    add_undirected_edge(&g, 3, 5, 1);  // D-F

    printf("=== 图遍历演示 ===\n\n");

    printf("DFS (递归): ");
    reset_visited(g.n);
    dfs_recursive(&g, 0);
    printf("\n");

    printf("DFS (迭代): ");
    reset_visited(g.n);
    dfs_iterative(&g, 0);
    printf("\n");

    printf("\nBFS: ");
    reset_visited(g.n);
    bfs(&g, 0);
    printf("\n");

    printf("\nBFS层序遍历:\n");
    reset_visited(g.n);
    bfs_level_order(&g, 0);

    printf("\nBFS最短路径 (A到各点):\n");
    int dist[MAX_VERTICES], parent[MAX_VERTICES];
    bfs_shortest_path(&g, 0, dist, parent);
    for (int i = 0; i < g.n; i++) {
        printf("到 %c: 距离=%d, 路径=", g.vertices[i].name, dist[i]);
        print_path(&g, parent, i);
        printf("\n");
    }

    free_graph(&g);
    return 0;
}
