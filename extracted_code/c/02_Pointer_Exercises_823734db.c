/*
 * Auto-generated from: 12_Practice_Exercises\02_Pointer_Exercises.md
 * Line: 131
 * Language: c
 * Block ID: 823734db
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 实现支持任意类型的快速排序
void generic_qsort(void *base, size_t nmemb, size_t size,
                   int (*compare)(const void *, const void *));

// 使用示例
int compare_int(const void *a, const void *b);
int compare_string(const void *a, const void *b);
