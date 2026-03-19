/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 1284
 * Language: c
 * Block ID: 5643ee0c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// file: database_example.c
// compile: clang -std=c2y database_example.c -o database_example -lsqlite3

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

typedef struct {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int in_transaction;
} DbContext;

int db_execute_query(DbContext *ctx, const char *sql,
                     int (*callback)(void*, int, char**, char**),
                     void *user_data) {
    // 注册上下文清理
    defer {
        if (ctx->stmt) {
            sqlite3_finalize(ctx->stmt);
            ctx->stmt = NULL;
        }
        if (ctx->in_transaction) {
            sqlite3_exec(ctx->db, "ROLLBACK", NULL, NULL, NULL);
            ctx->in_transaction = 0;
        }
    }

    // 准备语句
    int rc = sqlite3_prepare_v2(ctx->db, sql, -1, &ctx->stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare: %s\n", sqlite3_errmsg(ctx->db));
        return -1;
    }

    // 执行
    int step_result;
    while ((step_result = sqlite3_step(ctx->stmt)) == SQLITE_ROW) {
        if (callback) {
            int col_count = sqlite3_column_count(ctx->stmt);
            char *values[col_count];
            char *columns[col_count];

            for (int i = 0; i < col_count; i++) {
                columns[i] = (char*)sqlite3_column_name(ctx->stmt, i);
                values[i] = (char*)sqlite3_column_text(ctx->stmt, i);
            }

            if (callback(user_data, col_count, values, columns) != 0) {
                return -1;
            }
        }
    }

    if (step_result != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(ctx->db));
        return -1;
    }

    return 0;  // 自动清理stmt
}

int db_transaction_begin(DbContext *ctx) {
    if (ctx->in_transaction) {
        fprintf(stderr, "Already in transaction\n");
        return -1;
    }

    int rc = sqlite3_exec(ctx->db, "BEGIN TRANSACTION", NULL, NULL, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to begin transaction: %s\n",
                sqlite3_errmsg(ctx->db));
        return -1;
    }

    ctx->in_transaction = 1;
    return 0;
}

int db_transaction_commit(DbContext *ctx) {
    if (!ctx->in_transaction) {
        fprintf(stderr, "Not in transaction\n");
        return -1;
    }

    int rc = sqlite3_exec(ctx->db, "COMMIT", NULL, NULL, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to commit: %s\n", sqlite3_errmsg(ctx->db));
        return -1;
    }

    ctx->in_transaction = 0;
    return 0;
}

// 回调函数示例
static int print_row(void *unused, int argc, char **argv, char **colnames) {
    (void)unused;
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", colnames[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int main(void) {
    DbContext ctx = {0};

    // 打开数据库
    int rc = sqlite3_open(":memory:", &ctx.db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(ctx.db));
        return 1;
    }
    defer sqlite3_close(ctx.db);

    // 创建表
    db_execute_query(&ctx,
        "CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)",
        NULL, NULL);

    // 插入数据
    db_execute_query(&ctx,
        "INSERT INTO users (name) VALUES ('Alice'), ('Bob'), ('Charlie')",
        NULL, NULL);

    // 查询数据
    printf("=== Users ===\n");
    db_execute_query(&ctx, "SELECT * FROM users", print_row, NULL);

    return 0;
}
