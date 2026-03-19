/*
 * Auto-generated from: 06_Thinking_Representation\08_Index\README.md
 * Line: 649
 * Language: c
 * Block ID: bb09881a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* index_maintenance.c */
#include "knowledge_index.h"

/* 增量更新索引 */
int ki_incremental_update(knowledge_index_t *ki) {
    /* 扫描变更 */
    for (uint32_t i = 0; i < ki->doc_count; i++) {
        doc_metadata_t *doc = ki->documents[i];

        /* 检查文件修改时间 */
        struct stat st;
        if (stat(doc->path, &st) == 0) {
            if (st.st_mtime > doc->update_time) {
                /* 文档已更新，重新索引 */
                ki_reindex_document(ki, doc->doc_id);
            }
        }
    }
    return 0;
}

/* 重建索引 */
int ki_rebuild_index(knowledge_index_t *ki) {
    /* 清空现有索引 */
    for (uint32_t i = 0; i < ki->index_size; i++) {
        free(ki->inverted_index[i]->term);
        free(ki->inverted_index[i]->doc_ids);
        free(ki->inverted_index[i]->tf_weights);
        free(ki->inverted_index[i]);
    }
    ki->index_size = 0;

    /* 重新构建 */
    for (uint32_t i = 0; i < ki->doc_count; i++) {
        char *content = load_document_content(ki->documents[i]->path);
        if (content) {
            add_to_inverted_index(ki, ki->documents[i], content);
            free(content);
        }
    }

    /* 计算权重 */
    calculate_tfidf_weights(ki);

    return 0;
}
