/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\03_Graph_Algorithms_Fundamentals.md
 * Line: 509
 * Language: c
 * Block ID: 5463cf78
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
