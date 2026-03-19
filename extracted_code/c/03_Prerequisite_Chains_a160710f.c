/*
 * Auto-generated from: 06_Thinking_Representation\07_Knowledge_Graph\03_Prerequisite_Chains.md
 * Line: 388
 * Language: c
 * Block ID: a160710f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * C语言学习依赖链拓扑排序实现
 * 标准: C17
 *
 * 功能: 根据前置依赖关系，生成合理的学习顺序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TOPICS 100
#define MAX_NAME_LEN 64
#define MAX_PREREQ 10

/* 学习主题结构 */
typedef struct Topic {
    int id;
    char name[MAX_NAME_LEN];
    char description[256];
    int difficulty;          /* 1-5 */
    int estimated_hours;     /* 预计学习时长 */
    int stage;               /* 阶段: 1-4 */
} Topic;

/* 依赖关系边 */
typedef struct Edge {
    int from;
    int to;
    int weight;              /* 依赖强度 1-10 */
} Edge;

/* 依赖图 */
typedef struct DependencyGraph {
    Topic topics[MAX_TOPICS];
    int adj_matrix[MAX_TOPICS][MAX_TOPICS];  /* 邻接矩阵 */
    int in_degree[MAX_TOPICS];                /* 入度 */
    int num_topics;
    int num_edges;
} DependencyGraph;

/* 学习规划结果 */
typedef struct StudyPlan {
    int order[MAX_TOPICS];    /* 学习顺序 */
    int num_stages;           /* 阶段数 */
    int stage_start[10];      /* 每个阶段的起始索引 */
    char stage_names[10][32]; /* 阶段名称 */
} StudyPlan;

/* 初始化图 */
void init_graph(DependencyGraph* g) {
    g->num_topics = 0;
    g->num_edges = 0;
    memset(g->adj_matrix, 0, sizeof(g->adj_matrix));
    memset(g->in_degree, 0, sizeof(g->in_degree));
}

/* 添加学习主题 */
int add_topic(DependencyGraph* g, const char* name, const char* desc,
              int difficulty, int hours, int stage) {
    if (g->num_topics >= MAX_TOPICS) {
        fprintf(stderr, "Error: Too many topics\n");
        return -1;
    }

    Topic* t = &g->topics[g->num_topics];
    t->id = g->num_topics;
    strncpy(t->name, name, MAX_NAME_LEN - 1);
    t->name[MAX_NAME_LEN - 1] = '\0';
    strncpy(t->description, desc, 255);
    t->description[255] = '\0';
    t->difficulty = difficulty;
    t->estimated_hours = hours;
    t->stage = stage;

    return g->num_topics++;
}

/* 添加前置依赖 */
bool add_prerequisite(DependencyGraph* g, int from, int to, int weight) {
    if (from < 0 || from >= g->num_topics ||
        to < 0 || to >= g->num_topics) {
        fprintf(stderr, "Error: Invalid topic ID\n");
        return false;
    }

    if (g->adj_matrix[from][to]) {
        fprintf(stderr, "Warning: Dependency already exists\n");
        return false;
    }

    /* from 必须在 to 之前学习，所以有向边 from -> to */
    g->adj_matrix[from][to] = weight;
    g->in_degree[to]++;
    g->num_edges++;

    return true;
}

/* 拓扑排序 - Kahn算法 */
bool topological_sort(DependencyGraph* g, StudyPlan* plan) {
    int n = g->num_topics;
    int queue[MAX_TOPICS];
    int front = 0, rear = 0;
    int temp_in_degree[MAX_TOPICS];

    /* 复制入度数组 */
    memcpy(temp_in_degree, g->in_degree, sizeof(int) * n);

    /* 将所有入度为0的节点入队 */
    for (int i = 0; i < n; i++) {
        if (temp_in_degree[i] == 0) {
            queue[rear++] = i;
        }
    }

    int count = 0;
    while (front < rear) {
        int u = queue[front++];
        plan->order[count++] = u;

        /* 移除u的所有出边 */
        for (int v = 0; v < n; v++) {
            if (g->adj_matrix[u][v] > 0) {
                temp_in_degree[v]--;
                if (temp_in_degree[v] == 0) {
                    queue[rear++] = v;
                }
            }
        }
    }

    /* 检查是否存在环 */
    if (count != n) {
        fprintf(stderr, "Error: Dependency cycle detected!\n");
        return false;
    }

    return true;
}

