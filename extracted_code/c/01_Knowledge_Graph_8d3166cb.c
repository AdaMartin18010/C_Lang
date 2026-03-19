/*
 * Auto-generated from: 06_Thinking_Representation\07_Knowledge_Graph\01_Knowledge_Graph.md
 * Line: 287
 * Language: c
 * Block ID: 8d3166cb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * C语言知识图谱的邻接表实现
 * 使用邻接表表示有向图，支持权重
 * 标准: C17
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NODES 100
#define MAX_NAME_LEN 64

/* 边节点 - 表示概念间的关系 */
typedef struct EdgeNode {
    int dest_id;                /* 目标节点ID */
    int weight;                 /* 关联强度权重 1-10 */
    char relation_type[32];     /* 关系类型: prerequisite|related|extends */
    struct EdgeNode* next;
} EdgeNode;

/* 顶点节点 - 表示知识概念 */
typedef struct VertexNode {
    int id;                     /* 概念ID */
    char name[MAX_NAME_LEN];    /* 概念名称 */
    char category[32];          /* 所属类别 */
    int difficulty;             /* 难度等级 1-5 */
    int layer;                  /* 所属层级 1-6 */
    bool visited;               /* 遍历标记 */
    EdgeNode* first_edge;       /* 第一条边 */
} VertexNode;

/* 知识图谱结构 */
typedef struct KnowledgeGraph {
    VertexNode vertices[MAX_NODES];
    int num_vertices;
    int num_edges;
} KnowledgeGraph;

/* 初始化知识图谱 */
void init_graph(KnowledgeGraph* g) {
    g->num_vertices = 0;
    g->num_edges = 0;
    for (int i = 0; i < MAX_NODES; i++) {
        g->vertices[i].first_edge = NULL;
        g->vertices[i].visited = false;
    }
}

/* 添加知识概念节点 */
int add_vertex(KnowledgeGraph* g, const char* name, const char* category,
               int difficulty, int layer) {
    if (g->num_vertices >= MAX_NODES) {
        fprintf(stderr, "Error: Graph is full\n");
        return -1;
    }

    VertexNode* v = &g->vertices[g->num_vertices];
    v->id = g->num_vertices;
    strncpy(v->name, name, MAX_NAME_LEN - 1);
    v->name[MAX_NAME_LEN - 1] = '\0';
    strncpy(v->category, category, 31);
    v->category[31] = '\0';
    v->difficulty = difficulty;
    v->layer = layer;
    v->visited = false;
    v->first_edge = NULL;

    return g->num_vertices++;
}

/* 添加概念间关系（有向边） */
bool add_edge(KnowledgeGraph* g, int from, int to, int weight,
              const char* relation_type) {
    if (from < 0 || from >= g->num_vertices ||
        to < 0 || to >= g->num_vertices) {
        fprintf(stderr, "Error: Invalid vertex ID\n");
        return false;
    }

    EdgeNode* edge = (EdgeNode*)malloc(sizeof(EdgeNode));
    if (!edge) {
        perror("malloc failed");
        return false;
    }

    edge->dest_id = to;
    edge->weight = weight;
    strncpy(edge->relation_type, relation_type, 31);
    edge->relation_type[31] = '\0';
    edge->next = g->vertices[from].first_edge;
    g->vertices[from].first_edge = edge;

    g->num_edges++;
    return true;
}

/* 打印知识图谱 */
void print_graph(const KnowledgeGraph* g) {
    printf("\n========== C语言知识图谱 ==========\n");
    printf("概念总数: %d\n", g->num_vertices);
    printf("关系总数: %d\n\n", g->num_edges);

    for (int i = 0; i < g->num_vertices; i++) {
        const VertexNode* v = &g->vertices[i];
        printf("[%d] %s (L%d, 难度%d) [%s]\n",
               v->id, v->name, v->layer, v->difficulty, v->category);

        EdgeNode* e = v->first_edge;
        while (e) {
            printf("    └──> [%d] %s (权重:%d, %s)\n",
                   e->dest_id, g->vertices[e->dest_id].name,
                   e->weight, e->relation_type);
            e = e->next;
        }
    }
}

/* DFS深度优先遍历 - 探索学习路径 */
void dfs_explore(KnowledgeGraph* g, int start_id, int depth) {
    if (start_id < 0 || start_id >= g->num_vertices) return;

    VertexNode* v = &g->vertices[start_id];
    if (v->visited) return;

    v->visited = true;

    /* 打印缩进表示层级 */
    for (int i = 0; i < depth; i++) printf("  ");
    printf("└─ %s (L%d)\n", v->name, v->layer);

    /* 遍历所有前置依赖 */
    EdgeNode* e = v->first_edge;
    while (e) {
        if (strcmp(e->relation_type, "prerequisite") == 0) {
            dfs_explore(g, e->dest_id, depth + 1);
        }
        e = e->next;
    }
}

