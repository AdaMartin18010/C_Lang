/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\02_Database_Engine.md
 * Line: 209
 * Language: c
 * Block ID: 900297f2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* embedded_db.h - 简化嵌入式数据库 */
#ifndef EMBEDDED_DB_H
#define EMBEDDED_DB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#define DB_VERSION          1
#define PAGE_SIZE           4096
#define MAX_KEY_SIZE        256
#define MAX_VALUE_SIZE      1024
#define ORDER               4       /* B+树的阶 */
#define MAX_PAGES           1024
#define MAX_TRANSACTIONS    16
#define WAL_MAGIC           0x57414C21  /* "WAL!" */

/* 错误码 */
typedef enum {
    DB_OK = 0,
    DB_ERROR_IO,
    DB_ERROR_NOMEM,
    DB_ERROR_NOT_FOUND,
    DB_ERROR_EXISTS,
    DB_ERROR_FULL,
    DB_ERROR_CORRUPT,
    DB_ERROR_INVALID
} db_error_t;

/* 页类型 */
typedef enum {
    PAGE_TYPE_FREE = 0,
    PAGE_TYPE_META,
    PAGE_TYPE_BTREE_INTERNAL,
    PAGE_TYPE_BTREE_LEAF,
    PAGE_TYPE_OVERFLOW
} page_type_t;

/* 页头 (128字节) */
typedef struct __attribute__((packed)) {
    uint32_t page_id;           /* 页ID */
    uint32_t checksum;          /* CRC32校验 */
    uint16_t type;              /* 页类型 */
    uint16_t num_keys;          /* 键数量 */
    uint16_t free_offset;       /* 空闲空间偏移 */
    uint16_t flags;             /* 标志位 */
    uint32_t right_sibling;     /* 右兄弟页ID */
    uint32_t parent;            /* 父页ID */
    uint64_t lsn;               /* 日志序列号 */
    uint8_t reserved[96];       /* 保留 */
} page_header_t;

/* B+树节点项 */
typedef struct {
    uint16_t key_len;
    uint16_t value_len;
    uint32_t page_id;           /* 子页ID(内部节点)或溢出页 */
    uint64_t tx_id;             /* 创建此版本的事务ID */
    uint64_t delete_tx;         /* 删除此版本的事务ID(0=未删除) */
    char data[];                /* key + value 数据 */
} btree_item_t;

/* 页结构 */
typedef struct page {
    page_header_t header;
    uint8_t data[PAGE_SIZE - sizeof(page_header_t)];
} page_t;

/* 页缓存 */
typedef struct {
    page_t *page;
    uint32_t page_id;
    bool is_dirty;
    uint64_t last_access;
    struct page_entry *next;
    struct page_entry *prev;
} page_entry_t;

/* 事务状态 */
typedef enum {
    TX_STATE_INACTIVE = 0,
    TX_STATE_ACTIVE,
    TX_STATE_COMMITTED,
    TX_STATE_ABORTED
} tx_state_t;

/* 事务 */
typedef struct {
    uint64_t tx_id;
    tx_state_t state;
    uint64_t start_time;
    uint64_t lsn;
    uint32_t modified_pages[16];  /* 本事务修改的页 */
    uint16_t num_modified;
} transaction_t;

/* WAL日志记录类型 */
typedef enum {
    WAL_TYPE_BEGIN = 1,
    WAL_TYPE_INSERT,
    WAL_TYPE_UPDATE,
    WAL_TYPE_DELETE,
    WAL_TYPE_COMMIT,
    WAL_TYPE_CHECKPOINT
} wal_type_t;

/* WAL日志记录头 */
typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint32_t checksum;
    uint64_t lsn;
    uint64_t tx_id;
    uint16_t type;
    uint16_t key_len;
    uint16_t value_len;
    uint32_t page_id;
} wal_header_t;

/* WAL日志记录 */
typedef struct wal_record {
    wal_header_t header;
    char *key;
    char *value;
    struct wal_record *next;
} wal_record_t;

/* WAL文件 */
typedef struct {
    FILE *fp;
    char filename[256];
    uint64_t current_lsn;
    wal_record_t *records;
} wal_t;

/* B+树索引 */
typedef struct {
    uint32_t root_page;         /* 根页ID */
    uint32_t num_pages;         /* 总页数 */
    uint32_t num_keys;          /* 总键数 */
    uint32_t height;            /* 树高 */
} btree_t;

/* 数据库实例 */
typedef struct {
    char db_name[64];
    FILE *db_fp;                /* 数据文件 */
    wal_t *wal;                 /* WAL日志 */
    btree_t btree;              /* B+树索引 */

    /* 页缓存 */
    page_entry_t *page_cache;
    uint32_t cache_size;
    uint32_t cache_capacity;

    /* 事务管理 */
    transaction_t transactions[MAX_TRANSACTIONS];
    uint64_t next_tx_id;
    uint64_t last_checkpoint;

    /* 自由页列表 */
    uint32_t *free_pages;
    uint32_t num_free_pages;
    uint32_t max_free_pages;

    bool is_open;
} database_t;

/* 迭代器 */
typedef struct {
    database_t *db;
    uint32_t current_page;
    uint16_t current_slot;
    uint64_t tx_id;             /* 事务可见性 */
} db_iterator_t;

/* API函数 */
/* 数据库生命周期 */
db_error_t db_open(database_t *db, const char *name);
db_error_t db_close(database_t *db);
db_error_t db_checkpoint(database_t *db);

/* 键值操作 */
db_error_t db_put(database_t *db, const char *key,
                  const char *value, uint64_t tx_id);
db_error_t db_get(database_t *db, const char *key,
                  char *value, size_t value_size, uint64_t tx_id);
db_error_t db_delete(database_t *db, const char *key, uint64_t tx_id);

/* 事务 */
db_error_t db_begin_transaction(database_t *db, uint64_t *tx_id);
db_error_t db_commit(database_t *db, uint64_t tx_id);
db_error_t db_rollback(database_t *db, uint64_t tx_id);

/* 迭代器 */
db_error_t db_iterator_init(database_t *db, db_iterator_t *iter, uint64_t tx_id);
db_error_t db_iterator_next(database_t *db, db_iterator_t *iter,
                             char *key, size_t key_size,
                             char *value, size_t value_size);

/* 工具函数 */
void db_stats(database_t *db);
db_error_t db_recover(database_t *db);

#endif /* EMBEDDED_DB_H */
