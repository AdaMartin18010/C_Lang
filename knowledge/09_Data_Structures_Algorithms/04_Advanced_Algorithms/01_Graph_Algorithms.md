# 图算法实现

> **层级**: L3 (核心层)
> **目标**: 掌握常用图算法与实现
> **标准**: C11/C17
> **更新**: 2026-03-28

---

## 目录

1. [图的形式化定义](#1-图的形式化定义)
2. [图表示方法](#2-图表示方法)
3. [遍历算法](#3-遍历算法)
4. [最短路径算法](#4-最短路径算法)
5. [最小生成树算法](#5-最小生成树算法)
6. [网络流算法](#6-网络流算法)
7. [匹配算法](#7-匹配算法)
8. [深入理解](#8-深入理解)
9. [思维表征](#9-思维表征)
10. [层次映射](#10-层次映射)

---

## 1. 图的形式化定义

### 1.1 基本定义

**图**（Graph）是顶点集和边集的集合，形式化定义为 $G = (V, E)$：

- $V$：顶点（Vertex）的有限非空集合，$|V| = n$
- $E$：边（Edge）的集合，$|E| = m$
- 有向图：$E \subseteq V \times V$，边有方向，$(u,v) \neq (v,u)$
- 无向图：$E \subseteq \{\{u,v\} \mid u,v \in V\}$，边无方向，$\{u,v\} = \{v,u\}$

### 1.2 加权图

**加权图**（Weighted Graph）为每条边赋予权重：

$$G = (V, E, w) \quad \text{其中} \quad w: E \rightarrow \mathbb{R}$$

- $w(u,v)$ 表示边 $(u,v)$ 的权重
- 无权图可视为所有边权重为1的特殊情况

### 1.3 图的分类

| 类型 | 定义 | 特性 |
|------|------|------|
| 简单图 | 无自环、无重边 | $|E| \leq \frac{n(n-1)}{2}$（无向） |
| 完全图 | 每对顶点间都有边 | $|E| = \frac{n(n-1)}{2}$ |
| 稀疏图 | $|E| \ll |V|^2$ | 适合邻接表存储 |
| 稠密图 | $|E| \approx |V|^2$ | 适合邻接矩阵存储 |
| DAG | 有向无环图 | 可进行拓扑排序 |
| 树 | 连通无环图 | $|E| = |V| - 1$ |

---

## 2. 图表示方法

### 2.1 邻接矩阵 (Adjacency Matrix)

**形式化**: 对于图 $G=(V,E)$，邻接矩阵 $A$ 是 $n \times n$ 矩阵：

$$A_{ij} = \begin{cases} w(i,j) & (i,j) \in E \\ 0 \text{或} \infty & (i,j) \notin E \end{cases}$$

```c
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_VERTICES 1000
#define INF INT_MAX

/* 邻接矩阵表示 */
typedef struct {
    int num_vertices;
    int num_edges;
    int directed;           // 1=有向图, 0=无向图
    int weighted;           // 1=带权图, 0=无权图
    int matrix[MAX_VERTICES][MAX_VERTICES];
} AdjMatrixGraph;

/* 初始化图 */
void adj_matrix_init(AdjMatrixGraph *g, int n, int directed, int weighted) {
    g->num_vertices = n;
    g->num_edges = 0;
    g->directed = directed;
    g->weighted = weighted;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            g->matrix[i][j] = (weighted && i != j) ? INF : 0;
        }
    }
}

/* 添加边 */
void adj_matrix_add_edge(AdjMatrixGraph *g, int u, int v, int w) {
    g->matrix[u][v] = g->weighted ? w : 1;
    if (!g->directed) {
        g->matrix[v][u] = g->weighted ? w : 1;
    }
    g->num_edges++;
}

/* 空间复杂度: O(|V|²), 查询边: O(1), 遍历邻接点: O(|V|) */
```

### 2.2 邻接表 (Adjacency List)

**形式化**: 对每个顶点 $u \in V$，维护其邻接点列表 $Adj[u] = \{v \mid (u,v) \in E\}$

```c
/* 邻接表边节点 */
typedef struct AdjNode {
    int dest;               // 目标顶点
    int weight;             // 边权重
    struct AdjNode *next;   // 下一条边
} AdjNode;

/* 邻接表图 */
typedef struct {
    int num_vertices;
    int num_edges;
    int directed;
    AdjNode **array;        // 邻接表数组
} AdjListGraph;

/* 创建边节点 */
AdjNode* create_adj_node(int dest, int weight) {
    AdjNode *node = (AdjNode*)malloc(sizeof(AdjNode));
    node->dest = dest;
    node->weight = weight;
    node->next = NULL;
    return node;
}

/* 初始化图 */
void adj_list_init(AdjListGraph *g, int n, int directed) {
    g->num_vertices = n;
    g->num_edges = 0;
    g->directed = directed;
    g->array = (AdjNode**)malloc(n * sizeof(AdjNode*));
    for (int i = 0; i < n; i++) {
        g->array[i] = NULL;
    }
}

/* 添加边 (头插法) */
void adj_list_add_edge(AdjListGraph *g, int src, int dest, int weight) {
    AdjNode *node = create_adj_node(dest, weight);
    node->next = g->array[src];
    g->array[src] = node;

    if (!g->directed) {
        node = create_adj_node(src, weight);
        node->next = g->array[dest];
        g->array[dest] = node;
    }
    g->num_edges++;
}

/* 空间复杂度: O(|V| + |E|), 查询边: O(degree(u)), 遍历邻接点: O(degree(u)) */
```

### 2.3 边列表 (Edge List)

**形式化**: 直接存储边集 $E = \{(u_1,v_1,w_1), (u_2,v_2,w_2), \ldots, (u_m,v_m,w_m)\}$

```c
/* 边结构 */
typedef struct {
    int src;        // 起点
    int dest;       // 终点
    int weight;     // 权重
} Edge;

/* 边列表图 */
typedef struct {
    int num_vertices;
    int num_edges;
    int max_edges;
    Edge *edges;    // 边数组
} EdgeListGraph;

/* 初始化 */
void edge_list_init(EdgeListGraph *g, int n, int m) {
    g->num_vertices = n;
    g->num_edges = 0;
    g->max_edges = m;
    g->edges = (Edge*)malloc(m * sizeof(Edge));
}

/* 添加边 */
void edge_list_add_edge(EdgeListGraph *g, int src, int dest, int weight) {
    if (g->num_edges >= g->max_edges) return;
    g->edges[g->num_edges].src = src;
    g->edges[g->num_edges].dest = dest;
    g->edges[g->num_edges].weight = weight;
    g->num_edges++;
}

/* 空间复杂度: O(|E|), 查询边: O(|E|), 适合Kruskal等需排序边的算法 */
```

### 2.4 压缩稀疏行 (CSR - Compressed Sparse Row)

**形式化**: 使用三个数组压缩存储稀疏邻接矩阵：

- `values[]`: 存储非零元素值（权重）
- `col_idx[]`: 存储非零元素的列索引
- `row_ptr[]`: 存储每行在`values`中的起始位置

```c
/* CSR格式图 */
typedef struct {
    int num_vertices;
    int num_edges;
    int *values;        // 边权重
    int *col_idx;       // 列索引
    int *row_ptr;       // 行指针
} CSRGraph;

/* 从邻接表构建CSR (要求边已按起点排序) */
void csr_init(CSRGraph *g, int n, Edge edges[], int m) {
    g->num_vertices = n;
    g->num_edges = m;
    g->values = (int*)malloc(m * sizeof(int));
    g->col_idx = (int*)malloc(m * sizeof(int));
    g->row_ptr = (int*)malloc((n + 1) * sizeof(int));

    // 统计每行的边数
    for (int i = 0; i <= n; i++) g->row_ptr[i] = 0;
    for (int i = 0; i < m; i++) {
        g->row_ptr[edges[i].src + 1]++;
    }

    // 前缀和得到行指针
    for (int i = 1; i <= n; i++) {
        g->row_ptr[i] += g->row_ptr[i - 1];
    }

    // 填充数据
    int *temp = (int*)malloc(n * sizeof(int));
    memcpy(temp, g->row_ptr, n * sizeof(int));

    for (int i = 0; i < m; i++) {
        int row = edges[i].src;
        int pos = temp[row]++;
        g->col_idx[pos] = edges[i].dest;
        g->values[pos] = edges[i].weight;
    }

    free(temp);
}

/* 空间复杂度: O(|V| + |E|), 缓存友好, 适合大规模稀疏图 */
```

---

## 3. 遍历算法

### 3.1 深度优先搜索 (DFS)

**形式化**: 从起始顶点 $s$ 出发，尽可能深地探索图的分支。

```c
#include <stdbool.h>

/* 递归DFS */
void dfs_recursive(AdjListGraph *g, int v, bool visited[]) {
    visited[v] = true;
    printf("%d ", v);  // 访问顶点

    for (AdjNode *p = g->array[v]; p != NULL; p = p->next) {
        if (!visited[p->dest]) {
            dfs_recursive(g, p->dest, visited);
        }
    }
}

/* 迭代DFS (使用栈) */
#include <stdio.h>

#define STACK_SIZE 1000

typedef struct {
    int data[STACK_SIZE];
    int top;
} Stack;

void stack_init(Stack *s) { s->top = -1; }
bool stack_empty(Stack *s) { return s->top == -1; }
void stack_push(Stack *s, int x) { s->data[++s->top] = x; }
int stack_pop(Stack *s) { return s->data[s->top--]; }

void dfs_iterative(AdjListGraph *g, int start) {
    bool visited[MAX_VERTICES] = {false};
    Stack s;
    stack_init(&s);
    stack_push(&s, start);

    while (!stack_empty(&s)) {
        int v = stack_pop(&s);

        if (!visited[v]) {
            visited[v] = true;
            printf("%d ", v);

            // 将未访问的邻接点入栈
            for (AdjNode *p = g->array[v]; p != NULL; p = p->next) {
                if (!visited[p->dest]) {
                    stack_push(&s, p->dest);
                }
            }
        }
    }
}

/* 时间复杂度: O(|V| + |E|), 空间复杂度: O(|V|) */
```

### 3.2 广度优先搜索 (BFS)

**形式化**: 从起始顶点 $s$ 出发，按层次逐层访问所有邻接点。

```c
#define QUEUE_SIZE 1000

typedef struct {
    int data[QUEUE_SIZE];
    int front, rear;
} Queue;

void queue_init(Queue *q) { q->front = q->rear = 0; }
bool queue_empty(Queue *q) { return q->front == q->rear; }
void queue_push(Queue *q, int x) { q->data[q->rear++] = x; }
int queue_pop(Queue *q) { return q->data[q->front++]; }

/* BFS */
void bfs(AdjListGraph *g, int start) {
    bool visited[MAX_VERTICES] = {false};
    Queue q;
    queue_init(&q);

    visited[start] = true;
    queue_push(&q, start);

    while (!queue_empty(&q)) {
        int v = queue_pop(&q);
        printf("%d ", v);

        for (AdjNode *p = g->array[v]; p != NULL; p = p->next) {
            if (!visited[p->dest]) {
                visited[p->dest] = true;
                queue_push(&q, p->dest);
            }
        }
    }
}

/* 时间复杂度: O(|V| + |E|), 空间复杂度: O(|V|) */
```

### 3.3 拓扑排序

**形式化**: 对于DAG $G=(V,E)$，拓扑排序是顶点的线性排序，使得对每条边 $(u,v) \in E$，$u$ 在 $v$ 之前。

**Kahn算法**（基于入度）：

```c
/* Kahn算法 - BFS实现 */
bool topological_sort_kahn(AdjListGraph *g, int result[]) {
    int in_degree[MAX_VERTICES] = {0};

    // 计算入度
    for (int i = 0; i < g->num_vertices; i++) {
        for (AdjNode *p = g->array[i]; p != NULL; p = p->next) {
            in_degree[p->dest]++;
        }
    }

    // 入度为0的顶点入队
    Queue q;
    queue_init(&q);
    for (int i = 0; i < g->num_vertices; i++) {
        if (in_degree[i] == 0) {
            queue_push(&q, i);
        }
    }

    int count = 0;
    while (!queue_empty(&q)) {
        int u = queue_pop(&q);
        result[count++] = u;

        for (AdjNode *p = g->array[u]; p != NULL; p = p->next) {
            in_degree[p->dest]--;
            if (in_degree[p->dest] == 0) {
                queue_push(&q, p->dest);
            }
        }
    }

    return count == g->num_vertices;  // 若存在环则返回false
}
```

**DFS-based算法**：

```c
/* DFS-based拓扑排序 */
void dfs_topo(AdjListGraph *g, int v, bool visited[], int *time, int finish_time[]) {
    visited[v] = true;

    for (AdjNode *p = g->array[v]; p != NULL; p = p->next) {
        if (!visited[p->dest]) {
            dfs_topo(g, p->dest, visited, time, finish_time);
        }
    }

    finish_time[v] = (*time)++;
}

bool topological_sort_dfs(AdjListGraph *g, int result[]) {
    bool visited[MAX_VERTICES] = {false};
    int finish_time[MAX_VERTICES];
    int time = 0;

    for (int i = 0; i < g->num_vertices; i++) {
        if (!visited[i]) {
            dfs_topo(g, i, visited, &time, finish_time);
        }
    }

    // 按finish_time降序排列
    for (int i = 0; i < g->num_vertices; i++) {
        result[g->num_vertices - 1 - finish_time[i]] = i;
    }

    return true;
}

/* 时间复杂度: O(|V| + |E|), 空间复杂度: O(|V|) */
```

### 3.4 强连通分量 (SCC)

**Tarjan算法**（单遍DFS）：

```c
typedef struct {
    int index[MAX_VERTICES];    // DFS序号
    int lowlink[MAX_VERTICES];  // 能追溯到的最早顶点
    int on_stack[MAX_VERTICES]; // 是否在栈中
    int stack[MAX_VERTICES];
    int stack_top;
    int idx;
    int scc_count;
    int scc_id[MAX_VERTICES];   // 每个顶点所属的SCC
} TarjanData;

void tarjan_scc_dfs(AdjListGraph *g, int v, TarjanData *td) {
    td->index[v] = td->lowlink[v] = td->idx++;
    td->stack[++td->stack_top] = v;
    td->on_stack[v] = 1;

    for (AdjNode *p = g->array[v]; p != NULL; p = p->next) {
        int w = p->dest;
        if (td->index[w] == -1) {
            tarjan_scc_dfs(g, w, td);
            if (td->lowlink[w] < td->lowlink[v]) {
                td->lowlink[v] = td->lowlink[w];
            }
        } else if (td->on_stack[w] && td->index[w] < td->lowlink[v]) {
            td->lowlink[v] = td->index[w];
        }
    }

    // 找到SCC的根
    if (td->lowlink[v] == td->index[v]) {
        int w;
        do {
            w = td->stack[td->stack_top--];
            td->on_stack[w] = 0;
            td->scc_id[w] = td->scc_count;
        } while (w != v);
        td->scc_count++;
    }
}

void tarjan_scc(AdjListGraph *g, int scc_id[]) {
    TarjanData td = {.idx = 0, .stack_top = -1, .scc_count = 0};
    for (int i = 0; i < g->num_vertices; i++) {
        td.index[i] = -1;
        td.lowlink[i] = 0;
        td.on_stack[i] = 0;
    }

    for (int i = 0; i < g->num_vertices; i++) {
        if (td.index[i] == -1) {
            tarjan_scc_dfs(g, i, &td);
        }
    }

    memcpy(scc_id, td.scc_id, g->num_vertices * sizeof(int));
}

/* 时间复杂度: O(|V| + |E|), 空间复杂度: O(|V|) */
```

---

## 4. 最短路径算法

### 4.1 Dijkstra算法（堆优化）

**形式化**: 对于非负权图 $G=(V,E,w)$，求从源点 $s$ 到所有顶点的最短路径。

**贪心策略**: 每次选择距离最小的未确定顶点，松弛其邻接边。

```c
#include <stdbool.h>

/* 最小堆节点 */
typedef struct {
    int vertex;
    int dist;
} HeapNode;

typedef struct {
    HeapNode data[MAX_VERTICES];
    int size;
    int pos[MAX_VERTICES];  // 顶点在堆中的位置
} MinHeap;

void heap_swap(MinHeap *h, int i, int j) {
    HeapNode tmp = h->data[i];
    h->data[i] = h->data[j];
    h->data[j] = tmp;
    h->pos[h->data[i].vertex] = i;
    h->pos[h->data[j].vertex] = j;
}

void heapify_up(MinHeap *h, int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (h->data[i].dist >= h->data[parent].dist) break;
        heap_swap(h, i, parent);
        i = parent;
    }
}

void heapify_down(MinHeap *h, int i) {
    while (2 * i + 1 < h->size) {
        int min_child = 2 * i + 1;
        if (min_child + 1 < h->size &&
            h->data[min_child + 1].dist < h->data[min_child].dist) {
            min_child++;
        }
        if (h->data[i].dist <= h->data[min_child].dist) break;
        heap_swap(h, i, min_child);
        i = min_child;
    }
}

HeapNode heap_extract_min(MinHeap *h) {
    HeapNode min = h->data[0];
    h->pos[min.vertex] = -1;
    h->data[0] = h->data[--h->size];
    if (h->size > 0) {
        h->pos[h->data[0].vertex] = 0;
        heapify_down(h, 0);
    }
    return min;
}

void heap_decrease_key(MinHeap *h, int vertex, int new_dist) {
    int i = h->pos[vertex];
    if (i == -1 || h->data[i].dist <= new_dist) return;
    h->data[i].dist = new_dist;
    heapify_up(h, i);
}

/* Dijkstra算法 */
void dijkstra(AdjListGraph *g, int src, int dist[], int parent[]) {
    bool visited[MAX_VERTICES] = {false};
    MinHeap h = {.size = 0};

    for (int i = 0; i < g->num_vertices; i++) {
        dist[i] = INF;
        parent[i] = -1;
        h.pos[i] = -1;
    }

    dist[src] = 0;
    h.data[0] = (HeapNode){src, 0};
    h.pos[src] = 0;
    h.size = 1;

    while (h.size > 0) {
        HeapNode node = heap_extract_min(&h);
        int u = node.vertex;
        visited[u] = true;

        for (AdjNode *p = g->array[u]; p != NULL; p = p->next) {
            int v = p->dest;
            if (!visited[v] && dist[u] != INF &&
                dist[u] + p->weight < dist[v]) {
                dist[v] = dist[u] + p->weight;
                parent[v] = u;
                if (h.pos[v] == -1) {
                    h.data[h.size] = (HeapNode){v, dist[v]};
                    h.pos[v] = h.size++;
                    heapify_up(&h, h.pos[v]);
                } else {
                    heap_decrease_key(&h, v, dist[v]);
                }
            }
        }
    }
}

/* 时间复杂度: O((|V|+|E|)log|V|), 空间复杂度: O(|V|) */
```

### 4.2 Bellman-Ford算法

**形式化**: 可处理负权边，检测负权环。

```c
/* Bellman-Ford算法 */
bool bellman_ford(EdgeListGraph *g, int src, int dist[], int parent[]) {
    int V = g->num_vertices;
    int E = g->num_edges;

    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }
    dist[src] = 0;

    // 松弛所有边 V-1 次
    for (int i = 0; i < V - 1; i++) {
        for (int j = 0; j < E; j++) {
            int u = g->edges[j].src;
            int v = g->edges[j].dest;
            int w = g->edges[j].weight;

            if (dist[u] != INF && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
            }
        }
    }

    // 检测负权环
    for (int j = 0; j < E; j++) {
        int u = g->edges[j].src;
        int v = g->edges[j].dest;
        int w = g->edges[j].weight;

        if (dist[u] != INF && dist[u] + w < dist[v]) {
            return false;  // 存在负权环
        }
    }

    return true;
}

/* 时间复杂度: O(|V|·|E|), 空间复杂度: O(|V|) */
```

### 4.3 SPFA算法

**形式化**: Bellman-Ford的队列优化，对稀疏图效率更高。

```c
/* SPFA算法 */
bool spfa(AdjListGraph *g, int src, int dist[], int parent[]) {
    int in_queue[MAX_VERTICES] = {0};
    int count[MAX_VERTICES] = {0};  // 入队次数计数
    Queue q;
    queue_init(&q);

    for (int i = 0; i < g->num_vertices; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }

    dist[src] = 0;
    queue_push(&q, src);
    in_queue[src] = 1;
    count[src] = 1;

    while (!queue_empty(&q)) {
        int u = queue_pop(&q);
        in_queue[u] = 0;

        for (AdjNode *p = g->array[u]; p != NULL; p = p->next) {
            int v = p->dest;
            int w = p->weight;

            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;

                if (!in_queue[v]) {
                    queue_push(&q, v);
                    in_queue[v] = 1;
                    if (++count[v] > g->num_vertices) {
                        return false;  // 存在负权环
                    }
                }
            }
        }
    }

    return true;
}

/* 平均时间复杂度: O(|E|), 最坏: O(|V|·|E|), 空间复杂度: O(|V|) */
```

### 4.4 Floyd-Warshall算法

**形式化**: 全源最短路径，动态规划求解。

```c
/* Floyd-Warshall算法 */
void floyd_warshall(int dist[][MAX_VERTICES], int n, int next[][MAX_VERTICES]) {
    // 初始化next数组用于路径重建
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (dist[i][j] != INF && i != j) {
                next[i][j] = j;
            } else {
                next[i][j] = -1;
            }
        }
    }

    // 动态规划: dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j])
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

/* 时间复杂度: O(|V|³), 空间复杂度: O(|V|²) */
```

### 4.5 A*启发式算法

**形式化**: 使用启发函数 $f(n) = g(n) + h(n)$ 指导搜索方向。

```c
/* A*算法节点 */
typedef struct {
    int vertex;
    int g;      // 从起点到当前点的实际代价
    int f;      // f = g + h (估计总代价)
} AStarNode;

/* 启发函数: 欧几里得距离 */
int heuristic(int u, int v, int x[], int y[]) {
    int dx = x[u] - x[v];
    int dy = y[u] - y[v];
    return (int)(sqrt(dx*dx + dy*dy));
}

/* A*算法 (假设有坐标信息) */
void astar(AdjListGraph *g, int src, int dest, int dist[], int parent[],
           int h[], int x[], int y[]) {
    bool closed_set[MAX_VERTICES] = {false};
    MinHeap open_set = {.size = 0};
    int g_score[MAX_VERTICES];

    for (int i = 0; i < g->num_vertices; i++) {
        g_score[i] = INF;
        dist[i] = INF;
        parent[i] = -1;
        open_set.pos[i] = -1;
    }

    g_score[src] = 0;
    dist[src] = h[src];  // f = g + h

    open_set.data[0] = (HeapNode){src, dist[src]};
    open_set.pos[src] = 0;
    open_set.size = 1;

    while (open_set.size > 0) {
        HeapNode node = heap_extract_min(&open_set);
        int u = node.vertex;

        if (u == dest) break;

        if (closed_set[u]) continue;
        closed_set[u] = true;

        for (AdjNode *p = g->array[u]; p != NULL; p = p->next) {
            int v = p->dest;
            if (closed_set[v]) continue;

            int tentative_g = g_score[u] + p->weight;

            if (tentative_g < g_score[v]) {
                parent[v] = u;
                g_score[v] = tentative_g;
                dist[v] = g_score[v] + heuristic(v, dest, x, y);

                if (open_set.pos[v] == -1) {
                    open_set.data[open_set.size] = (HeapNode){v, dist[v]};
                    open_set.pos[v] = open_set.size++;
                }
                heapify_up(&open_set, open_set.pos[v]);
            }
        }
    }
}

/* 时间复杂度: 取决于启发函数质量, 最坏O(|E|) */
```

---

## 5. 最小生成树算法

### 5.1 Prim算法（优先队列）

**形式化**: 对于连通无向图 $G=(V,E,w)$，找到连接所有顶点的边集 $T$，使 $\sum_{e \in T} w(e)$ 最小。

**贪心策略**: 从任意顶点开始，每次添加连接树与非树顶点的最小权边。

```c
/* Prim算法 */
void prim_mst(AdjListGraph *g, int mst_parent[], int mst_weight[]) {
    bool in_mst[MAX_VERTICES] = {false};
    MinHeap h = {.size = 0};

    for (int i = 0; i < g->num_vertices; i++) {
        mst_weight[i] = INF;
        mst_parent[i] = -1;
        h.pos[i] = -1;
    }

    // 从顶点0开始
    mst_weight[0] = 0;
    h.data[0] = (HeapNode){0, 0};
    h.pos[0] = 0;
    h.size = 1;

    while (h.size > 0) {
        HeapNode node = heap_extract_min(&h);
        int u = node.vertex;
        in_mst[u] = true;

        for (AdjNode *p = g->array[u]; p != NULL; p = p->next) {
            int v = p->dest;
            if (!in_mst[v] && p->weight < mst_weight[v]) {
                mst_weight[v] = p->weight;
                mst_parent[v] = u;

                if (h.pos[v] == -1) {
                    h.data[h.size] = (HeapNode){v, mst_weight[v]};
                    h.pos[v] = h.size++;
                }
                heapify_up(&h, h.pos[v]);
            }
        }
    }
}

/* 时间复杂度: O(|E|log|V|), 空间复杂度: O(|V|) */
```

### 5.2 Kruskal算法（并查集）

**形式化**: 按边权从小到大选择，不形成环则加入MST。

```c
/* 并查集 */
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
        uf->parent[x] = uf_find(uf, uf->parent[x]);  // 路径压缩
    }
    return uf->parent[x];
}

void uf_union(UnionFind *uf, int x, int y) {
    int px = uf_find(uf, x);
    int py = uf_find(uf, y);
    if (px == py) return;

    // 按秩合并
    if (uf->rank[px] < uf->rank[py]) {
        uf->parent[px] = py;
    } else if (uf->rank[px] > uf->rank[py]) {
        uf->parent[py] = px;
    } else {
        uf->parent[py] = px;
        uf->rank[px]++;
    }
}

/* 边比较函数 (用于qsort) */
int edge_compare(const void *a, const void *b) {
    return ((Edge*)a)->weight - ((Edge*)b)->weight;
}

/* Kruskal算法 */
int kruskal_mst(EdgeListGraph *g, Edge mst[]) {
    UnionFind uf;
    uf_init(&uf, g->num_vertices);

    // 按权重排序边
    qsort(g->edges, g->num_edges, sizeof(Edge), edge_compare);

    int mst_size = 0;
    for (int i = 0; i < g->num_edges && mst_size < g->num_vertices - 1; i++) {
        int u = g->edges[i].src;
        int v = g->edges[i].dest;

        if (uf_find(&uf, u) != uf_find(&uf, v)) {
            mst[mst_size++] = g->edges[i];
            uf_union(&uf, u, v);
        }
    }

    return mst_size;
}

/* 时间复杂度: O(|E|log|E|), 空间复杂度: O(|V|) */
```

### 5.3 Boruvka算法

**形式化**: 并行选择每个连通分量的最小出边。

```c
/* Boruvka算法 */
int boruvka_mst(EdgeListGraph *g, Edge mst[]) {
    UnionFind uf;
    uf_init(&uf, g->num_vertices);

    int components = g->num_vertices;
    int mst_size = 0;

    while (components > 1) {
        Edge min_edge[MAX_VERTICES];
        for (int i = 0; i < g->num_vertices; i++) {
            min_edge[i] = (Edge){-1, -1, INF};
        }

        // 为每个连通分量找最小出边
        for (int i = 0; i < g->num_edges; i++) {
            int u = g->edges[i].src;
            int v = g->edges[i].dest;
            int pu = uf_find(&uf, u);
            int pv = uf_find(&uf, v);

            if (pu != pv) {
                if (g->edges[i].weight < min_edge[pu].weight) {
                    min_edge[pu] = g->edges[i];
                }
                if (g->edges[i].weight < min_edge[pv].weight) {
                    min_edge[pv] = g->edges[i];
                }
            }
        }

        // 添加找到的最小边
        for (int i = 0; i < g->num_vertices; i++) {
            if (min_edge[i].src != -1) {
                int u = min_edge[i].src;
                int v = min_edge[i].dest;
                if (uf_find(&uf, u) != uf_find(&uf, v)) {
                    mst[mst_size++] = min_edge[i];
                    uf_union(&uf, u, v);
                    components--;
                }
            }
        }
    }

    return mst_size;
}

/* 时间复杂度: O(|E|log|V|), 空间复杂度: O(|V|) */
```

---

## 6. 网络流算法

### 6.1 Ford-Fulkerson方法

**形式化**: 对于流网络 $G=(V,E,c,s,t)$，$c(u,v)$为容量，求最大流。

```c
/* 残量图边 */
typedef struct ResidualEdge {
    int to;
    int capacity;
    int flow;
    struct ResidualEdge *reverse;
    struct ResidualEdge *next;
} ResidualEdge;

typedef struct {
    int n;
    ResidualEdge *adj[MAX_VERTICES];
} FlowGraph;

/* DFS找增广路 */
int dfs_augment(FlowGraph *g, int u, int t, int min_cap, bool visited[]) {
    if (u == t) return min_cap;
    visited[u] = true;

    for (ResidualEdge *e = g->adj[u]; e != NULL; e = e->next) {
        if (!visited[e->to] && e->capacity > e->flow) {
            int bottleneck = dfs_augment(g, e->to, t,
                min_cap < e->capacity - e->flow ? min_cap : e->capacity - e->flow,
                visited);
            if (bottleneck > 0) {
                e->flow += bottleneck;
                e->reverse->flow -= bottleneck;
                return bottleneck;
            }
        }
    }
    return 0;
}

/* Ford-Fulkerson */
int ford_fulkerson(FlowGraph *g, int s, int t) {
    int max_flow = 0;
    bool visited[MAX_VERTICES];

    while (1) {
        memset(visited, 0, sizeof(visited));
        int bottleneck = dfs_augment(g, s, t, INF, visited);
        if (bottleneck == 0) break;
        max_flow += bottleneck;
    }

    return max_flow;
}

/* 时间复杂度: O(|E|·|f*|), f*为最大流值 */
```

### 6.2 Edmonds-Karp算法

**形式化**: 使用BFS找最短增广路，保证多项式时间复杂度。

```c
/* Edmonds-Karp (BFS找增广路) */
int edmonds_karp(FlowGraph *g, int s, int t) {
    int max_flow = 0;
    int parent[MAX_VERTICES];

    while (1) {
        memset(parent, -1, sizeof(parent));
        Queue q;
        queue_init(&q);
        queue_push(&q, s);
        parent[s] = s;

        // BFS找最短增广路
        while (!queue_empty(&q) && parent[t] == -1) {
            int u = queue_pop(&q);
            for (ResidualEdge *e = g->adj[u]; e != NULL; e = e->next) {
                if (parent[e->to] == -1 && e->capacity > e->flow) {
                    parent[e->to] = u;
                    queue_push(&q, e->to);
                }
            }
        }

        if (parent[t] == -1) break;  // 无增广路

        // 找瓶颈容量
        int bottleneck = INF;
        for (int v = t; v != s; v = parent[v]) {
            for (ResidualEdge *e = g->adj[parent[v]]; e != NULL; e = e->next) {
                if (e->to == v) {
                    bottleneck = (bottleneck < e->capacity - e->flow) ?
                                 bottleneck : e->capacity - e->flow;
                    break;
                }
            }
        }

        // 更新流量
        for (int v = t; v != s; v = parent[v]) {
            for (ResidualEdge *e = g->adj[parent[v]]; e != NULL; e = e->next) {
                if (e->to == v) {
                    e->flow += bottleneck;
                    e->reverse->flow -= bottleneck;
                    break;
                }
            }
        }

        max_flow += bottleneck;
    }

    return max_flow;
}

/* 时间复杂度: O(|V|·|E|²), 空间复杂度: O(|V|) */
```

### 6.3 Dinic算法

**形式化**: 分层图 + 阻塞流，效率更高。

```c
/* Dinic算法 */

/* 构建分层图 */
bool dinic_bfs(FlowGraph *g, int s, int t, int level[]) {
    memset(level, -1, sizeof(int) * g->n);
    Queue q;
    queue_init(&q);
    level[s] = 0;
    queue_push(&q, s);

    while (!queue_empty(&q)) {
        int u = queue_pop(&q);
        for (ResidualEdge *e = g->adj[u]; e != NULL; e = e->next) {
            if (level[e->to] < 0 && e->capacity > e->flow) {
                level[e->to] = level[u] + 1;
                queue_push(&q, e->to);
            }
        }
    }
    return level[t] >= 0;
}

/* DFS发送阻塞流 */
int dinic_dfs(FlowGraph *g, int u, int t, int f, int level[], int iter[]) {
    if (u == t) return f;

    for (ResidualEdge *e = g->adj[u]; e != NULL; e = e->next) {
        if (e->capacity > e->flow && level[u] < level[e->to]) {
            int d = dinic_dfs(g, e->to, t,
                f < e->capacity - e->flow ? f : e->capacity - e->flow,
                level, iter);
            if (d > 0) {
                e->flow += d;
                e->reverse->flow -= d;
                return d;
            }
        }
    }
    return 0;
}

int dinic(FlowGraph *g, int s, int t) {
    int max_flow = 0;
    int level[MAX_VERTICES];
    int iter[MAX_VERTICES];

    while (dinic_bfs(g, s, t, level)) {
        memset(iter, 0, sizeof(iter));
        int f;
        while ((f = dinic_dfs(g, s, t, INF, level, iter)) > 0) {
            max_flow += f;
        }
    }

    return max_flow;
}

/* 时间复杂度: O(|V|²·|E|), 对于单位容量: O(min(|V|^{2/3}, |E|^{1/2})·|E|) */
```

---

## 7. 匹配算法

### 7.1 匈牙利算法（二分图最大匹配）

**形式化**: 对于二分图 $G=(U,V,E)$，找到最大基数匹配。

```c
/* 匈牙利算法 - DFS实现 */
bool hungarian_dfs(AdjListGraph *g, int u, bool visited[], int match[]) {
    for (AdjNode *p = g->array[u]; p != NULL; p = p->next) {
        int v = p->dest;
        if (!visited[v]) {
            visited[v] = true;
            if (match[v] == -1 || hungarian_dfs(g, match[v], visited, match)) {
                match[v] = u;
                return true;
            }
        }
    }
    return false;
}

int hungarian(AdjListGraph *g, int left_size) {
    int match[MAX_VERTICES];
    memset(match, -1, sizeof(match));
    int result = 0;

    for (int u = 0; u < left_size; u++) {
        bool visited[MAX_VERTICES] = {false};
        if (hungarian_dfs(g, u, visited, match)) {
            result++;
        }
    }

    return result;
}

/* 时间复杂度: O(|V|·|E|), 空间复杂度: O(|V|) */
```

### 7.2 KM算法（二分图最大权匹配）

**形式化**: 对于带权二分图，找到总权重最大的完美匹配。

```c
/* KM算法 */
#define KM_INF 0x3f3f3f3f

int km_match[MAX_VERTICES];      // 匹配数组
int km_lx[MAX_VERTICES];         // 左部顶标
int km_ly[MAX_VERTICES];         // 右部顶标
bool km_s[MAX_VERTICES];         // S集合
bool km_t[MAX_VERTICES];         // T集合
int km_slack[MAX_VERTICES];      // 松弛量
int km_slack_x[MAX_VERTICES];    // 达到最小松弛量的x

bool km_dfs(int u, int n, int w[][MAX_VERTICES]) {
    km_s[u] = true;
    for (int v = 0; v < n; v++) {
        if (km_t[v]) continue;
        int gap = km_lx[u] + km_ly[v] - w[u][v];
        if (gap == 0) {
            km_t[v] = true;
            if (km_match[v] == -1 || km_dfs(km_match[v], n, w)) {
                km_match[v] = u;
                return true;
            }
        } else if (gap < km_slack[v]) {
            km_slack[v] = gap;
            km_slack_x[v] = u;
        }
    }
    return false;
}

int km_algorithm(int n, int w[][MAX_VERTICES]) {
    memset(km_match, -1, sizeof(km_match));
    memset(km_ly, 0, sizeof(km_ly));

    // 初始化顶标
    for (int i = 0; i < n; i++) {
        km_lx[i] = -KM_INF;
        for (int j = 0; j < n; j++) {
            if (w[i][j] > km_lx[i]) km_lx[i] = w[i][j];
        }
    }

    for (int u = 0; u < n; u++) {
        for (int i = 0; i < n; i++) {
            km_slack[i] = KM_INF;
            km_slack_x[i] = -1;
        }

        while (1) {
            memset(km_s, 0, sizeof(km_s));
            memset(km_t, 0, sizeof(km_t));
            if (km_dfs(u, n, w)) break;

            // 调整顶标
            int delta = KM_INF;
            for (int i = 0; i < n; i++) {
                if (!km_t[i] && km_slack[i] < delta) {
                    delta = km_slack[i];
                }
            }

            for (int i = 0; i < n; i++) {
                if (km_s[i]) km_lx[i] -= delta;
                if (km_t[i]) km_ly[i] += delta;
                else km_slack[i] -= delta;
            }
        }
    }

    // 计算总权重
    int result = 0;
    for (int i = 0; i < n; i++) {
        if (km_match[i] != -1) {
            result += w[km_match[i]][i];
        }
    }
    return result;
}

/* 时间复杂度: O(|V|³), 空间复杂度: O(|V|²) */
```

---

## 8. 深入理解

### 8.1 算法正确性证明概要

**Dijkstra算法正确性**（贪心选择性质）：

> **定理**: 当顶点 $u$ 从优先队列中取出时，$dist[u]$ 已是最短距离。
>
> **证明**: 反证法。假设存在更短路径 $P$ 从 $s$ 到 $u$，则 $P$ 必经过某个在队列中的顶点 $v$。由于边权非负，$dist[v] + \delta(v,u) \geq dist[v] \geq dist[u]$，矛盾。

**Kruskal算法正确性**（割性质）：

> **定理**: 设 $S$ 为图的一个割，$e$ 为横跨该割的最小权边，则存在包含 $e$ 的MST。
>
> **证明**: 假设某MST $T$ 不包含 $e$，则 $T \cup \{e\}$ 形成环，该环必含另一条横跨割的边 $e'$。用 $e$ 替换 $e'$ 得到更小的生成树，矛盾。

**Bellman-Ford正确性**（松弛定理）：

> **定理**: 经过 $k$ 轮松弛后，所有最多包含 $k$ 条边的最短路径已被找到。
>
> **证明**: 归纳法。最短路径最多含 $|V|-1$ 条边，故需 $|V|-1$ 轮。

### 8.2 复杂度分析总结

| 算法 | 时间复杂度 | 空间复杂度 | 适用条件 |
|------|-----------|-----------|---------|
| DFS/BFS | $O(|V|+|E|)$ | $O(|V|)$ | 遍历、连通性检测 |
| 拓扑排序 | $O(|V|+|E|)$ | $O(|V|)$ | DAG |
| Tarjan SCC | $O(|V|+|E|)$ | $O(|V|)$ | 有向图SCC |
| Dijkstra(堆) | $O((|V|+|E|)\log|V|)$ | $O(|V|)$ | 非负权图 |
| Bellman-Ford | $O(|V|\cdot|E|)$ | $O(|V|)$ | 含负权边 |
| SPFA | $O(|E|)$ 平均 | $O(|V|)$ | 稀疏图、负权边 |
| Floyd-Warshall | $O(|V|^3)$ | $O(|V|^2)$ | 全源最短路径 |
| A* | $O(|E|)$ 启发依赖 | $O(|V|)$ | 有启发信息 |
| Prim(堆) | $O(|E|\log|V|)$ | $O(|V|)$ | 稠密图MST |
| Kruskal | $O(|E|\log|E|)$ | $O(|V|)$ | 稀疏图MST |
| Dinic | $O(|V|^2\cdot|E|)$ | $O(|V|)$ | 最大流问题 |
| 匈牙利 | $O(|V|\cdot|E|)$ | $O(|V|)$ | 二分图匹配 |
| KM | $O(|V|^3)$ | $O(|V|^2)$ | 最大权匹配 |

### 8.3 实际应用场景

| 算法 | 应用场景 |
|------|---------|
| Dijkstra | GPS导航、网络路由协议(OSPF)、游戏寻路 |
| Bellman-Ford | 货币套利检测、距离向量路由(RIP) |
| Floyd-Warshall | 全对最短路径预计算、传递闭包 |
| Prim/Kruskal | 网络设计(电缆铺设)、聚类分析 |
| Dinic | 二分图匹配、图像分割、任务调度 |
| 拓扑排序 | 任务调度、编译依赖分析、课程先修关系 |
| SCC | 网页链接分析、程序依赖分析 |

---

## 9. 思维表征

### 9.1 算法选择决策树

```
图算法选择决策树:
                        ┌─────────────────┐
                        │   问题类型?      │
                        └────────┬────────┘
                                 │
            ┌────────────────────┼────────────────────┐
            ▼                    ▼                    ▼
    ┌───────────────┐    ┌───────────────┐    ┌───────────────┐
    │   遍历/连通性  │    │   最短路径    │    │   其他优化    │
    └───────┬───────┘    └───────┬───────┘    └───────┬───────┘
            │                    │                    │
      ┌─────┴─────┐       ┌──────┴──────┐      ┌──────┴──────┐
      ▼           ▼       ▼             ▼      ▼             ▼
  ┌──────┐   ┌──────┐ ┌─────┐      ┌─────┐ ┌─────┐      ┌─────┐
  │ 图小 │   │ 图大 │ │ 全源│      │ 单源│ │ MST │      │ 流  │
  └─┬────┘   └─┬────┘ └─┬───┘      └─┬───┘ └─┬───┘      └─┬───┘
    │          │        │            │       │            │
    ▼          ▼        ▼            ▼       ▼            ▼
  DFS/BFS   BFS+队列  Floyd      负权边?  Kruskal    二分图?
                          │        /    \    (稀疏)       /    \
                          │       是     否             是      否
                          │       │      │              │       │
                          │       ▼      ▼              ▼       ▼
                          │   Bellman- Dijkstra    匈牙利   Dinic
                          │      Ford   (堆优化)     /KM      /ISAP
                          │       │
                          └───────┘
```

### 9.2 图表示对比矩阵

| 特性 | 邻接矩阵 | 邻接表 | 边列表 | CSR |
|------|---------|--------|--------|-----|
| 空间复杂度 | $O(|V|^2)$ | $O(|V|+|E|)$ | $O(|E|)$ | $O(|V|+|E|)$ |
| 查询边(u,v) | $O(1)$ | $O(deg(u))$ | $O(|E|)$ | $O(log(deg(u)))$ |
| 遍历邻接点 | $O(|V|)$ | $O(deg(u))$ | $O(|E|)$ | $O(deg(u))$ |
| 添加边 | $O(1)$ | $O(1)$ | $O(1)$ | $O(|E|)$ |
| 删除边 | $O(1)$ | $O(deg(u))$ | $O(|E|)$ | $O(|E|)$ |
| 缓存局部性 | 好 | 一般 | 差 | 优秀 |
| 适用场景 | 稠密图 | 一般图 | Kruskal | 大规模稀疏图 |

### 9.3 最短路径算法对比表

| 特性 | Dijkstra | Bellman-Ford | SPFA | Floyd-Warshall |
|------|----------|--------------|------|----------------|
| 时间复杂度 | $O(E\log V)$ | $O(VE)$ | $O(E)$ 平均 | $O(V^3)$ |
| 负权边 | ✗ | ✓ | ✓ | ✓ |
| 负权环检测 | ✗ | ✓ | ✓ | ✓ |
| 单源/全源 | 单源 | 单源 | 单源 | 全源 |
| 数据结构 | 堆 | 数组 | 队列 | 矩阵 |
| 稀疏图性能 | 优秀 | 一般 | 优秀 | 较差 |
| 稠密图性能 | 良好 | 较差 | 较差 | 良好 |

### 9.4 算法关系图谱

```
图算法核心关系:

                    ┌─────────────┐
                    │  图的表示    │
                    │ (矩阵/表/CSR)│
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           ▼               ▼               ▼
    ┌─────────────┐ ┌─────────────┐ ┌─────────────┐
    │  DFS/BFS    │ │  拓扑排序    │ │  并查集     │
    │  (遍历基础)  │ │  (DAG特性)   │ │  (连通性)   │
    └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
           │               │               │
           ▼               ▼               ▼
    ┌─────────────┐ ┌─────────────┐ ┌─────────────┐
    │ 强连通分量   │ │  关键路径   │ │ Kruskal     │
    │(Tarjan/Kos) │ │  (AOE网)    │ │ (MST基础)   │
    └─────────────┘ └─────────────┘ └──────┬──────┘
                                           │
              ┌────────────────────────────┼────────────────────────────┐
              │                            │                            │
              ▼                            ▼                            ▼
       ┌─────────────┐             ┌─────────────┐               ┌─────────────┐
       │  Dijkstra   │             │    Prim     │               │   Dinic     │
       │  (贪心+堆)   │             │  (贪心+堆)   │               │ (分层+阻塞)  │
       └──────┬──────┘             └─────────────┘               └──────┬──────┘
              │                                                        │
              ▼                                                        ▼
       ┌─────────────┐                                          ┌─────────────┐
       │     A*      │                                          │  匈牙利算法  │
       │ (启发搜索)  │                                          │ (二分匹配)   │
       └─────────────┘                                          └──────┬──────┘
                                                                       │
                                                                       ▼
                                                                ┌─────────────┐
                                                                │   KM算法    │
                                                                │ (最大权匹配) │
                                                                └─────────────┘
```

---

## 10. 层次映射

### 10.1 图算法与树算法关系

**树是图的特殊形式**: 树是连通无环图，满足 $|E| = |V| - 1$。

| 图算法 | 对应树算法 | 关系说明 |
|--------|-----------|---------|
| DFS遍历 | 先序/中序/后序遍历 | DFS在树上的特例 |
| BFS遍历 | 层次遍历 | BFS在树上的应用 |
| Dijkstra | 从根到叶路径 | 带权树上的最短路径 |
| Prim | 构建树的过程 | 逐步构建MST |
| 并查集 | 森林表示 | 维护连通分量森林 |

```c
/* 树是图的子集示例: 树的DFS就是图的DFS去掉环检测 */
void tree_dfs(TreeNode *root) {
    if (!root) return;
    printf("%d ", root->val);      // 访问节点
    tree_dfs(root->left);          // 递归左子树
    tree_dfs(root->right);         // 递归右子树
}

/* 等价于图的DFS在无环情况下的简化 */
void graph_dfs_on_tree(AdjListGraph *g, int v, bool visited[]) {
    visited[v] = true;
    printf("%d ", v);
    for (AdjNode *p = g->array[v]; p != NULL; p = p->next) {
        if (!visited[p->dest]) {   // 树无回边，此检查足够
            graph_dfs_on_tree(g, p->dest, visited);
        }
    }
}
```

### 10.2 图算法与动态规划关系

| 算法 | DP视角 | 状态定义 | 转移方程 |
|------|--------|---------|---------|
| **Floyd-Warshall** | 区间DP | $dp[k][i][j]$ | $dp[k][i][j] = \min(dp[k-1][i][j], dp[k-1][i][k] + dp[k-1][k][j])$ |
| **Dijkstra** | 贪心DP | $dist[v]$ | $dist[v] = \min_{u}(dist[u] + w(u,v))$ |
| **Bellman-Ford** | 序列DP | $dp[i][v]$ | $dp[i][v] = \min(dp[i-1][v], \min_{u}(dp[i-1][u] + w(u,v)))$ |
| **最短路径DAG** | 拓扑序DP | $dist[v]$ | 按拓扑序松弛 |
| **KM算法** | 对偶问题 | 顶标 $lx, ly$ | 维护可行性条件 $lx[u] + ly[v] \geq w[u][v]$ |

```c
/* Floyd-Warshall的DP本质展示 */
void floyd_dp_perspective(int dist[][MAX_VERTICES], int n) {
    // dp[k][i][j]: 只使用前k个顶点作为中间点时i到j的最短距离
    // 空间优化后降维为dist[i][j]

    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                // 状态转移: 要么不经过k，要么经过k
                // dp[k][i][j] = min(dp[k-1][i][j], dp[k-1][i][k] + dp[k-1][k][j])
                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }
}
```

### 10.3 学习路径建议

```
图算法学习进阶路径:

Level 1: 基础
  ├── 图的基本概念 (定义、分类)
  ├── 图的存储表示 (邻接矩阵/表)
  ├── DFS/BFS遍历
  └── 简单应用 (连通性、二分图检测)

Level 2: 核心
  ├── 拓扑排序 (Kahn/DFS)
  ├── 最短路径 (Dijkstra/Bellman-Ford)
  └── 最小生成树 (Prim/Kruskal)

Level 3: 进阶
  ├── 强连通分量 (Tarjan/Kosaraju)
  ├── 网络流 (Dinic/最小割)
  └── 匹配算法 (匈牙利/KM)

Level 4: 综合
  ├── 图的高级数据结构 (树链剖分、LCT)
  ├── 随机化算法 (遗传算法、模拟退火)
  └── 实际工程应用 (大规模图处理)
```

---

## 总结

图算法是计算机科学的核心内容，涵盖了从基础遍历到复杂网络优化的各类问题。掌握图算法需要：

1. **理解形式化定义**：图 $G=(V,E)$ 是抽象数据结构的基础
2. **选择合适表示**：邻接矩阵适合稠密图，邻接表适合稀疏图
3. **掌握核心算法**：遍历、最短路径、MST是应用最广的基础
4. **理解算法本质**：贪心、DP、搜索是图算法的三大范式
5. **注重实践应用**：结合实际场景选择合适的算法实现

**关键要点**：

- 稠密图 $\rightarrow$ 邻接矩阵 + Floyd/MST
- 稀疏图 $\rightarrow$ 邻接表 + Dijkstra/SPFA
- 负权边 $\rightarrow$ Bellman-Ford/SPFA
- 全源最短 $\rightarrow$ Floyd-Warshall (小图) / Johnson (大图)
- 网络流问题 $\rightarrow$ Dinic是工程首选
- 二分匹配 $\rightarrow$ 匈牙利算法简洁高效

---

*文档版本: 1.0 | 标准: C11/C17 | 最后更新: 2026-03-28*
