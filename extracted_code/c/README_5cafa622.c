/*
 * Auto-generated from: 06_Thinking_Representation\08_Index\README.md
 * Line: 237
 * Language: c
 * Block ID: 5cafa622
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* inverted_index.c - 倒排索引实现 */
#include "knowledge_index.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* 简单的分词函数 */
static char **tokenize(const char *text, uint32_t *token_count) {
    char **tokens = calloc(256, sizeof(char *));
    *token_count = 0;

    char buffer[256];
    int buf_idx = 0;

    for (const char *p = text; *p; p++) {
        if (isalnum(*p)) {
            buffer[buf_idx++] = tolower(*p);
        } else if (buf_idx > 0) {
            buffer[buf_idx] = '\0';
            tokens[*token_count] = strdup(buffer);
            (*token_count)++;
            buf_idx = 0;
        }
    }

    if (buf_idx > 0) {
        buffer[buf_idx] = '\0';
        tokens[*token_count] = strdup(buffer);
        (*token_count)++;
    }

    return tokens;
}

/* 查找或创建倒排索引项 */
static inverted_index_entry_t *find_or_create_entry(knowledge_index_t *ki,
                                                     const char *term) {
    /* 查找现有项 */
    for (uint32_t i = 0; i < ki->index_size; i++) {
        if (strcmp(ki->inverted_index[i]->term, term) == 0) {
            return ki->inverted_index[i];
        }
    }

    /* 创建新项 */
    if (ki->index_size >= ki->index_capacity) {
        ki->index_capacity *= 2;
        ki->inverted_index = realloc(ki->inverted_index,
                                      ki->index_capacity * sizeof(inverted_index_entry_t *));
    }

    inverted_index_entry_t *entry = calloc(1, sizeof(inverted_index_entry_t));
    entry->term = strdup(term);
    entry->doc_capacity = 16;
    entry->doc_ids = calloc(entry->doc_capacity, sizeof(uint32_t));
    entry->tf_weights = calloc(entry->doc_capacity, sizeof(float));

    ki->inverted_index[ki->index_size++] = entry;
    return entry;
}

/* 添加文档到倒排索引 */
static void add_to_inverted_index(knowledge_index_t *ki,
                                   doc_metadata_t *doc,
                                   const char *content) {
    uint32_t token_count;
    char **tokens = tokenize(content, &token_count);

    /* 统计词频 */
    for (uint32_t i = 0; i < token_count; i++) {
        inverted_index_entry_t *entry = find_or_create_entry(ki, tokens[i]);

        /* 检查文档是否已存在 */
        bool exists = false;
        for (uint32_t j = 0; j < entry->doc_count; j++) {
            if (entry->doc_ids[j] == doc->doc_id) {
                entry->tf_weights[j] += 1.0;
                exists = true;
                break;
            }
        }

        /* 添加新文档 */
        if (!exists) {
            if (entry->doc_count >= entry->doc_capacity) {
                entry->doc_capacity *= 2;
                entry->doc_ids = realloc(entry->doc_ids,
                                          entry->doc_capacity * sizeof(uint32_t));
                entry->tf_weights = realloc(entry->tf_weights,
                                            entry->doc_capacity * sizeof(float));
            }
            entry->doc_ids[entry->doc_count] = doc->doc_id;
            entry->tf_weights[entry->doc_count] = 1.0;
            entry->doc_count++;
        }

        free(tokens[i]);
    }

    free(tokens);
}

/* 计算TF-IDF权重 */
void calculate_tfidf_weights(knowledge_index_t *ki) {
    for (uint32_t i = 0; i < ki->index_size; i++) {
        inverted_index_entry_t *entry = ki->inverted_index[i];

        /* IDF = log(N / df) */
        float idf = log((float)ki->doc_count / entry->doc_count);

        for (uint32_t j = 0; j < entry->doc_count; j++) {
            /* TF-IDF = tf * idf */
            entry->tf_weights[j] *= idf;
        }
    }
}
