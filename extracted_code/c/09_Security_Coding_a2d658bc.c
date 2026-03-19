/*
 * Auto-generated from: 12_Practice_Exercises\09_Security_Coding.md
 * Line: 321
 * Language: c
 * Block ID: a2d658bc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct QueryBuilder QueryBuilder;

// 创建参数化查询
QueryBuilder* query_create(const char *table);

// 安全添加条件（自动转义）
void query_where_int(QueryBuilder *qb, const char *column,
                     const char *op, int64_t value);
void query_where_string(QueryBuilder *qb, const char *column,
                        const char *op, const char *value);
void query_where_in_ints(QueryBuilder *qb, const char *column,
                         const int64_t *values, size_t count);

// 构建查询（生成参数化SQL）
char* query_build(const QueryBuilder *qb);
void** query_get_params(const QueryBuilder *qb, size_t *count);

// 清理
void query_destroy(QueryBuilder *qb);
