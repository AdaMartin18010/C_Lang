/*
 * Auto-generated from: 12_Practice_Exercises\03_Data_Structure_Projects.md
 * Line: 225
 * Language: c
 * Block ID: bf8f4f43
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct Graph Graph;
typedef struct {
    int from;
    int to;
    double weight;
} Edge;

// 创建和销毁
Graph* graph_create(int num_vertices, bool directed);
void graph_destroy(Graph *g);

// 修改
void graph_add_edge(Graph *g, int from, int to, double weight);
void graph_remove_edge(Graph *g, int from, int to);

// 查询
bool graph_has_edge(const Graph *g, int from, int to);
double graph_get_weight(const Graph *g, int from, int to);
int graph_vertex_count(const Graph *g);
int graph_edge_count(const Graph *g);
