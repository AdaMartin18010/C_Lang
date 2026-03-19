/*
 * Auto-generated from: 06_Thinking_Representation\08_Index\README.md
 * Line: 508
 * Language: c
 * Block ID: 67fbd16a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* tag_system.c - 标签系统实现 */
#include "knowledge_index.h"
#include <stdlib.h>
#include <string.h>

/* 查找或创建标签 */
static tag_index_entry_t *find_or_create_tag(knowledge_index_t *ki,
                                              const char *tag_name) {
    for (uint32_t i = 0; i < ki->tag_count; i++) {
        if (strcmp(ki->tag_index[i]->tag_name, tag_name) == 0) {
            return ki->tag_index[i];
        }
    }

    /* 创建新标签 */
    if (ki->tag_count >= ki->tag_capacity) {
        ki->tag_capacity *= 2;
        ki->tag_index = realloc(ki->tag_index,
                                 ki->tag_capacity * sizeof(tag_index_entry_t *));
    }

    tag_index_entry_t *tag = calloc(1, sizeof(tag_index_entry_t));
    tag->tag_name = strdup(tag_name);
    tag->doc_capacity = 32;
    tag->doc_ids = calloc(tag->doc_capacity, sizeof(uint32_t));

    ki->tag_index[ki->tag_count++] = tag;
    return tag;
}

int ki_add_tag_to_document(knowledge_index_t *ki,
                            uint32_t doc_id,
                            const char *tag_name) {
    doc_metadata_t *doc = ki_get_document(ki, doc_id);
    if (!doc) return -1;

    /* 添加到文档标签列表 */
    doc->tags = realloc(doc->tags, (doc->tag_count + 1) * sizeof(char *));
    doc->tags[doc->tag_count++] = strdup(tag_name);

    /* 添加到标签索引 */
    tag_index_entry_t *tag = find_or_create_tag(ki, tag_name);

    if (tag->doc_count >= tag->doc_capacity) {
        tag->doc_capacity *= 2;
        tag->doc_ids = realloc(tag->doc_ids,
                               tag->doc_capacity * sizeof(uint32_t));
    }

    tag->doc_ids[tag->doc_count++] = doc_id;
    return 0;
}

search_result_t *ki_search_by_tag(knowledge_index_t *ki,
                                   const char *tag_name,
                                   uint32_t *result_count) {
    /* 查找标签 */
    tag_index_entry_t *tag = NULL;
    for (uint32_t i = 0; i < ki->tag_count; i++) {
        if (strcmp(ki->tag_index[i]->tag_name, tag_name) == 0) {
            tag = ki->tag_index[i];
            break;
        }
    }

    if (!tag) {
        *result_count = 0;
        return NULL;
    }

    /* 返回标签下的所有文档 */
    *result_count = tag->doc_count;
    search_result_t *results = calloc(*result_count, sizeof(search_result_t));

    for (uint32_t i = 0; i < tag->doc_count; i++) {
        results[i].doc_id = tag->doc_ids[i];
        results[i].relevance_score = 1.0; /* 标签匹配固定分数 */

        doc_metadata_t *doc = ki_get_document(ki, tag->doc_ids[i]);
        if (doc) {
            results[i].snippet = strdup(doc->summary);
        }
    }

    return results;
}
