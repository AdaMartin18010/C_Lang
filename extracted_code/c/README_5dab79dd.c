/*
 * Auto-generated from: 06_Thinking_Representation\08_Index\README.md
 * Line: 714
 * Language: c
 * Block ID: 5dab79dd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#include "knowledge_index.h"
#include <stdio.h>

int main() {
    /* 创建索引 */
    knowledge_index_t *ki = ki_create("C语言知识库");

    /* 添加文档 */
    doc_metadata_t doc1 = {
        .doc_id = 1,
        .title = "C指针完全指南",
        .path = "docs/pointer_guide.md",
        .summary = "全面介绍C语言指针的概念、用法和最佳实践",
        .type = DOC_TYPE_TUTORIAL,
        .difficulty = DIFFICULTY_INTERMEDIATE,
        .category_path = "语言基础/指针"
    };
    ki_add_document(ki, &doc1);

    /* 添加标签 */
    ki_add_tag_to_document(ki, 1, "指针");
    ki_add_tag_to_document(ki, 1, "内存管理");
    ki_add_tag_to_document(ki, 1, "进阶");

    /* 构建索引 */
    ki_build_inverted_index(ki);
    ki_build_tag_index(ki);

    /* 搜索 */
    uint32_t result_count;
    search_result_t *results = ki_search(ki, "指针 内存", &result_count);

    printf("找到 %d 个结果:\n", result_count);
    for (uint32_t i = 0; i < result_count; i++) {
        doc_metadata_t *doc = ki_get_document(ki, results[i].doc_id);
        printf("%d. %s (相关度: %.2f)\n",
               i + 1, doc->title, results[i].relevance_score);
    }

    /* 清理 */
    ki_destroy(ki);
    return 0;
}
