/*
 * Auto-generated from: 06_Thinking_Representation\08_Index\README.md
 * Line: 73
 * Language: c
 * Block ID: c22cbe5c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* knowledge_index.h - 知识库索引核心定义 */
#ifndef KNOWLEDGE_INDEX_H
#define KNOWLEDGE_INDEX_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* 文档类型 */
typedef enum {
    DOC_TYPE_CONCEPT = 0,       /* 概念文档 */
    DOC_TYPE_TUTORIAL,          /* 教程 */
    DOC_TYPE_REFERENCE,         /* 参考手册 */
    DOC_TYPE_EXAMPLE,           /* 代码示例 */
    DOC_TYPE_EXERCISE,          /* 练习题 */
    DOC_TYPE_CASE_STUDY,        /* 案例研究 */
    DOC_TYPE_TOOL_GUIDE,        /* 工具指南 */
    DOC_TYPE_BEST_PRACTICE      /* 最佳实践 */
} doc_type_t;

/* 难度等级 */
typedef enum {
    DIFFICULTY_BEGINNER = 1,
    DIFFICULTY_ELEMENTARY,
    DIFFICULTY_INTERMEDIATE,
    DIFFICULTY_ADVANCED,
    DIFFICULTY_EXPERT
} difficulty_t;

/* 文档元数据 */
typedef struct {
    uint32_t doc_id;
    char title[256];
    char path[512];
    char summary[1024];
    doc_type_t type;
    difficulty_t difficulty;
    time_t create_time;
    time_t update_time;
    uint32_t view_count;
    float rating;               /* 评分 0-5 */

    /* 标签 */
    char **tags;
    uint32_t tag_count;

    /* 层级位置 */
    char *category_path;        /* 如: "语言基础/指针/高级指针" */
    uint32_t depth;             /* 层级深度 */
} doc_metadata_t;

/* 倒排索引项 */
typedef struct {
    char *term;                 /* 词项 */
    uint32_t *doc_ids;          /* 包含该词的文档ID列表 */
    uint32_t doc_count;
    uint32_t doc_capacity;
    float *tf_weights;          /* 词频权重 */
} inverted_index_entry_t;

/* 标签索引 */
typedef struct {
    char *tag_name;
    uint32_t *doc_ids;
    uint32_t doc_count;
    uint32_t color_code;        /* 标签颜色 */
    char *description;
} tag_index_entry_t;

/* 前缀树节点 */
typedef struct trie_node {
    char ch;
    bool is_end;
    uint32_t doc_id;
    struct trie_node *children[256];
} trie_node_t;

/* 知识库索引 */
typedef struct {
    const char *name;

    /* 文档存储 */
    doc_metadata_t **documents;
    uint32_t doc_count;
    uint32_t doc_capacity;

    /* 倒排索引 */
    inverted_index_entry_t **inverted_index;
    uint32_t index_size;
    uint32_t index_capacity;

    /* 标签索引 */
    tag_index_entry_t **tag_index;
    uint32_t tag_count;
    uint32_t tag_capacity;

    /* 前缀树 */
    trie_node_t *title_trie;

    /* 层级索引 */
    struct category_node *category_tree;

    /* 统计 */
    uint64_t total_terms;
    uint64_t total_searches;
    double avg_search_time_ms;
} knowledge_index_t;

/* API */
knowledge_index_t *ki_create(const char *name);
void ki_destroy(knowledge_index_t *ki);

/* 文档管理 */
int ki_add_document(knowledge_index_t *ki, doc_metadata_t *doc);
doc_metadata_t *ki_get_document(knowledge_index_t *ki, uint32_t doc_id);
int ki_remove_document(knowledge_index_t *ki, uint32_t doc_id);

/* 索引构建 */
int ki_build_inverted_index(knowledge_index_t *ki);
int ki_build_tag_index(knowledge_index_t *ki);
int ki_build_title_trie(knowledge_index_t *ki);

/* 搜索 */
typedef struct {
    uint32_t doc_id;
    float relevance_score;
    char *snippet;              /* 匹配片段 */
} search_result_t;

search_result_t *ki_search(knowledge_index_t *ki,
                            const char *query,
                            uint32_t *result_count);
search_result_t *ki_search_by_tag(knowledge_index_t *ki,
                                   const char *tag,
                                   uint32_t *result_count);
search_result_t *ki_fuzzy_search(knowledge_index_t *ki,
                                  const char *prefix,
                                  uint32_t *result_count);

/* 自动补全 */
char **ki_get_suggestions(knowledge_index_t *ki,
                           const char *prefix,
                           uint32_t *suggestion_count);

/* 推荐 */
doc_metadata_t **ki_get_related(knowledge_index_t *ki,
                                 uint32_t doc_id,
                                 uint32_t *related_count);

/* 浏览 */
doc_metadata_t **ki_browse_category(knowledge_index_t *ki,
                                     const char *category_path,
                                     uint32_t *doc_count);

#endif /* KNOWLEDGE_INDEX_H */
