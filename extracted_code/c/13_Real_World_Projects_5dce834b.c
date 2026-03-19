/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 3285
 * Language: c
 * Block ID: 5dce834b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef DB_H
#define DB_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define DB_VERSION "1.0.0"
#define PAGE_SIZE 4096
#define CACHE_SIZE 1024  /* pages */
#define MAX_NAME_LEN 64
#define MAX_COLUMNS 32
#define MAX_TABLES 256

typedef enum {
    TYPE_INTEGER = 1,
    TYPE_FLOAT = 2,
    TYPE_TEXT = 3,
    TYPE_BLOB = 4,
    TYPE_NULL = 5
} db_type_t;

typedef enum {
    DB_OK = 0,
    DB_ERROR = -1,
    DB_FULL = -2,
    DB_CORRUPT = -3,
    DB_CONSTRAINT = -4,
    DB_NOTFOUND = -5,
    DB_BUSY = -6,
    DB_READONLY = -7
} db_result_t;

typedef enum {
    OP_EQ, OP_NE, OP_LT, OP_LE, OP_GT, OP_GE,
    OP_AND, OP_OR, OP_NOT
} db_op_t;

/* Forward declarations */
typedef struct db db_t;
typedef struct cursor cursor_t;
typedef struct stmt stmt_t;
typedef struct value value_t;

/* Value structure */
struct value {
    db_type_t type;
    union {
        int64_t i;
        double f;
        struct {
            char *data;
            size_t len;
        } text;
        struct {
            void *data;
            size_t len;
        } blob;
    } data;
};

/* Column definition */
typedef struct column_def {
    char name[MAX_NAME_LEN];
    db_type_t type;
    bool not_null;
    bool primary_key;
    bool auto_increment;
    value_t default_val;
} column_def_t;

/* Table schema */
typedef struct table_def {
    char name[MAX_NAME_LEN];
    column_def_t columns[MAX_COLUMNS];
    int column_count;
    int primary_key_idx;
} table_def_t;

/* Database API */
db_t* db_open(const char *path);
void db_close(db_t *db);
int db_execute(db_t *db, const char *sql);
stmt_t* db_prepare(db_t *db, const char *sql);
int db_step(stmt_t *stmt);
void db_finalize(stmt_t *stmt);

/* Value operations */
value_t db_make_int(int64_t i);
value_t db_make_float(double f);
value_t db_make_text(const char *str, size_t len);
value_t db_make_blob(const void *data, size_t len);
value_t db_make_null(void);
void db_free_value(value_t *val);
int db_compare_values(const value_t *a, const value_t *b);

/* Cursor operations for B+ tree */
cursor_t* db_cursor_open(db_t *db, const char *table);
void db_cursor_close(cursor_t *cursor);
int db_cursor_first(cursor_t *cursor);
int db_cursor_next(cursor_t *cursor);
int db_cursor_prev(cursor_t *cursor);
int db_cursor_seek(cursor_t *cursor, const value_t *key);
value_t db_cursor_key(const cursor_t *cursor);
value_t* db_cursor_value(const cursor_t *cursor);
int db_cursor_insert(cursor_t *cursor, const value_t *key,
                     const value_t *values, int count);
int db_cursor_delete(cursor_t *cursor);

/* Transaction API */
int db_begin_transaction(db_t *db);
int db_commit(db_t *db);
int db_rollback(db_t *db);

/* Metadata */
int db_create_table(db_t *db, const table_def_t *def);
int db_drop_table(db_t *db, const char *name);
table_def_t* db_get_table(db_t *db, const char *name);

/* Error handling */
const char* db_errmsg(db_t *db);
int db_errcode(db_t *db);

#endif /* DB_H */
