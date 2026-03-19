/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\03_Graph_Algorithms_Fundamentals.md
 * Line: 167
 * Language: c
 * Block ID: 064d9cc5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
