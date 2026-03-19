/*
 * Auto-generated from: 12_Practice_Exercises\12_Mini_Projects.md
 * Line: 58
 * Language: c
 * Block ID: b8d7efc1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct Database Database;

typedef enum {
    DB_OK = 0,
    DB_NOT_FOUND,
    DB_FULL,
    DB_IO_ERROR
} DBError;

// 生命周期
Database* db_open(const char *path);
void db_close(Database *db);

// 基本操作
DBError db_put(Database *db, const char *key,
               const void *value, size_t value_len);
DBError db_get(Database *db, const char *key,
               void *out_buf, size_t buf_size, size_t *out_len);
DBError db_delete(Database *db, const char *key);

// 迭代
void* db_iterator_create(Database *db);
bool db_iterator_next(void *iter, char **key, void **value, size_t *len);
void db_iterator_destroy(void *iter);

// 事务
DBError db_begin(Database *db);
DBError db_commit(Database *db);
DBError db_rollback(Database *db);