/* 按阶段分组学习顺序 */
void organize_by_stage(DependencyGraph* g, StudyPlan* plan) {
    int n = g->num_topics;
    int current_stage = 0;
    int stage_count = 0;

    plan->stage_start[0] = 0;
    strcpy(plan->stage_names[0], "入门阶段");
    strcpy(plan->stage_names[1], "基础阶段");
    strcpy(plan->stage_names[2], "核心阶段");
    strcpy(plan->stage_names[3], "高级阶段");

    for (int i = 0; i < n; i++) {
        int topic_id = plan->order[i];
        int topic_stage = g->topics[topic_id].stage;

        if (topic_stage > current_stage) {
            current_stage = topic_stage;
            stage_count++;
            plan->stage_start[stage_count] = i;
        }
    }

    plan->stage_start[stage_count + 1] = n;
    plan->num_stages = stage_count + 1;
}

/* 打印学习规划 */
void print_study_plan(DependencyGraph* g, StudyPlan* plan) {
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║              C语言学习路径规划（拓扑排序结果）              ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");

    int total_hours = 0;

    for (int s = 0; s < plan->num_stages; s++) {
        int start = plan->stage_start[s];
        int end = plan->stage_start[s + 1];
        int stage_hours = 0;

        printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
        printf("📚 %s (共%d个主题)\n", plan->stage_names[s], end - start);
        printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");

        for (int i = start; i < end; i++) {
            int tid = plan->order[i];
            Topic* t = &g->topics[tid];

            /* 打印难度指示 */
            char difficulty[6];
            for (int d = 0; d < 5; d++) {
                difficulty[d] = (d < t->difficulty) ? '★' : '☆';
            }
            difficulty[5] = '\0';

            printf("  %2d. %-20s 难度:%s  预计:%dh\n",
                   i + 1, t->name, difficulty, t->estimated_hours);
            printf("      └─ %s\n", t->description);

            stage_hours += t->estimated_hours;
        }

        printf("\n  阶段总计: %d小时\n\n", stage_hours);
        total_hours += stage_hours;
    }

    printf("════════════════════════════════════════════════════════════\n");
    printf("📊 学习统计: 共%d个主题, %d个依赖关系, 预计总时长:%d小时\n",
           g->num_topics, g->num_edges, total_hours);
    printf("════════════════════════════════════════════════════════════\n");
}

/* 检查学习可行性 */
bool can_learn(DependencyGraph* g, StudyPlan* plan, int topic_id, bool learned[]) {
    /* 找到该主题在学习顺序中的位置 */
    int pos = -1;
    for (int i = 0; i < g->num_topics; i++) {
        if (plan->order[i] == topic_id) {
            pos = i;
            break;
        }
    }

    if (pos < 0) return false;

    /* 检查所有前置依赖是否已学习 */
    for (int i = 0; i < pos; i++) {
        int pre_id = plan->order[i];
        if (g->adj_matrix[pre_id][topic_id] > 0 && !learned[pre_id]) {
            printf("  ⚠️  前置知识 '%s' 尚未掌握\n", g->topics[pre_id].name);
            return false;
        }
    }

    return true;
}

/* 查找关键路径（最长路径） */
void find_critical_path(DependencyGraph* g, StudyPlan* plan) {
    int n = g->num_topics;
    int dist[MAX_TOPICS];
    int path[MAX_TOPICS];
    int max_dist = 0;
    int end_node = 0;

    /* 初始化距离 */
    for (int i = 0; i < n; i++) {
        dist[i] = 1;
        path[i] = -1;
    }

    /* 按拓扑序计算最长路径 */
    for (int i = 0; i < n; i++) {
        int u = plan->order[i];
        for (int v = 0; v < n; v++) {
            if (g->adj_matrix[u][v] > 0 && dist[v] < dist[u] + 1) {
                dist[v] = dist[u] + 1;
                path[v] = u;
                if (dist[v] > max_dist) {
                    max_dist = dist[v];
                    end_node = v;
                }
            }
        }
    }

    /* 打印关键路径 */
    printf("\n🔑 关键学习路径（最长依赖链，长度=%d）:\n", max_dist);
    int cp[MAX_TOPICS];
    int cp_len = 0;
    int node = end_node;

    while (node != -1) {
        cp[cp_len++] = node;
        node = path[node];
    }

    for (int i = cp_len - 1; i >= 0; i--) {
        printf("  %s", g->topics[cp[i]].name);
        if (i > 0) printf(" → ");
        if ((cp_len - i) % 4 == 0 && i > 0) printf("\n     ");
    }
    printf("\n");
}

