/*
 * Auto-generated from: 12_Practice_Exercises\07_Algorithm_Challenges.md
 * Line: 180
 * Language: c
 * Block ID: d39d9446
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    int from, to;
    double weight;
} Edge;

typedef struct {
    int n, m;
    Edge *edges;
} Graph;

// Dijkstra - 单源，非负权
void dijkstra(const Graph *g, int src, double *dist, int *prev);

// Bellman-Ford - 单源，可负权，可检测负环
bool bellman_ford(const Graph *g, int src, double *dist, int *prev);

// SPFA - Bellman-Ford优化
bool spfa(const Graph *g, int src, double *dist, int *prev);

// Floyd-Warshall - 全源
void floyd_warshall(const Graph *g, double **dist, int **next);

// Johnson - 稀疏图全源
void johnson(const Graph *g, double **dist);
