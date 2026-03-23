# 图算法基础 - Graph Algorithms Fundamentals

> **版本**: C23 Standard | **难度**: 中级 | **预计学习时间**: 25小时

---

## 📋 目录

- [图算法基础 - Graph Algorithms Fundamentals](#图算法基础---graph-algorithms-fundamentals)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [1. 图的基本概念](#1-图的基本概念)
    - [1.1 什么是图](#11-什么是图)
    - [1.2 图的分类](#12-图的分类)
    - [1.3 图的基本术语](#13-图的基本术语)
    - [1.4 图的表示对比](#14-图的表示对比)
  - [2. 图的表示方法](#2-图的表示方法)
    - [2.1 邻接矩阵 (Adjacency Matrix)](#21-邻接矩阵-adjacency-matrix)
    - [2.2 邻接表 (Adjacency List)](#22-邻接表-adjacency-list)
    - [2.3 图的表示转换](#23-图的表示转换)
  - [3. 图的遍历](#3-图的遍历)
    - [3.1 深度优先搜索 (DFS - Depth First Search)](#31-深度优先搜索-dfs---depth-first-search)
      - [3.1.1 算法原理](#311-算法原理)
      - [3.1.2 C23实现](#312-c23实现)
    - [3.2 广度优先搜索 (BFS - Breadth First Search)](#32-广度优先搜索-bfs---breadth-first-search)
      - [3.2.1 算法原理](#321-算法原理)
      - [3.2.2 C23实现](#322-c23实现)
    - [3.3 遍历算法对比](#33-遍历算法对比)
  - [4. 最短路径算法](#4-最短路径算法)
    - [4.1 Dijkstra算法](#41-dijkstra算法)
      - [4.1.1 算法原理](#411-算法原理)
      - [4.1.2 C23实现](#412-c23实现)
    - [4.2 Bellman-Ford算法](#42-bellman-ford算法)
    - [4.3 Floyd-Warshall算法](#43-floyd-warshall算法)
    - [4.4 最短路径算法对比](#44-最短路径算法对比)
  - [5. 最小生成树](#5-最小生成树)
    - [5.1 Prim算法](#51-prim算法)
    - [5.2 Kruskal算法](#52-kruskal算法)
    - [5.3 最小生成树算法对比](#53-最小生成树算法对比)
  - [6. 拓扑排序](#6-拓扑排序)
    - [6.1 Kahn算法](#61-kahn算法)
  - [7. 实战应用](#7-实战应用)
    - [7.1 网络路由](#71-网络路由)
    - [7.2 社交网络分析](#72-社交网络分析)
  - [总结](#总结)
    - [核心要点](#核心要点)
    - [学习建议](#学习建议)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---


---

## 📑 目录

- [图算法基础 - Graph Algorithms Fundamentals](#图算法基础---graph-algorithms-fundamentals)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [1. 图的基本概念](#1-图的基本概念)
    - [1.1 什么是图](#11-什么是图)
    - [1.2 图的分类](#12-图的分类)
    - [1.3 图的基本术语](#13-图的基本术语)
    - [1.4 图的表示对比](#14-图的表示对比)
  - [2. 图的表示方法](#2-图的表示方法)
    - [2.1 邻接矩阵 (Adjacency Matrix)](#21-邻接矩阵-adjacency-matrix)
    - [2.2 邻接表 (Adjacency List)](#22-邻接表-adjacency-list)
    - [2.3 图的表示转换](#23-图的表示转换)
  - [3. 图的遍历](#3-图的遍历)
    - [3.1 深度优先搜索 (DFS - Depth First Search)](#31-深度优先搜索-dfs---depth-first-search)
      - [3.1.1 算法原理](#311-算法原理)
      - [3.1.2 C23实现](#312-c23实现)
    - [3.2 广度优先搜索 (BFS - Breadth First Search)](#32-广度优先搜索-bfs---breadth-first-search)
      - [3.2.1 算法原理](#321-算法原理)
      - [3.2.2 C23实现](#322-c23实现)
    - [3.3 遍历算法对比](#33-遍历算法对比)
  - [4. 最短路径算法](#4-最短路径算法)
    - [4.1 Dijkstra算法](#41-dijkstra算法)
      - [4.1.1 算法原理](#411-算法原理)
      - [4.1.2 C23实现](#412-c23实现)
    - [4.2 Bellman-Ford算法](#42-bellman-ford算法)
    - [4.3 Floyd-Warshall算法](#43-floyd-warshall算法)
    - [4.4 最短路径算法对比](#44-最短路径算法对比)
  - [5. 最小生成树](#5-最小生成树)
    - [5.1 Prim算法](#51-prim算法)
    - [5.2 Kruskal算法](#52-kruskal算法)
    - [5.3 最小生成树算法对比](#53-最小生成树算法对比)
  - [6. 拓扑排序](#6-拓扑排序)
    - [6.1 Kahn算法](#61-kahn算法)
  - [7. 实战应用](#7-实战应用)
    - [7.1 网络路由](#71-网络路由)
    - [7.2 社交网络分析](#72-社交网络分析)
  - [总结](#总结)
    - [核心要点](#核心要点)
    - [学习建议](#学习建议)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 1. 图的基本概念

### 1.1 什么是图

图(Graph)是由**顶点(Vertex)**集合和**边(Edge)**集合组成的数据结构，用于表示对象之间的关系。

```
图 G = (V, E)
- V：顶点集合，如 V = {A, B, C, D, E}
- E：边集合，如 E = {(A,B), (B,C), (C,D), (D,E), (E,A)}
```

### 1.2 图的分类

```
图
├── 按方向
│   ├── 无向图 (Undirected)
│   │   └── 边无方向，(A,B) = (B,A)
│   │
│   └── 有向图 (Directed)
│       └── 边有方向，<A,B> ≠ <B,A>
│
├── 按权重
│   ├── 无权图
│   └── 带权图 (Weighted)
│       └── 边有权重/代价
│
├── 按边数
│   ├── 稀疏图 (边数 ≈ 顶点数)
│   └── 稠密图 (边数 ≈ 顶点数²)
│
└── 特殊图
    ├── 完全图 (任意两顶点有边)
    ├── 树 (无环连通图)
    ├── 有向无环图 (DAG)
    └── 二分图
```

### 1.3 图的基本术语

| 术语 | 定义 | 示例 |
|------|------|------|
| **度(Degree)** | 与顶点相连的边数 | A的度为3 |
| **入度(In-degree)** | 指向该顶点的边数 | 有向图中A的入度 |
| **出度(Out-degree)** | 从该顶点出发的边数 | 有向图中A的出度 |
| **路径(Path)** | 顶点序列，相邻顶点有边 | A→B→C→D |
| **环(Cycle)** | 起点和终点相同的路径 | A→B→C→A |
| **连通性** | 顶点间是否可达 | A和C连通 |
| **子图** | 原图的顶点和边的子集 | - |

### 1.4 图的表示对比

| 特性 | 邻接矩阵 | 邻接表 |
|------|---------|--------|
| 空间复杂度 | O(V²) | O(V + E) |
| 添加边 | O(1) | O(1) |
| 删除边 | O(1) | O(E) |
| 检查边(u,v) | O(1) | O(degree(u)) |
| 遍历邻接点 | O(V) | O(degree(u)) |
| 适用场景 | 稠密图 | 稀疏图 |

---

## 2. 图的表示方法

### 2.1 邻接矩阵 (Adjacency Matrix)

```c
/*
 * 图的表示方法
 * 文件名: graph_representation.c
 * 编译: gcc -std=c23 -o graph_representation graph_representation.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_VERTICES 100
#define INF INT_MAX

// ==================== 邻接矩阵表示 ====================

typedef struct {
    int num_vertices;
    int num_edges;
    int adj_matrix[MAX_VERTICES][MAX_VERTICES];
    char vertices[MAX_VERTICES];  // 顶点名称
} AdjMatrixGraph;

// 初始化图
void init_adj_matrix_graph(AdjMatrixGraph *g, int n) {
    g->num_vertices = n;
    g->num_edges = 0;

    for (int i = 0; i < n; i++) {
        g->vertices[i] = 'A' + i;
        for (int j = 0; j < n; j++) {
            g->adj_matrix[i][j] = (i == j) ? 0 : INF;
        }
    }
}

// 添加无向边
void add_undirected_edge_matrix(AdjMatrixGraph *g, int u, int v, int weight) {
    g->adj_matrix[u][v] = weight;
    g->adj_matrix[v][u] = weight;
    g->num_edges++;
}

// 添加有向边
void add_directed_edge_matrix(AdjMatrixGraph *g, int u, int v, int weight) {
    g->adj_matrix[u][v] = weight;
    g->num_edges++;
}

// 删除边
void remove_edge_matrix(AdjMatrixGraph *g, int u, int v) {
    g->adj_matrix[u][v] = INF;
    g->adj_matrix[v][u] = INF;
    g->num_edges--;
}

// 检查边是否存在
bool has_edge_matrix(const AdjMatrixGraph *g, int u, int v) {
    return g->adj_matrix[u][v] != INF && g->adj_matrix[u][v] != 0;
}

// 获取顶点度数（无向图）
int get_degree_matrix(const AdjMatrixGraph *g, int v) {
    int degree = 0;
    for (int i = 0; i < g->num_vertices; i++) {
        if (g->adj_matrix[v][i] != INF && g->adj_matrix[v][i] != 0) {
            degree++;
        }
    }
    return degree;
}

// 打印邻接矩阵
void print_adj_matrix(const AdjMatrixGraph *g) {
    printf("邻接矩阵 (%d个顶点, %d条边):\n", g->num_vertices, g->num_edges);
    printf("    ");
    for (int i = 0; i < g->num_vertices; i++) {
        printf(" %c  ", g->vertices[i]);
    }
    printf("\n");

    for (int i = 0; i < g->num_vertices; i++) {
        printf(" %c |", g->vertices[i]);
        for (int j = 0; j < g->num_vertices; j++) {
            if (g->adj_matrix[i][j] == INF) {
                printf(" ∞ |");
            } else {
                printf(" %2d |", g->adj_matrix[i][j]);
            }
        }
        printf("\n");
    }
}
```

### 2.2 邻接表 (Adjacency List)

```c
// ==================== 邻接表表示 ====================

// 边节点
typedef struct EdgeNode {
    int dest;           // 目标顶点
    int weight;         // 权重
    struct EdgeNode *next;
} EdgeNode;

// 顶点节点
typedef struct {
    char name;
    EdgeNode *head;     // 邻接边链表头
} VertexNode;

// 图
typedef struct {
    int num_vertices;
    int num_edges;
    VertexNode vertices[MAX_VERTICES];
} AdjListGraph;

// 初始化图
void init_adj_list_graph(AdjListGraph *g, int n) {
    g->num_vertices = n;
    g->num_edges = 0;

    for (int i = 0; i < n; i++) {
        g->vertices[i].name = 'A' + i;
        g->vertices[i].head = NULL;
    }
}

// 创建边节点
EdgeNode* create_edge_node(int dest, int weight) {
    EdgeNode *new_node = malloc(sizeof(EdgeNode));
    if (new_node) {
        new_node->dest = dest;
        new_node->weight = weight;
        new_node->next = NULL;
    }
    return new_node;
}

// 添加无向边
void add_undirected_edge_list(AdjListGraph *g, int u, int v, int weight) {
    // 添加 u -> v
    EdgeNode *node1 = create_edge_node(v, weight);
    node1->next = g->vertices[u].head;
    g->vertices[u].head = node1;

    // 添加 v -> u
    EdgeNode *node2 = create_edge_node(u, weight);
    node2->next = g->vertices[v].head;
    g->vertices[v].head = node2;

    g->num_edges++;
}

// 添加有向边
void add_directed_edge_list(AdjListGraph *g, int u, int v, int weight) {
    EdgeNode *node = create_edge_node(v, weight);
    node->next = g->vertices[u].head;
    g->vertices[u].head = node;
    g->num_edges++;
}

// 删除边
void remove_edge_list(AdjListGraph *g, int u, int v) {
    EdgeNode **curr = &g->vertices[u].head;
    while (*curr) {
        if ((*curr)->dest == v) {
            EdgeNode *temp = *curr;
            *curr = (*curr)->next;
            free(temp);
            g->num_edges--;
            return;
        }
        curr = &(*curr)->next;
    }
}

// 检查边是否存在
bool has_edge_list(const AdjListGraph *g, int u, int v) {
    EdgeNode *curr = g->vertices[u].head;
    while (curr) {
        if (curr->dest == v) return true;
        curr = curr->next;
    }
    return false;
}

// 获取顶点度数
int get_degree_list(const AdjListGraph *g, int v) {
    int degree = 0;
    EdgeNode *curr = g->vertices[v].head;
    while (curr) {
        degree++;
        curr = curr->next;
    }
    return degree;
}

// 打印邻接表
void print_adj_list(const AdjListGraph *g) {
    printf("\n邻接表 (%d个顶点, %d条边):\n", g->num_vertices, g->num_edges);

    for (int i = 0; i < g->num_vertices; i++) {
        printf(" %c: ", g->vertices[i].name);
        EdgeNode *curr = g->vertices[i].head;
        while (curr) {
            printf("-> %c(%d) ", g->vertices[curr->dest].name, curr->weight);
            curr = curr->next;
        }
        printf("\n");
    }
}

// 释放图内存
void free_adj_list_graph(AdjListGraph *g) {
    for (int i = 0; i < g->num_vertices; i++) {
        EdgeNode *curr = g->vertices[i].head;
        while (curr) {
            EdgeNode *temp = curr;
            curr = curr->next;
            free(temp);
        }
        g->vertices[i].head = NULL;
    }
}

// 测试主函数
int main(void) {
    printf("=== 图的表示方法演示 ===\n\n");

    // 邻接矩阵测试
    AdjMatrixGraph mat_graph;
    init_adj_matrix_graph(&mat_graph, 5);

    add_undirected_edge_matrix(&mat_graph, 0, 1, 4);  // A-B:4
    add_undirected_edge_matrix(&mat_graph, 0, 2, 2);  // A-C:2
    add_undirected_edge_matrix(&mat_graph, 1, 2, 1);  // B-C:1
    add_undirected_edge_matrix(&mat_graph, 1, 3, 5);  // B-D:5
    add_undirected_edge_matrix(&mat_graph, 2, 3, 8);  // C-D:8
    add_undirected_edge_matrix(&mat_graph, 2, 4, 10); // C-E:10

    print_adj_matrix(&mat_graph);
    printf("\n顶点B的度数: %d\n", get_degree_matrix(&mat_graph, 1));
    printf("边(B,D)存在: %s\n",
           has_edge_matrix(&mat_graph, 1, 3) ? "是" : "否");

    // 邻接表测试
    AdjListGraph list_graph;
    init_adj_list_graph(&list_graph, 5);

    add_undirected_edge_list(&list_graph, 0, 1, 4);
    add_undirected_edge_list(&list_graph, 0, 2, 2);
    add_undirected_edge_list(&list_graph, 1, 2, 1);
    add_undirected_edge_list(&list_graph, 1, 3, 5);
    add_undirected_edge_list(&list_graph, 2, 3, 8);
    add_undirected_edge_list(&list_graph, 2, 4, 10);

    print_adj_list(&list_graph);
    printf("\n顶点B的度数: %d\n", get_degree_list(&list_graph, 1));

    free_adj_list_graph(&list_graph);

    return 0;
}
```

### 2.3 图的表示转换

```c
/**
 * @brief 邻接矩阵转邻接表
 */
void matrix_to_list(const AdjMatrixGraph *mat, AdjListGraph *list) {
    init_adj_list_graph(list, mat->num_vertices);

    for (int i = 0; i < mat->num_vertices; i++) {
        for (int j = 0; j < mat->num_vertices; j++) {
            if (mat->adj_matrix[i][j] != INF && mat->adj_matrix[i][j] != 0) {
                // 为避免重复添加无向边，只处理i < j的情况
                if (i < j) {
                    add_undirected_edge_list(list, i, j, mat->adj_matrix[i][j]);
                }
            }
        }
    }
}

/**
 * @brief 邻接表转邻接矩阵
 */
void list_to_matrix(const AdjListGraph *list, AdjMatrixGraph *mat) {
    init_adj_matrix_graph(mat, list->num_vertices);

    for (int i = 0; i < list->num_vertices; i++) {
        EdgeNode *curr = list->vertices[i].head;
        while (curr) {
            mat->adj_matrix[i][curr->dest] = curr->weight;
            curr = curr->next;
        }
    }

    mat->num_edges = list->num_edges;
}
```

---

## 3. 图的遍历

### 3.1 深度优先搜索 (DFS - Depth First Search)

#### 3.1.1 算法原理

DFS从起始顶点出发，沿着一条路径尽可能深地探索，直到无法继续，然后回溯。

```
图:
    A --- B
    |     |
    C --- D --- E

DFS从A开始:
1. 访问A，标记为已访问
2. 选择邻接点B（第一个未访问的）
3. 访问B，标记为已访问
4. 从B选择D
5. 访问D，标记为已访问
6. 从D选择C
7. 访问C，标记为已访问
8. C的邻接点都已访问，回溯到D
9. 从D选择E
10. 访问E

遍历顺序: A → B → D → C → E
```

#### 3.1.2 C23实现

```c
/*
 * 图遍历算法
 * 文件名: graph_traversal.c
 * 编译: gcc -std=c23 -o graph_traversal graph_traversal.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_VERTICES 100

// 邻接表定义
typedef struct EdgeNode {
    int dest;
    int weight;
    struct EdgeNode *next;
} EdgeNode;

typedef struct {
    char name;
    EdgeNode *head;
} Vertex;

typedef struct {
    int n;
    Vertex vertices[MAX_VERTICES];
} Graph;

// 访问标记数组
bool visited[MAX_VERTICES];

// 初始化图
void init_graph(Graph *g, int n) {
    g->n = n;
    for (int i = 0; i < n; i++) {
        g->vertices[i].name = 'A' + i;
        g->vertices[i].head = NULL;
    }
}

// 添加边
void add_edge(Graph *g, int u, int v, int weight) {
    EdgeNode *node = malloc(sizeof(EdgeNode));
    node->dest = v;
    node->weight = weight;
    node->next = g->vertices[u].head;
    g->vertices[u].head = node;
}

// 添加无向边
void add_undirected_edge(Graph *g, int u, int v, int weight) {
    add_edge(g, u, v, weight);
    add_edge(g, v, u, weight);
}

// ==================== DFS实现 ====================

/**
 * @brief 递归DFS
 */
void dfs_recursive(Graph *g, int v) {
    visited[v] = true;
    printf("%c ", g->vertices[v].name);

    EdgeNode *curr = g->vertices[v].head;
    while (curr) {
        if (!visited[curr->dest]) {
            dfs_recursive(g, curr->dest);
        }
        curr = curr->next;
    }
}

/**
 * @brief 显式栈的DFS
 */
// 栈实现
typedef struct {
    int items[MAX_VERTICES];
    int top;
} Stack;

void stack_init(Stack *s) { s->top = -1; }
bool stack_empty(Stack *s) { return s->top == -1; }
void stack_push(Stack *s, int v) { s->items[++s->top] = v; }
int stack_pop(Stack *s) { return s->items[s->top--]; }

void dfs_iterative(Graph *g, int start) {
    Stack stack;
    stack_init(&stack);

    stack_push(&stack, start);

    while (!stack_empty(&stack)) {
        int v = stack_pop(&stack);

        if (!visited[v]) {
            visited[v] = true;
            printf("%c ", g->vertices[v].name);

            // 将未访问的邻接点入栈
            EdgeNode *curr = g->vertices[v].head;
            while (curr) {
                if (!visited[curr->dest]) {
                    stack_push(&stack, curr->dest);
                }
                curr = curr->next;
            }
        }
    }
}

/**
 * @brief DFS检测环
 */
bool dfs_detect_cycle(Graph *g, int v, int parent) {
    visited[v] = true;

    EdgeNode *curr = g->vertices[v].head;
    while (curr) {
        if (!visited[curr->dest]) {
            if (dfs_detect_cycle(g, curr->dest, v)) {
                return true;
            }
        } else if (curr->dest != parent) {
            // 遇到已访问且不是父节点的节点，说明有环
            return true;
        }
        curr = curr->next;
    }
    return false;
}

/**
 * @brief DFS计算连通分量
 */
void dfs_connected_components(Graph *g) {
    int components = 0;

    for (int i = 0; i < g->n; i++) {
        if (!visited[i]) {
            components++;
            printf("\n连通分量 %d: ", components);
            dfs_recursive(g, i);
        }
    }
    printf("\n总连通分量数: %d\n", components);
}

/**
 * @brief DFS拓扑排序（用于有向无环图）
 */
// 使用栈存储排序结果
typedef struct {
    int items[MAX_VERTICES];
    int top;
} ResultStack;

ResultStack result_stack;

bool dfs_topo_sort(Graph *g, int v, int *rec_stack) {
    visited[v] = true;
    rec_stack[v] = 1;  // 标记在当前递归栈中

    EdgeNode *curr = g->vertices[v].head;
    while (curr) {
        if (!visited[curr->dest]) {
            if (dfs_topo_sort(g, curr->dest, rec_stack)) {
                return true;  // 有环
            }
        } else if (rec_stack[curr->dest]) {
            return true;  // 有环
        }
        curr = curr->next;
    }

    rec_stack[v] = 0;
    result_stack.items[++result_stack.top] = v;
    return false;
}
```

### 3.2 广度优先搜索 (BFS - Breadth First Search)

#### 3.2.1 算法原理

BFS从起始顶点出发，先访问所有邻接点，然后逐层向外扩展。

```
图:
    A --- B
    |     |
    C --- D --- E

BFS从A开始:
1. 访问A，标记为已访问
2. A的邻接点: B, C 加入队列
   队列: [B, C]
3. 出队B，访问B
4. B的邻接点D加入队列
   队列: [C, D]
5. 出队C，访问C
6. C的邻接点都已访问
   队列: [D]
7. 出队D，访问D
8. D的邻接点E加入队列
   队列: [E]
9. 出队E，访问E

遍历顺序: A → B → C → D → E
```

#### 3.2.2 C23实现

```c
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
```

### 3.3 遍历算法对比

| 特性 | DFS | BFS |
|------|-----|-----|
| 数据结构 | 栈 | 队列 |
| 空间复杂度 | O(V) | O(V) |
| 最短路径 | ❌ | ✅（无权图） |
| 适用场景 | 连通性、环检测、拓扑排序 | 最短路径、层序遍历 |
| 实现难度 | 递归简单 | 简单 |

---

## 4. 最短路径算法

### 4.1 Dijkstra算法

#### 4.1.1 算法原理

Dijkstra算法用于求解单源最短路径，适用于非负权边图。

```
核心思想：贪心策略
1. 初始化：起点的距离为0，其他为∞
2. 选择当前距离最小的未访问顶点u
3. 更新u的所有邻接点v的距离：
   dist[v] = min(dist[v], dist[u] + weight(u,v))
4. 标记u为已访问
5. 重复2-4直到所有顶点都访问
```

#### 4.1.2 C23实现

```c
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
```

### 4.2 Bellman-Ford算法

```c
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
```

### 4.3 Floyd-Warshall算法

```c
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
```

### 4.4 最短路径算法对比

| 算法 | 时间复杂度 | 空间复杂度 | 负权边 | 适用场景 |
|------|-----------|-----------|--------|---------|
| Dijkstra（数组） | O(V²) | O(V) | ❌ | 稠密图 |
| Dijkstra（堆） | O((V+E)log V) | O(V) | ❌ | 稀疏图 |
| Bellman-Ford | O(VE) | O(V) | ✅ | 含负权边 |
| SPFA | O(E)平均 | O(V) | ✅ | 稀疏图，含负权边 |
| Floyd-Warshall | O(V³) | O(V²) | ✅ | 全源最短路径 |

---

## 5. 最小生成树

### 5.1 Prim算法

```c
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
```

### 5.2 Kruskal算法

```c
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
```

### 5.3 最小生成树算法对比

| 特性 | Prim | Kruskal |
|------|------|---------|
| 时间复杂度 | O(E log V) 或 O(V²) | O(E log E) |
| 空间复杂度 | O(V) | O(E) |
| 适用图类型 | 稠密图 | 稀疏图 |
| 数据结构 | 优先队列/数组 | 并查集 |
| 起始点 | 需要 | 不需要 |

---

## 6. 拓扑排序

### 6.1 Kahn算法

```c
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
```

---

## 7. 实战应用

### 7.1 网络路由

```c
/**
 * @brief 简化版OSPF路由计算
 */
typedef struct {
    int dest;
    int next_hop;
    int cost;
} RouteEntry;

void calculate_routing_table(int network[MAX_VERTICES][MAX_VERTICES],
                             int n, int router, RouteEntry table[]) {
    int dist[MAX_VERTICES];
    int parent[MAX_VERTICES];

    dijkstra_basic(network, n, router, dist, parent);

    for (int i = 0; i < n; i++) {
        table[i].dest = i;
        table[i].cost = dist[i];

        // 计算下一跳
        if (i == router) {
            table[i].next_hop = -1;
        } else if (parent[i] == router) {
            table[i].next_hop = i;
        } else {
            int curr = i;
            while (parent[curr] != router) {
                curr = parent[curr];
            }
            table[i].next_hop = curr;
        }
    }
}
```

### 7.2 社交网络分析

```c
/**
 * @brief 计算共同好友数
 */
int common_friends(int graph[MAX_VERTICES][MAX_VERTICES],
                   int n, int u, int v) {
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (graph[u][i] && graph[v][i]) {
            count++;
        }
    }
    return count;
}

/**
 * @brief 推荐好友（基于共同好友数）
 */
void recommend_friends(int graph[MAX_VERTICES][MAX_VERTICES],
                       int n, int user, int recommendations[],
                       int *count) {
    int common_count[MAX_VERTICES] = {0};

    for (int i = 0; i < n; i++) {
        if (i != user && !graph[user][i]) {
            common_count[i] = common_friends(graph, n, user, i);
        }
    }

    // 排序推荐（简化版）
    *count = 0;
    for (int i = 0; i < n && *count < 10; i++) {
        if (common_count[i] > 0) {
            recommendations[(*count)++] = i;
        }
    }
}
```

---

## 总结

### 核心要点

| 算法 | 时间复杂度 | 核心思想 | 应用场景 |
|------|-----------|---------|---------|
| DFS | O(V+E) | 深度优先 | 连通性、环检测、拓扑排序 |
| BFS | O(V+E) | 广度优先 | 最短路径（无权）、层序遍历 |
| Dijkstra | O((V+E)log V) | 贪心 | 单源最短路径（非负权） |
| Bellman-Ford | O(VE) | 动态规划 | 含负权边的最短路径 |
| Floyd | O(V³) | 动态规划 | 全源最短路径 |
| Prim | O(E log V) | 贪心 | 稠密图最小生成树 |
| Kruskal | O(E log E) | 贪心+并查集 | 稀疏图最小生成树 |

### 学习建议

1. **动手画图**：图算法的理解离不开画图
2. **编码实现**：自己实现比看代码更有效
3. **分析复杂度**：理解为什么不同算法适用不同场景
4. **实际应用**：尝试将算法应用到实际问题

---

*文档版本: 1.0 | 最后更新: 2026年3月*


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