/* 重置访问标记 */
void reset_visited(KnowledgeGraph* g) {
    for (int i = 0; i < g->num_vertices; i++) {
        g->vertices[i].visited = false;
    }
}

/* 查找概念ID */
int find_vertex_id(const KnowledgeGraph* g, const char* name) {
    for (int i = 0; i < g->num_vertices; i++) {
        if (strcmp(g->vertices[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

/* 获取指定层级的所有概念 */
void get_concepts_by_layer(const KnowledgeGraph* g, int layer) {
    printf("\n========== 第%d层概念 ==========\n", layer);
    int count = 0;
    for (int i = 0; i < g->num_vertices; i++) {
        if (g->vertices[i].layer == layer) {
            printf("  %s [%s]\n", g->vertices[i].name, g->vertices[i].category);
            count++;
        }
    }
    printf("总计: %d个概念\n", count);
}

/* 查找高关联概念（推荐学习） */
void find_related_concepts(const KnowledgeGraph* g, const char* name, int min_weight) {
    int id = find_vertex_id(g, name);
    if (id < 0) {
        printf("概念 '%s' 未找到\n", name);
        return;
    }

    printf("\n与 '%s' 高度相关的概念 (权重>=%d):\n", name, min_weight);

    EdgeNode* e = g->vertices[id].first_edge;
    while (e) {
        if (e->weight >= min_weight) {
            printf("  → %s (权重:%d, %s)\n",
                   g->vertices[e->dest_id].name,
                   e->weight, e->relation_type);
        }
        e = e->next;
    }
}

/* 释放图内存 */
void destroy_graph(KnowledgeGraph* g) {
    for (int i = 0; i < g->num_vertices; i++) {
        EdgeNode* e = g->vertices[i].first_edge;
        while (e) {
            EdgeNode* temp = e;
            e = e->next;
            free(temp);
        }
        g->vertices[i].first_edge = NULL;
    }
    g->num_vertices = 0;
    g->num_edges = 0;
}

/* 构建示例知识图谱 */
void build_sample_graph(KnowledgeGraph* g) {
    init_graph(g);

    /* L1: 语言基础 */
    int var = add_vertex(g, "变量与常量", "基础", 1, 1);
    int type = add_vertex(g, "数据类型", "基础", 1, 1);
    int op = add_vertex(g, "运算符", "基础", 1, 1);
    int ctrl = add_vertex(g, "控制结构", "基础", 2, 1);

    /* L2: 核心机制 */
    int func = add_vertex(g, "函数", "核心", 2, 2);
    int arr = add_vertex(g, "数组", "核心", 2, 2);
    int str = add_vertex(g, "字符串", "核心", 2, 2);
    int ptr = add_vertex(g, "指针基础", "核心", 3, 2);

    /* L3: 复合结构 */
    int st = add_vertex(g, "结构体", "复合", 2, 3);
    int uni = add_vertex(g, "联合体", "复合", 3, 3);

    /* L4: 高级特性 */
    int aptr = add_vertex(g, "高级指针", "高级", 4, 4);
    int mem = add_vertex(g, "内存管理", "高级", 4, 4);

    /* 添加依赖关系 */
    add_edge(g, func, var, 8, "prerequisite");
    add_edge(g, func, type, 7, "prerequisite");
    add_edge(g, arr, type, 9, "prerequisite");
    add_edge(g, arr, var, 6, "related");
    add_edge(g, str, arr, 9, "prerequisite");
    add_edge(g, ptr, var, 10, "prerequisite");
    add_edge(g, ptr, type, 8, "prerequisite");
    add_edge(g, st, type, 8, "prerequisite");
    add_edge(g, aptr, ptr, 10, "prerequisite");
    add_edge(g, aptr, arr, 9, "prerequisite");
    add_edge(g, mem, ptr, 10, "prerequisite");
    add_edge(g, mem, type, 7, "related");
    add_edge(g, ptr, arr, 9, "related");
    add_edge(g, arr, ptr, 8, "related");
}

/* 主函数 - 演示知识图谱操作 */
int main(void) {
    KnowledgeGraph kg;

    /* 构建示例图谱 */
    build_sample_graph(&kg);

    /* 打印完整图谱 */
    print_graph(&kg);

    /* 按层级查看概念 */
    get_concepts_by_layer(&kg, 1);
    get_concepts_by_layer(&kg, 2);

    /* 查找相关概念 */
    find_related_concepts(&kg, "指针基础", 8);

    /* DFS探索学习路径 */
    printf("\n========== 从'内存管理'反向探索学习路径 ==========\n");
    int mem_id = find_vertex_id(&kg, "内存管理");
    dfs_explore(&kg, mem_id, 0);

    /* 清理 */
    destroy_graph(&kg);

    return 0;
}
