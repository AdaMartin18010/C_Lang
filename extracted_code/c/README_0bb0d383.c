/*
 * Auto-generated from: 06_Thinking_Representation\07_Knowledge_Graph\README.md
 * Line: 67
 * Language: c
 * Block ID: 0bb0d383
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* knowledge_graph.h - 知识图谱核心数据结构 */
#ifndef KNOWLEDGE_GRAPH_H
#define KNOWLEDGE_GRAPH_H

#include <stdint.h>
#include <stdbool.h>

/* 实体类型 */
typedef enum {
    ENTITY_CONCEPT = 0,         /* 概念 */
    ENTITY_TECHNIQUE,           /* 技术/方法 */
    ENTITY_TOOL,                /* 工具 */
    ENTITY_STANDARD,            /* 标准/规范 */
    ENTITY_DOMAIN,              /* 应用领域 */
    ENTITY_PROBLEM,             /* 问题/难点 */
    ENTITY_PRACTICE,            /* 实践/项目 */
    ENTITY_RESOURCE             /* 学习资源 */
} entity_type_t;

/* 关系类型 */
typedef enum {
    REL_PREREQUISITE = 0,       /* 前置依赖 */
    REL_RELATED,                /* 相关 */
    REL_CONTAINS,               /* 包含 */
    REL_APPLIES_TO,             /* 应用于 */
    REL_IMPLEMENTS,             /* 实现 */
    REL_CONTRASTS_WITH,         /* 对比 */
    REL_EVOLVED_FROM,           /* 演进自 */
    REL_ALTERNATIVE_TO,         /* 替代方案 */
    REL_RECOMMENDED_BY          /* 推荐 */
} relation_type_t;

/* 难度等级 */
typedef enum {
    LEVEL_FUNDAMENTAL = 1,      /* 基础 */
    LEVEL_INTERMEDIATE = 2,     /* 中级 */
    LEVEL_ADVANCED = 3,         /* 高级 */
    LEVEL_EXPERT = 4            /* 专家 */
} difficulty_level_t;

/* 实体节点 */
typedef struct entity {
    uint32_t id;
    const char *name;
    const char *description;
    entity_type_t type;
    difficulty_level_t difficulty;

    /* 学习属性 */
    uint32_t estimated_hours;   /* 预计学习时长 */
    uint32_t importance;        /* 重要性 1-10 */

    /* 图结构 */
    struct relation **outgoing; /* 出边 */
    uint32_t out_degree;
    struct relation **incoming; /* 入边 */
    uint32_t in_degree;

    /* 属性 */
    struct attribute **attrs;
    uint32_t attr_count;
} entity_t;

/* 关系边 */
typedef struct relation {
    uint32_t id;
    relation_type_t type;
    entity_t *from;
    entity_t *to;
    float weight;               /* 关系权重 0-1 */
    const char *description;
} relation_t;

/* 属性 */
typedef struct attribute {
    const char *key;
    const char *value;
} attribute_t;

/* 知识图谱 */
typedef struct {
    const char *name;
    entity_t **entities;
    uint32_t entity_count;
    uint32_t entity_capacity;

    relation_t **relations;
    uint32_t relation_count;
    uint32_t relation_capacity;

    /* 索引 */
    struct hash_table *name_index;
} knowledge_graph_t;

/* API */
knowledge_graph_t *kg_create(const char *name);
void kg_destroy(knowledge_graph_t *kg);

entity_t *kg_add_entity(knowledge_graph_t *kg,
                         const char *name,
                         entity_type_t type,
                         difficulty_level_t difficulty);

relation_t *kg_add_relation(knowledge_graph_t *kg,
                             entity_t *from,
                             entity_t *to,
                             relation_type_t type,
                             float weight);

entity_t *kg_find_entity(knowledge_graph_t *kg, const char *name);

/* 查询 */
entity_t **kg_find_prerequisites(knowledge_graph_t *kg,
                                  entity_t *target,
                                  uint32_t *count);
entity_t **kg_find_related(knowledge_graph_t *kg,
                            entity_t *entity,
                            relation_type_t rel_type,
                            uint32_t *count);

/* 路径查找 */
entity_t **kg_find_learning_path(knowledge_graph_t *kg,
                                  entity_t *from,
                                  entity_t *to,
                                  uint32_t *path_length);

/* 导出 */
int kg_export_dot(knowledge_graph_t *kg, const char *filename);
int kg_export_json(knowledge_graph_t *kg, const char *filename);

#endif /* KNOWLEDGE_GRAPH_H */
