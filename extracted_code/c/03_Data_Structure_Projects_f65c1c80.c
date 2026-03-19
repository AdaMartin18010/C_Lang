/*
 * Auto-generated from: 12_Practice_Exercises\03_Data_Structure_Projects.md
 * Line: 250
 * Language: c
 * Block ID: f65c1c80
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 遍历
void graph_bfs(const Graph *g, int start, void (*visit)(int));
void graph_dfs(const Graph *g, int start, void (*visit)(int));

// 最短路径
double* graph_dijkstra(const Graph *g, int start);
double** graph_floyd_warshall(const Graph *g);

// 最小生成树
Edge* graph_prim_mst(const Graph *g, int *mst_size);
Edge* graph_kruskal_mst(const Graph *g, int *mst_size);

// 拓扑排序
int* graph_topological_sort(const Graph *g, int *result_size);

// 连通分量
int graph_connected_components(const Graph *g, int *component_ids);
