/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\03_Top_Down_Analysis_Method.md
 * Line: 1480
 * Language: c
 * Block ID: 455d1b81
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* database_query.c - 数据库查询处理 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[32];
    int category;
    double value;
} Record;

/* 原始版本 - 行存储处理 */
double query_sum_original(Record *records, int n, int target_category) {
    double sum = 0;
    for (int i = 0; i < n; i++) {
        if (records[i].category == target_category) {
            sum += records[i].value;
        }
    }
    return sum;
}

/* 分析结果: Frontend_Bound高(18%)，Cache_Misses严重 */
/* 原因: 每次迭代加载整个Record结构体，但只有部分字段使用 */

/* 优化版本 - 列存储处理 */
typedef struct {
    int *categories;
    double *values;
    int n;
} ColumnStore;

double query_sum_columnar(ColumnStore *store, int target_category) {
    double sum = 0;
    /* 顺序访问categories数组，缓存友好 */
    for (int i = 0; i < store->n; i++) {
        if (store->categories[i] == target_category) {
            sum += store->values[i];
        }
    }
    return sum;
}

/* 进一步优化 - 分支预测优化 */
double query_sum_optimized(ColumnStore *store, int target_category) {
    double sum = 0;
    /* 先收集匹配索引，然后处理 */
    int *matches = malloc(store->n * sizeof(int));
    int match_count = 0;

    for (int i = 0; i < store->n; i++) {
        matches[match_count] = i;
        match_count += (store->categories[i] == target_category);
    }

    for (int j = 0; j < match_count; j++) {
        sum += store->values[matches[j]];
    }

    free(matches);
    return sum;
}
