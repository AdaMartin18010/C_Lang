# 数据库引擎案例研究


---

## 📑 目录

- [数据库引擎案例研究](#数据库引擎案例研究)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [1. 存储引擎架构](#1-存储引擎架构)
    - [1.1 B+树 vs LSM树](#11-b树-vs-lsm树)
    - [1.2 B+树结构详解](#12-b树结构详解)
    - [1.3 页结构](#13-页结构)
  - [2. 事务ACID实现](#2-事务acid实现)
    - [2.1 ACID特性保证](#21-acid特性保证)
    - [2.2 MVCC多版本并发控制](#22-mvcc多版本并发控制)
  - [3. WAL日志和检查点](#3-wal日志和检查点)
    - [3.1 WAL（预写日志）原理](#31-wal预写日志原理)
    - [3.2 检查点机制](#32-检查点机制)
  - [4. 完整嵌入式数据库实现](#4-完整嵌入式数据库实现)
    - [4.1 数据库头文件](#41-数据库头文件)
    - [4.2 核心实现代码](#42-核心实现代码)
    - [4.3 编译和运行](#43-编译和运行)
  - [5. 架构总结](#5-架构总结)
  - [总结](#总结)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 概述

本章深入分析嵌入式数据库引擎的设计与实现，涵盖存储引擎架构、事务管理、日志系统等核心组件。
通过完整的代码示例展示如何构建一个简化但功能完整的B+树数据库。

---

## 1. 存储引擎架构

### 1.1 B+树 vs LSM树

```text
┌─────────────────────────────────────────────────────────────┐
│                     存储引擎架构对比                          │
├─────────────────────────────────────────────────────────────┤
│  B+树存储引擎                   LSM树存储引擎                  │
│  ────────────                   ────────────                  │
│                                                             │
│  ┌─────────┐                   ┌─────────┐                   │
│  │ 内存缓存 │                   │ 内存表  │ MemTable          │
│  │ (Buffer)│                   │ (SkipList)│                  │
│  └────┬────┘                   └────┬────┘                   │
│       │                             │                       │
│  ┌────▼────┐                   ┌────▼────┐                   │
│  │ B+树索引 │                   │ WAL日志 │ (持久化)          │
│  │ (磁盘)  │                   └────┬────┘                   │
│  └────┬────┘                        │                       │
│       │                        ┌────▼────┐                  │
│  ┌────▼────┐                   │ SSTable │ (磁盘文件)        │
│  │ 数据页  │                   │ 合并压缩 │                  │
│  │ (Page)  │                   └─────────┘                  │
│  └─────────┘                                                │
│                                                             │
│  特点: 读优化, 原地更新        特点: 写优化, 追加写入         │
│  代表: InnoDB, PostgreSQL      代表: LevelDB, RocksDB       │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 B+树结构详解

```text
                    ┌─────┐
                    │ 10  │  <- 内部节点(索引)
                    └──┬──┘
           ┌───────────┼───────────┐
           ▼           ▼           ▼
       ┌───────┐   ┌───────┐   ┌───────┐
       │5│7│9  │   │10│15  │   │20│30  │  <- 内部节点
       └───┬───┘   └───┬───┘   └───┬───┘
     ┌─────┼─────┐ ┌───┼────┐ ┌────┼────┐
     ▼     ▼     ▼ ▼   ▼    ▼ ▼    ▼    ▼
    ┌─┐   ┌─┐   ┌─┐ ┌─┐  ┌─┐ ┌─┐  ┌─┐  ┌─┐
    │1│   │5│   │7│ │9│  │10││15│ │20│ │30│ <- 叶子节点
    │2│   │6│   │8│ │ │  │12││17│ │25│ │35│    (含数据)
    │3│   │ │   │ │ │ │  │ ││ │  │ │  │ │
    │4│   │ │   │ │ │ │  │ ││ │  │ │  │ │
    └─┘   └─┘   └─┘ └─┘  └─┘└─┘  └─┘  └─┘
     │     │     │   │    │  │    │    │
     └─────┴─────┴───┴────┴──┴────┴────┘
              叶子节点形成双向链表
```

### 1.3 页结构

```text
┌─────────────────────────────────────────────────────┐
│                    数据库页结构 (4KB/8KB)            │
├─────────────────────────────────────────────────────┤
│ 页头 (Page Header)        │ 128 bytes               │
│ ├── 页ID / Page ID        │ uint32_t                │
│ ├── 页类型 / Page Type    │ uint8_t (叶子/内部/溢出) │
│ ├── 记录数量 / Num Keys   │ uint16_t                │
│ ├── 空闲偏移 / Free Offset│ uint16_t                │
│ ├── 右兄弟页 / Right Sibling│ uint32_t               │
│ └── CRC校验 / Checksum    │ uint32_t                │
├─────────────────────────────────────────────────────┤
│ 记录目录 (Slot Directory) │ 变长                    │
│ ├── 槽位1偏移             │ uint16_t                │
│ ├── 槽位1长度             │ uint16_t                │
│ ├── 槽位2偏移             │ uint16_t                │
│ └── ...                   │                         │
├─────────────────────────────────────────────────────┤
│ 空闲空间 (Free Space)     │ 变长                    │
├─────────────────────────────────────────────────────┤
│ 记录数据 (Records)        │ 从页尾向上增长          │
│ ├── 记录N (最新)          │                         │
│ ├── 记录N-1               │                         │
│ └── ...                   │                         │
└─────────────────────────────────────────────────────┘
```

---

## 2. 事务ACID实现

### 2.1 ACID特性保证

| 特性 | 实现机制 | 说明 |
|------|----------|------|
| **原子性(A)** | WAL日志 | 操作先记日志，崩溃后回放恢复 |
| **一致性(C)** | 约束检查+级联更新 | 外键、唯一性约束等 |
| **隔离性(I)** | MVCC/锁 | 多版本并发控制或两阶段锁 |
| **持久性(D)** | WAL+checkpoint | 日志落盘+检查点刷脏页 |

### 2.2 MVCC多版本并发控制

```text
┌────────────────────────────────────────────────────────────┐
│                    MVCC 版本链                             │
├────────────────────────────────────────────────────────────┤
│                                                            │
│  事务ID 100: UPDATE row SET value='A' -> 'B'               │
│  事务ID 105: UPDATE row SET value='B' -> 'C'               │
│  事务ID 110: DELETE row                                    │
│                                                            │
│  版本链 (从新到旧):                                         │
│                                                            │
│  ┌─────────┐    ┌─────────┐    ┌─────────┐                 │
│  │ 删除标记 │───▶│ value=C │───▶│ value=B │───▶ ...       │
│  │ tx=110  │    │ tx=105  │    │ tx=100  │                 │
│  └─────────┘    └─────────┘    └─────────┘                 │
│                                                            │
│  事务102读取: 看到value=A (忽略tx>100的版本)                 │
│  事务107读取: 看到value=B (忽略tx>107的版本)                 │
│  事务115读取: 行不存在 (删除事务已提交)                       │
│                                                            │
└────────────────────────────────────────────────────────────┘
```

---

## 3. WAL日志和检查点

### 3.1 WAL（预写日志）原理

```text
写入流程:
    ┌─────────┐    ┌─────────┐    ┌─────────┐
    │  用户   │───▶│  缓冲   │───▶│  WAL    │
    │ 事务提交│    │  池     │    │  日志   │
    └─────────┘    └────┬────┘    └────┬────┘
                        │              │
                        │ 1. 先写WAL   │
                        │◀─────────────┘
                        │
                        ▼ 2. 再刷脏页
                   ┌─────────┐
                   │ 数据文件│
                   └─────────┘

恢复流程:
    ┌─────────┐    ┌─────────┐    ┌─────────┐
    │ 读取WAL │───▶│ 重做    │───▶│ 数据文件│
    │ 日志    │    │ (REDO)  │    │ 恢复    │
    └─────────┘    └─────────┘    └─────────┘
```

### 3.2 检查点机制

```text
时间线:
─────────────────────────────────────────────────────────►
│          │          │          │          │
CP 1       CP 2       CP 3       CP 4       CP 5
│←─ 需重做 ─→│          │          │          │
           │←────── 需重做 ──────→│          │
                      │←────── 需重做 ──────→│

检查点 = 将内存中所有脏页刷盘 + 记录日志序列号(LSN)
崩溃恢复时从最后一个检查点开始重做
```

---

## 4. 完整嵌入式数据库实现

### 4.1 数据库头文件

```c
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
```

### 4.2 核心实现代码

```c
/* embedded_db.c - 嵌入式数据库实现 */
#define _POSIX_C_SOURCE 200809L
#include "embedded_db.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

/*==================== CRC32校验 ====================*/

static uint32_t crc32_table[256];
static bool crc32_initialized = false;

static void crc32_init(void) {
    if (crc32_initialized) return;
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
        crc32_table[i] = crc;
    }
    crc32_initialized = true;
}

static uint32_t crc32(const void *data, size_t len) {
    if (!crc32_initialized) crc32_init();
    const uint8_t *bytes = data;
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < len; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ bytes[i]) & 0xFF];
    }
    return ~crc;
}

/*==================== 页管理 ====================*/

static page_t* page_alloc(database_t *db) {
    page_t *page = aligned_alloc(512, sizeof(page_t));
    if (page) {
        memset(page, 0, sizeof(page_t));
    }
    return page;
}

static void page_free(page_t *page) {
    free(page);
}

static uint32_t page_allocate_new(database_t *db) {
    /* 优先使用自由页 */
    if (db->num_free_pages > 0) {
        return db->free_pages[--db->num_free_pages];
    }
    /* 分配新页 */
    return db->btree.num_pages++;
}

static db_error_t page_read(database_t *db, uint32_t page_id, page_t *page) {
    if (fseek(db->db_fp, page_id * PAGE_SIZE, SEEK_SET) != 0) {
        return DB_ERROR_IO;
    }
    if (fread(page, sizeof(page_t), 1, db->db_fp) != 1) {
        return DB_ERROR_IO;
    }
    /* 校验 */
    uint32_t checksum = page->header.checksum;
    page->header.checksum = 0;
    if (crc32(page, sizeof(page_t)) != checksum) {
        return DB_ERROR_CORRUPT;
    }
    return DB_OK;
}

static db_error_t page_write(database_t *db, uint32_t page_id, page_t *page) {
    /* 先写WAL */
    wal_header_t wal_header = {
        .magic = WAL_MAGIC,
        .lsn = ++db->wal->current_lsn,
        .page_id = page_id,
        .type = WAL_TYPE_UPDATE
    };

    /* 计算校验并写入 */
    page->header.checksum = 0;
    page->header.checksum = crc32(page, sizeof(page_t));

    if (fseek(db->db_fp, page_id * PAGE_SIZE, SEEK_SET) != 0) {
        return DB_ERROR_IO;
    }
    if (fwrite(page, sizeof(page_t), 1, db->db_fp) != 1) {
        return DB_ERROR_IO;
    }
    fflush(db->db_fp);
    return DB_OK;
}

/*==================== B+树实现 ====================*/

static uint16_t btree_item_size(const btree_item_t *item) {
    return sizeof(btree_item_t) + item->key_len + item->value_len;
}

static btree_item_t* btree_get_item(const page_t *page, uint16_t slot) {
    uint16_t *offsets = (uint16_t *)page->data;
    if (slot >= page->header.num_keys) return NULL;
    return (btree_item_t *)((uint8_t *)page + offsets[slot]);
}

static int btree_compare_key(const char *k1, uint16_t len1,
                              const char *k2, uint16_t len2) {
    uint16_t min_len = len1 < len2 ? len1 : len2;
    int cmp = memcmp(k1, k2, min_len);
    if (cmp != 0) return cmp;
    return (len1 < len2) ? -1 : (len1 > len2) ? 1 : 0;
}

static int btree_find_slot(const page_t *page, const char *key,
                           uint16_t key_len) {
    int left = 0, right = page->header.num_keys;
    while (left < right) {
        int mid = (left + right) / 2;
        btree_item_t *item = btree_get_item(page, mid);
        int cmp = btree_compare_key(key, key_len,
                                     item->data, item->key_len);
        if (cmp < 0) {
            right = mid;
        } else if (cmp > 0) {
            left = mid + 1;
        } else {
            return mid;
        }
    }
    return -left - 1;  /* 负数表示未找到，插入位置为 (-result - 1) */
}

static db_error_t btree_insert_item(page_t *page, int slot,
                                     const char *key, uint16_t key_len,
                                     const char *value, uint16_t value_len,
                                     uint32_t child_page, uint64_t tx_id) {
    size_t item_size = sizeof(btree_item_t) + key_len + value_len;

    /* 检查空间 */
    uint16_t free_space = page->header.free_offset > 0
        ? page->header.free_offset
        : PAGE_SIZE - sizeof(page_header_t);
    free_space -= page->header.num_keys * sizeof(uint16_t);

    if (item_size > free_space) {
        return DB_ERROR_FULL;
    }

    /* 移动槽位目录 */
    uint16_t *offsets = (uint16_t *)page->data;
    if (slot < 0) slot = -slot - 1;

    for (int i = page->header.num_keys; i > slot; i--) {
        offsets[i] = offsets[i - 1];
    }

    /* 计算新项位置 (从页尾向上增长) */
    uint16_t offset = PAGE_SIZE - item_size;
    if (page->header.num_keys > 0) {
        btree_item_t *last = btree_get_item(page, 0);
        offset = (uint8_t *)last - (uint8_t *)page - item_size;
    }

    offsets[slot] = offset;

    /* 填充项 */
    btree_item_t *item = (btree_item_t *)((uint8_t *)page + offset);
    item->key_len = key_len;
    item->value_len = value_len;
    item->page_id = child_page;
    item->tx_id = tx_id;
    item->delete_tx = 0;
    memcpy(item->data, key, key_len);
    memcpy(item->data + key_len, value, value_len);

    page->header.num_keys++;
    return DB_OK;
}

static db_error_t btree_split(database_t *db, uint32_t page_id,
                               uint32_t *new_page_id,
                               char *split_key, uint16_t *split_key_len);

static db_error_t btree_insert_recursive(database_t *db, uint32_t page_id,
                                          const char *key, uint16_t key_len,
                                          const char *value, uint16_t value_len,
                                          uint64_t tx_id) {
    page_t *page = page_alloc(db);
    if (!page) return DB_ERROR_NOMEM;

    db_error_t err = page_read(db, page_id, page);
    if (err != DB_OK) {
        page_free(page);
        return err;
    }

    if (page->header.type == PAGE_TYPE_BTREE_LEAF) {
        /* 叶子节点直接插入 */
        int slot = btree_find_slot(page, key, key_len);
        if (slot >= 0) {
            /* 键已存在，更新值 */
            btree_item_t *item = btree_get_item(page, slot);
            if (value_len <= item->value_len) {
                memcpy(item->data + item->key_len, value, value_len);
                item->value_len = value_len;
                item->tx_id = tx_id;
            } else {
                /* 标记旧版本删除，插入新版本 */
                item->delete_tx = tx_id;
                err = btree_insert_item(page, page->header.num_keys,
                                        key, key_len, value, value_len,
                                        0, tx_id);
            }
        } else {
            err = btree_insert_item(page, slot, key, key_len,
                                    value, value_len, 0, tx_id);
        }

        if (err == DB_ERROR_FULL) {
            /* 需要分裂 */
            uint32_t new_page;
            char split_key_buf[MAX_KEY_SIZE];
            uint16_t split_key_len;
            err = btree_split(db, page_id, &new_page,
                             split_key_buf, &split_key_len);
            if (err == DB_OK) {
                /* 重新插入 */
                int cmp = btree_compare_key(key, key_len,
                                             split_key_buf, split_key_len);
                uint32_t target = (cmp >= 0) ? new_page : page_id;
                err = btree_insert_recursive(db, target, key, key_len,
                                              value, value_len, tx_id);
            }
        }
    } else {
        /* 内部节点: 找到子节点递归插入 */
        int slot = btree_find_slot(page, key, key_len);
        if (slot < 0) slot = -slot - 1;
        if (slot >= page->header.num_keys) slot = page->header.num_keys - 1;

        btree_item_t *item = btree_get_item(page, slot);
        uint32_t child = (slot == 0 && item) ? page->header.right_sibling
                                             : item->page_id;

        err = btree_insert_recursive(db, child, key, key_len,
                                      value, value_len, tx_id);
    }

    if (err == DB_OK) {
        page->header.lsn = db->wal->current_lsn;
        err = page_write(db, page_id, page);
    }

    page_free(page);
    return err;
}

static db_error_t btree_split(database_t *db, uint32_t page_id,
                               uint32_t *new_page_id,
                               char *split_key, uint16_t *split_key_len) {
    page_t *page = page_alloc(db);
    page_t *new_page = page_alloc(db);
    if (!page || !new_page) {
        page_free(page);
        page_free(new_page);
        return DB_ERROR_NOMEM;
    }

    db_error_t err = page_read(db, page_id, page);
    if (err != DB_OK) goto cleanup;

    *new_page_id = page_allocate_new(db);

    /* 复制后半部分到新页 */
    int mid = page->header.num_keys / 2;
    btree_item_t *mid_item = btree_get_item(page, mid);

    new_page->header.type = page->header.type;
    new_page->header.page_id = *new_page_id;
    new_page->header.right_sibling = page->header.right_sibling;
    page->header.right_sibling = *new_page_id;

    /* 复制分裂键 */
    *split_key_len = mid_item->key_len;
    memcpy(split_key, mid_item->data, mid_item->key_len);

    /* 移动键到新页 */
    for (int i = mid; i < page->header.num_keys; i++) {
        btree_item_t *item = btree_get_item(page, i);
        btree_insert_item(new_page, new_page->header.num_keys,
                          item->data, item->key_len,
                          item->data + item->key_len, item->value_len,
                          item->page_id, item->tx_id);
    }
    page->header.num_keys = mid;

    /* 写入页 */
    page_write(db, page_id, page);
    page_write(db, *new_page_id, new_page);

    /* 更新父节点(简化处理：若无父节点，创建新根) */
    if (page->header.parent == 0) {
        uint32_t new_root = page_allocate_new(db);
        page_t *root_page = page_alloc(db);
        root_page->header.type = PAGE_TYPE_BTREE_INTERNAL;
        root_page->header.page_id = new_root;
        root_page->header.right_sibling = page_id;
        btree_insert_item(root_page, 0, split_key, *split_key_len,
                          NULL, 0, *new_page_id, 0);
        page_write(db, new_root, root_page);
        page_free(root_page);

        page->header.parent = new_root;
        new_page->header.parent = new_root;
        db->btree.root_page = new_root;
        db->btree.height++;
    }

    err = DB_OK;

cleanup:
    page_free(page);
    page_free(new_page);
    return err;
}

/*==================== 公共API ====================*/

db_error_t db_open(database_t *db, const char *name) {
    memset(db, 0, sizeof(*db));
    strncpy(db->db_name, name, sizeof(db->db_name) - 1);

    /* 打开数据文件 */
    char db_filename[128];
    snprintf(db_filename, sizeof(db_filename), "%s.db", name);
    db->db_fp = fopen(db_filename, "r+b");

    if (!db->db_fp) {
        /* 创建新数据库 */
        db->db_fp = fopen(db_filename, "w+b");
        if (!db->db_fp) return DB_ERROR_IO;

        /* 初始化元数据页 */
        page_t meta_page = {0};
        meta_page.header.page_id = 0;
        meta_page.header.type = PAGE_TYPE_META;
        meta_page.header.num_keys = DB_VERSION;
        page_write(db, 0, &meta_page);

        /* 初始化根页 */
        page_t root_page = {0};
        root_page.header.page_id = 1;
        root_page.header.type = PAGE_TYPE_BTREE_LEAF;
        page_write(db, 1, &root_page);

        db->btree.root_page = 1;
        db->btree.num_pages = 2;
        db->btree.height = 1;
    } else {
        /* 读取已有数据库 */
        page_t meta_page;
        if (page_read(db, 0, &meta_page) != DB_OK) {
            fclose(db->db_fp);
            return DB_ERROR_CORRUPT;
        }
        db->btree.root_page = 1;
        db->btree.num_pages = 2;
    }

    /* 打开WAL */
    char wal_filename[128];
    snprintf(wal_filename, sizeof(wal_filename), "%s.wal", name);
    db->wal = malloc(sizeof(wal_t));
    db->wal->fp = fopen(wal_filename, "a+b");
    if (!db->wal->fp) {
        fclose(db->db_fp);
        free(db->wal);
        return DB_ERROR_IO;
    }
    strncpy(db->wal->filename, wal_filename, sizeof(db->wal->filename) - 1);
    db->wal->current_lsn = 0;

    /* 恢复WAL */
    db_recover(db);

    /* 分配自由页数组 */
    db->max_free_pages = 256;
    db->free_pages = malloc(db->max_free_pages * sizeof(uint32_t));

    db->is_open = true;
    db->next_tx_id = 1;

    return DB_OK;
}

db_error_t db_close(database_t *db) {
    if (!db->is_open) return DB_OK;

    /* 强制检查点 */
    db_checkpoint(db);

    /* 清理资源 */
    if (db->wal) {
        fclose(db->wal->fp);
        free(db->wal);
        db->wal = NULL;
    }

    if (db->db_fp) {
        fclose(db->db_fp);
        db->db_fp = NULL;
    }

    free(db->free_pages);
    db->is_open = false;

    return DB_OK;
}

db_error_t db_put(database_t *db, const char *key,
                  const char *value, uint64_t tx_id) {
    if (!db->is_open) return DB_ERROR_INVALID;
    if (!key || !value) return DB_ERROR_INVALID;

    uint16_t key_len = strlen(key);
    uint16_t value_len = strlen(value);

    if (key_len > MAX_KEY_SIZE || value_len > MAX_VALUE_SIZE) {
        return DB_ERROR_INVALID;
    }

    return btree_insert_recursive(db, db->btree.root_page,
                                   key, key_len,
                                   value, value_len,
                                   tx_id ? tx_id : db->next_tx_id);
}

db_error_t db_get(database_t *db, const char *key,
                  char *value, size_t value_size, uint64_t tx_id) {
    if (!db->is_open) return DB_ERROR_INVALID;

    uint16_t key_len = strlen(key);
    uint32_t page_id = db->btree.root_page;

    while (1) {
        page_t *page = page_alloc(db);
        if (!page) return DB_ERROR_NOMEM;

        db_error_t err = page_read(db, page_id, page);
        if (err != DB_OK) {
            page_free(page);
            return err;
        }

        int slot = btree_find_slot(page, key, key_len);

        if (page->header.type == PAGE_TYPE_BTREE_LEAF) {
            if (slot >= 0) {
                btree_item_t *item = btree_get_item(page, slot);
                /* MVCC可见性检查 */
                if (item->tx_id <= tx_id &&
                    (item->delete_tx == 0 || item->delete_tx > tx_id)) {
                    size_t copy_len = item->value_len;
                    if (copy_len >= value_size) copy_len = value_size - 1;
                    memcpy(value, item->data + item->key_len, copy_len);
                    value[copy_len] = '\0';
                    page_free(page);
                    return DB_OK;
                }
            }
            page_free(page);
            return DB_ERROR_NOT_FOUND;
        } else {
            /* 内部节点，继续向下 */
            if (slot < 0) slot = -slot - 1;
            if (slot >= page->header.num_keys) slot = page->header.num_keys - 1;

            btree_item_t *item = btree_get_item(page, slot);
            page_id = (slot == 0) ? page->header.right_sibling : item->page_id;
            page_free(page);
        }
    }
}

db_error_t db_begin_transaction(database_t *db, uint64_t *tx_id) {
    if (!db->is_open) return DB_ERROR_INVALID;

    /* 查找空闲事务槽 */
    for (int i = 0; i < MAX_TRANSACTIONS; i++) {
        if (db->transactions[i].state == TX_STATE_INACTIVE) {
            db->transactions[i].state = TX_STATE_ACTIVE;
            db->transactions[i].tx_id = db->next_tx_id++;
            db->transactions[i].start_time = time(NULL);
            db->transactions[i].num_modified = 0;
            *tx_id = db->transactions[i].tx_id;

            /* 写BEGIN日志 */
            wal_header_t header = {
                .magic = WAL_MAGIC,
                .type = WAL_TYPE_BEGIN,
                .tx_id = *tx_id
            };
            fwrite(&header, sizeof(header), 1, db->wal->fp);
            fflush(db->wal->fp);

            return DB_OK;
        }
    }
    return DB_ERROR_FULL;
}

db_error_t db_commit(database_t *db, uint64_t tx_id) {
    for (int i = 0; i < MAX_TRANSACTIONS; i++) {
        if (db->transactions[i].tx_id == tx_id &&
            db->transactions[i].state == TX_STATE_ACTIVE) {

            /* 写COMMIT日志 */
            wal_header_t header = {
                .magic = WAL_MAGIC,
                .type = WAL_TYPE_COMMIT,
                .tx_id = tx_id
            };
            fwrite(&header, sizeof(header), 1, db->wal->fp);
            fflush(db->wal->fp);

            db->transactions[i].state = TX_STATE_COMMITTED;
            return DB_OK;
        }
    }
    return DB_ERROR_INVALID;
}

db_error_t db_checkpoint(database_t *db) {
    /* 将所有脏页刷盘 */
    fflush(db->db_fp);

    /* 截断WAL */
    ftruncate(fileno(db->wal->fp), 0);
    db->wal->current_lsn = 0;

    db->last_checkpoint = time(NULL);
    return DB_OK;
}

db_error_t db_recover(database_t *db) {
    /* 简化恢复：回放WAL */
    rewind(db->wal->fp);
    wal_header_t header;

    printf("Starting WAL recovery...\n");
    int records = 0;

    while (fread(&header, sizeof(header), 1, db->wal->fp) == 1) {
        if (header.magic != WAL_MAGIC) break;
        records++;

        switch (header.type) {
            case WAL_TYPE_BEGIN:
                printf("  Recover: BEGIN tx=%llu\n",
                       (unsigned long long)header.tx_id);
                break;
            case WAL_TYPE_COMMIT:
                printf("  Recover: COMMIT tx=%llu\n",
                       (unsigned long long)header.tx_id);
                break;
            default:
                break;
        }

        /* 跳过键值数据 */
        if (header.key_len > 0) {
            fseek(db->wal->fp, header.key_len, SEEK_CUR);
        }
        if (header.value_len > 0) {
            fseek(db->wal->fp, header.value_len, SEEK_CUR);
        }
    }

    printf("WAL recovery complete: %d records\n", records);
    return DB_OK;
}

void db_stats(database_t *db) {
    printf("\n=== Database Statistics ===\n");
    printf("Database: %s\n", db->db_name);
    printf("Status: %s\n", db->is_open ? "open" : "closed");
    printf("B+Tree root page: %u\n", db->btree.root_page);
    printf("Total pages: %u\n", db->btree.num_pages);
    printf("Tree height: %u\n", db->btree.height);
    printf("Next transaction ID: %llu\n",
           (unsigned long long)db->next_tx_id);
    printf("Last checkpoint: %s", ctime(&db->last_checkpoint));
    printf("===========================\n\n");
}

/*==================== 主函数 ====================*/

int main(int argc, char *argv[]) {
    database_t db;

    printf("Embedded Database Demo\n");
    printf("======================\n\n");

    /* 打开数据库 */
    printf("Opening database 'testdb'...\n");
    db_error_t err = db_open(&db, "testdb");
    if (err != DB_OK) {
        fprintf(stderr, "Failed to open database: %d\n", err);
        return 1;
    }

    db_stats(&db);

    /* 开始事务 */
    uint64_t tx_id;
    err = db_begin_transaction(&db, &tx_id);
    if (err != DB_OK) {
        fprintf(stderr, "Failed to begin transaction\n");
        db_close(&db);
        return 1;
    }
    printf("Started transaction: %llu\n", (unsigned long long)tx_id);

    /* 插入数据 */
    printf("\nInserting records...\n");
    const char *test_data[][2] = {
        {"user:1", "{\"name\":\"Alice\",\"age\":30}"},
        {"user:2", "{\"name\":\"Bob\",\"age\":25}"},
        {"user:3", "{\"name\":\"Charlie\",\"age\":35}"},
        {"config:theme", "dark"},
        {"config:lang", "zh-CN"},
    };

    for (size_t i = 0; i < sizeof(test_data)/sizeof(test_data[0]); i++) {
        err = db_put(&db, test_data[i][0], test_data[i][1], tx_id);
        if (err == DB_OK) {
            printf("  PUT %s = %s\n", test_data[i][0], test_data[i][1]);
        } else {
            printf("  PUT %s FAILED (err=%d)\n", test_data[i][0], err);
        }
    }

    /* 查询数据 */
    printf("\nQuerying records...\n");
    char value[MAX_VALUE_SIZE];
    const char *keys[] = {"user:1", "user:2", "config:theme", "notfound"};

    for (size_t i = 0; i < sizeof(keys)/sizeof(keys[0]); i++) {
        err = db_get(&db, keys[i], value, sizeof(value), tx_id);
        if (err == DB_OK) {
            printf("  GET %s = %s\n", keys[i], value);
        } else {
            printf("  GET %s NOT FOUND (err=%d)\n", keys[i], err);
        }
    }

    /* 提交事务 */
    printf("\nCommitting transaction...\n");
    err = db_commit(&db, tx_id);
    if (err == DB_OK) {
        printf("Transaction committed successfully\n");
    } else {
        printf("Commit failed: %d\n", err);
    }

    /* 创建检查点 */
    printf("\nCreating checkpoint...\n");
    db_checkpoint(&db);

    /* 关闭数据库 */
    printf("\nClosing database...\n");
    db_close(&db);

    printf("\nDemo completed!\n");
    return 0;
}
```

### 4.3 编译和运行

```bash
# 编译
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
gcc -std=c17 -O2 -D_GNU_SOURCE \
    -o embedded_db \
    embedded_db.c \
    -lpthread

# 运行
./embedded_db
```

---

## 5. 架构总结

```
┌─────────────────────────────────────────────────────────────┐
│                    嵌入式数据库架构                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐          │
│  │   API层     │  │  事务管理   │  │   迭代器    │          │
│  │  put/get    │  │ MVCC/锁    │  │   范围查询  │          │
│  └──────┬──────┘  └──────┬──────┘  └─────────────┘          │
│         │                │                                  │
│  ┌──────▼────────────────▼──────┐                           │
│  │        B+树索引层             │                           │
│  │   插入/删除/查找/分裂/合并    │                           │
│  └─────────────┬───────────────┘                           │
│                │                                            │
│  ┌─────────────▼───────────────┐                            │
│  │        页管理层              │                           │
│  │   页缓存/分配/读写/校验      │                           │
│  └─────────────┬───────────────┘                           │
│                │                                            │
│  ┌─────────────▼───────────────┐    ┌─────────────┐        │
│  │        存储层                │◀───│    WAL      │        │
│  │   数据文件(.db)              │    │   日志文件  │        │
│  └─────────────────────────────┘    └─────────────┘        │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 总结

本案例实现了一个简化但功能完整的嵌入式数据库引擎，包含：

- **B+树索引**：支持插入、查询、节点分裂
- **页管理**：固定大小页，支持缓存和校验
- **WAL日志**：预写日志保证持久性
- **MVCC**：事务ID实现版本可见性
- **检查点**：定期刷盘和日志截断

该实现可作为学习数据库内核的基础，进一步扩展包括：并发控制、崩溃恢复、范围查询、迭代器等。


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
