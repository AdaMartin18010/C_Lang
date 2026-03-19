/*
 * Auto-generated from: 06_Thinking_Representation\08_Index\README.md
 * Line: 359
 * Language: c
 * Block ID: af8c26bf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* search.c - 搜索功能实现 */
#include "knowledge_index.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* 搜索结果项 */
typedef struct {
    uint32_t doc_id;
    double score;
} score_entry_t;

/* 比较函数用于排序 */
static int compare_scores(const void *a, const void *b) {
    double diff = ((score_entry_t *)b)->score - ((score_entry_t *)a)->score;
    return (diff > 0) - (diff < 0);
}

search_result_t *ki_search(knowledge_index_t *ki,
                            const char *query,
                            uint32_t *result_count) {
    clock_t start = clock();

    uint32_t query_token_count;
    char **query_tokens = tokenize(query, &query_token_count);

    /* 文档得分累加器 */
    double *doc_scores = calloc(ki->doc_capacity, sizeof(double));
    bool *doc_matched = calloc(ki->doc_capacity, sizeof(bool));

    /* 对每个查询词 */
    for (uint32_t i = 0; i < query_token_count; i++) {
        /* 查找倒排索引 */
        for (uint32_t j = 0; j < ki->index_size; j++) {
            inverted_index_entry_t *entry = ki->inverted_index[j];

            /* 精确匹配或前缀匹配 */
            if (strcmp(entry->term, query_tokens[i]) == 0 ||
                strncmp(entry->term, query_tokens[i], strlen(query_tokens[i])) == 0) {

                /* 累加文档得分 */
                for (uint32_t k = 0; k < entry->doc_count; k++) {
                    uint32_t doc_id = entry->doc_ids[k];
                    doc_scores[doc_id] += entry->tf_weights[k];
                    doc_matched[doc_id] = true;
                }
            }
        }
        free(query_tokens[i]);
    }
    free(query_tokens);

    /* 收集结果 */
    score_entry_t *scores = calloc(ki->doc_count, sizeof(score_entry_t));
    uint32_t match_count = 0;

    for (uint32_t i = 0; i < ki->doc_count; i++) {
        if (doc_matched[ki->documents[i]->doc_id]) {
            scores[match_count].doc_id = ki->documents[i]->doc_id;
            scores[match_count].score = doc_scores[ki->documents[i]->doc_id];
            match_count++;
        }
    }

    /* 排序 */
    qsort(scores, match_count, sizeof(score_entry_t), compare_scores);

    /* 构建结果 */
    *result_count = match_count > 20 ? 20 : match_count; /* 返回前20个 */
    search_result_t *results = calloc(*result_count, sizeof(search_result_t));

    for (uint32_t i = 0; i < *result_count; i++) {
        results[i].doc_id = scores[i].doc_id;
        results[i].relevance_score = (float)scores[i].score;

        /* 生成摘要片段 */
        doc_metadata_t *doc = ki_get_document(ki, scores[i].doc_id);
        if (doc) {
            results[i].snippet = strdup(doc->summary);
        }
    }

    /* 更新统计 */
    clock_t end = clock();
    double search_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    ki->total_searches++;
    ki->avg_search_time_ms = (ki->avg_search_time_ms * (ki->total_searches - 1)
                               + search_time) / ki->total_searches;

    free(doc_scores);
    free(doc_matched);
    free(scores);

    return results;
}
