/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\03_Graph_Algorithms_Fundamentals.md
 * Line: 265
 * Language: c
 * Block ID: 5429be80
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