/* 构建示例依赖图 */
void build_c_learning_graph(DependencyGraph* g) {
    init_graph(g);

    /* 入门阶段 */
    int t_env = add_topic(g, "开发环境搭建",
                          "安装编译器，配置IDE，编写Hello World",
                          1, 2, 1);

    /* 基础阶段 */
    int t_syntax = add_topic(g, "基本语法",
                             "语句、注释、代码结构",
                             1, 3, 2);
    int t_type = add_topic(g, "数据类型",
                           "整型、浮点型、字符型",
                           1, 4, 2);
    int t_var = add_topic(g, "变量与常量",
                          "变量声明、初始化、常量定义",
                          1, 3, 2);
    int t_op = add_topic(g, "运算符",
                         "算术、关系、逻辑、位运算",
                         2, 4, 2);
    int t_io = add_topic(g, "输入输出",
                         "printf/scanf格式化IO",
                         1, 2, 2);
    int t_ctrl = add_topic(g, "控制结构",
                           "if/switch/for/while",
                           2, 5, 2);

    /* 核心阶段 */
    int t_func = add_topic(g, "函数",
                           "函数定义、参数、返回值、递归",
                           2, 6, 3);
    int t_scope = add_topic(g, "作用域",
                            "局部/全局变量、生命周期",
                            2, 3, 3);
    int t_arr = add_topic(g, "数组",
                          "一维/多维数组、数组初始化",
                          2, 5, 3);
    int t_str = add_topic(g, "字符串",
                          "字符数组、字符串函数",
                          2, 4, 3);
    int t_ptr = add_topic(g, "指针基础",
                          "地址、指针变量、解引用",
                          3, 8, 3);

    /* 高级阶段 */
    int t_struct = add_topic(g, "结构体",
                             "struct定义、访问成员",
                             2, 4, 4);
    int t_ptr_arr = add_topic(g, "指针数组",
                              "指针数组、数组指针",
                              3, 4, 4);
    int t_func_ptr = add_topic(g, "函数指针",
                               "函数指针、回调函数",
                               4, 5, 4);
    int t_mem = add_topic(g, "动态内存",
                          "malloc/free、内存管理",
                          4, 6, 4);
    int t_list = add_topic(g, "链表",
                           "单链表、双链表、操作",
                           4, 8, 4);

    /* 添加依赖关系 */
    add_prerequisite(g, t_syntax, t_type, 8);
    add_prerequisite(g, t_syntax, t_var, 8);
    add_prerequisite(g, t_type, t_var, 10);
    add_prerequisite(g, t_type, t_op, 8);
    add_prerequisite(g, t_var, t_op, 6);
    add_prerequisite(g, t_var, t_io, 7);
    add_prerequisite(g, t_type, t_io, 6);
    add_prerequisite(g, t_op, t_ctrl, 9);
    add_prerequisite(g, t_io, t_ctrl, 5);
    add_prerequisite(g, t_ctrl, t_func, 8);
    add_prerequisite(g, t_var, t_func, 7);
    add_prerequisite(g, t_func, t_scope, 9);
    add_prerequisite(g, t_var, t_arr, 8);
    add_prerequisite(g, t_type, t_arr, 9);
    add_prerequisite(g, t_arr, t_str, 10);
    add_prerequisite(g, t_type, t_str, 7);
    add_prerequisite(g, t_var, t_ptr, 10);
    add_prerequisite(g, t_type, t_ptr, 8);
    add_prerequisite(g, t_arr, t_ptr, 8);
    add_prerequisite(g, t_type, t_struct, 7);
    add_prerequisite(g, t_ptr, t_ptr_arr, 10);
    add_prerequisite(g, t_arr, t_ptr_arr, 9);
    add_prerequisite(g, t_ptr, t_func_ptr, 10);
    add_prerequisite(g, t_func, t_func_ptr, 8);
    add_prerequisite(g, t_ptr, t_mem, 10);
    add_prerequisite(g, t_type, t_mem, 7);
    add_prerequisite(g, t_struct, t_list, 8);
    add_prerequisite(g, t_ptr, t_list, 10);
    add_prerequisite(g, t_mem, t_list, 9);
}

/* 主函数 */
int main(void) {
    DependencyGraph graph;
    StudyPlan plan;

    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║            C语言学习前置依赖链分析系统                      ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");

    /* 构建学习依赖图 */
    build_c_learning_graph(&graph);

    /* 拓扑排序生成学习顺序 */
    if (!topological_sort(&graph, &plan)) {
        fprintf(stderr, "依赖关系存在循环，无法生成学习规划\n");
        return 1;
    }

    /* 按阶段组织 */
    organize_by_stage(&graph, &plan);

    /* 打印学习规划 */
    print_study_plan(&graph, &plan);

    /* 查找关键路径 */
    find_critical_path(&graph, &plan);

    /* 学习可行性检查示例 */
    printf("\n📝 学习可行性检查示例:\n");
    bool learned[MAX_TOPICS] = {false};

    /* 假设已学习前几个主题 */
    for (int i = 0; i < 8; i++) {
        learned[plan.order[i]] = true;
    }

    printf("  检查是否可以学习 '链表':\n");
    can_learn(&graph, &plan, 14, learned);

    printf("\n  现在学习 '动态内存' 后再次检查:\n");
    learned[13] = true;  /* 动态内存 */
    can_learn(&graph, &plan, 14, learned);

    return 0;
}
