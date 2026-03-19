# LSM树内存数据库实现

## 目录

- [LSM树内存数据库实现](#lsm树内存数据库实现)
  - [目录](#目录)
  - [概述](#概述)
  - [LSM树数据结构原理](#lsm树数据结构原理)
    - [核心组件](#核心组件)
    - [写入流程](#写入流程)
    - [读取流程](#读取流程)
  - [放大问题分析](#放大问题分析)
    - [写放大（Write Amplification）](#写放大write-amplification)
    - [读放大（Read Amplification）](#读放大read-amplification)
    - [空间放大（Space Amplification）](#空间放大space-amplification)
  - [C语言实现](#c语言实现)
    - [头文件定义](#头文件定义)
    - [核心实现](#核心实现)
  - [代码示例](#代码示例)
    - [完整测试程序](#完整测试程序)
    - [编译运行](#编译运行)
  - [总结](#总结)

---

## 概述

LSM树（Log-Structured Merge-Tree）是一种为写密集型工作负载优化的数据结构，由Patrick O'Neil等人在1996年提出。
它广泛应用于现代NoSQL数据库（如RocksDB、LevelDB、Cassandra）中，通过将随机写转换为顺序写来显著提升写入性能。

## LSM树数据结构原理

### 核心组件

```text
┌─────────────────────────────────────────────┐
│                  LSM Tree                    │
├─────────────────────────────────────────────┤
│  MemTable (内存) - 跳表/红黑树              │
│  └─ WAL (预写日志)                           │
├─────────────────────────────────────────────┤
│  Immutable MemTable (待刷盘)                │
├─────────────────────────────────────────────┤
│  Level 0 SSTable (磁盘) - 允许重叠          │
├─────────────────────────────────────────────┤
│  Level 1 SSTable - 已排序，无重叠           │
├─────────────────────────────────────────────┤
│  Level 2...N SSTable - 容量呈指数增长       │
└─────────────────────────────────────────────┘
```

### 写入流程

1. **追加WAL**：保证持久性，系统崩溃可恢复
2. **写入MemTable**：插入内存中的有序数据结构
3. **MemTable刷盘**：达到阈值后转为Immutable，后台刷入Level 0
4. **Compaction**：后台合并SSTable，清理过期数据

### 读取流程

1. 查询MemTable
2. 查询Immutable MemTable
3. 按时间序查询Level 0 SSTable
4. 按key范围查询Level 1+ SSTable
5. 使用布隆过滤器加速不存在key的判定

## 放大问题分析

### 写放大（Write Amplification）

写放大 = 实际写入磁盘的数据量 / 用户写入的数据量

| 层级 | 放大因子 | 说明 |
|------|----------|------|
| Level 0→1 | ~10x | 初始排序和去重 |
| Level 1→2 | ~1x | 已排序数据合并 |
| 总计 | 10-30x | 取决于配置和workload |

**优化策略**：

- 使用**分层压缩**（Level Compaction）或**通用压缩**（Universal Compaction）
- 调整层级大小倍数（通常10倍）
- 采用**并行压缩**减少stall

### 读放大（Read Amplification）

读放大 = 为读取一个key需要读取的页面数

```text
最坏情况：MemTable + Immutable + L0所有文件 + L1~LN各1个文件
优化：布隆过滤器可将不存在key的查询降为O(1)
```

### 空间放大（Space Amplification）

空间放大 = 数据库实际占用空间 / 数据逻辑大小

- 来源：旧版本数据未清理、压缩未完成
- 典型值：1.1x - 1.3x（取决于压缩策略）

## C语言实现

### 头文件定义

```c
#ifndef LSM_TREE_H
#define LSM_TREE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* MISRA C:2012 合规定义 */
#define LSM_MAX_KEY_SIZE        (256U)
#define LSM_MAX_VALUE_SIZE      (4096U)
#define LSM_MAX_MEMTABLE_SIZE   (1048576U)  /* 1MB */
#define LSM_MAX_LEVELS          (7U)
#define LSM_MAX_FILES_PER_LEVEL (10U)
#define LSM_BLOCK_SIZE          (4096U)
#define LSM_BLOOM_BITS_PER_KEY  (10U)

/* 操作类型 */
typedef enum {
    LSM_OP_PUT = 0U,
    LSM_OP_DELETE = 1U
} lsm_op_type_t;

/* 内部键值对结构 */
typedef struct lsm_entry {
    uint32_t key_len;
    uint32_t value_len;
    uint64_t sequence;
    lsm_op_type_t type;
    uint8_t *key;
    uint8_t *value;
    struct lsm_entry *next;
} lsm_entry_t;

/* MemTable - 使用跳表 */
typedef struct {
    lsm_entry_t *head;
    lsm_entry_t *tail;
    uint32_t entry_count;
    uint32_t data_size;
    uint64_t sequence_counter;
} lsm_memtable_t;

/* SSTable元数据 */
typedef struct {
    uint32_t file_id;
    uint32_t level;
    uint64_t file_size;
    uint8_t smallest_key[LSM_MAX_KEY_SIZE];
    uint8_t largest_key[LSM_MAX_KEY_SIZE];
    uint32_t smallest_key_len;
    uint32_t largest_key_len;
    uint32_t entry_count;
    uint64_t *bloom_filter;
    uint32_t bloom_size;
} lsm_sstable_meta_t;

/* SSTable */
typedef struct {
    lsm_sstable_meta_t meta;
    int32_t file_handle;
    uint8_t *index_block;
    uint32_t index_size;
} lsm_sstable_t;

/* LSM树主结构 */
typedef struct {
    lsm_memtable_t *memtable;
    lsm_memtable_t *immutable;
    lsm_sstable_t *levels[LSM_MAX_LEVELS][LSM_MAX_FILES_PER_LEVEL];
    uint32_t level_file_count[LSM_MAX_LEVELS];
    uint32_t next_file_id;
    char *db_path;
    uint64_t total_write_bytes;
    uint64_t total_read_bytes;
} lsm_tree_t;

/* API声明 */
lsm_tree_t* lsm_create(const char *path);
void lsm_destroy(lsm_tree_t *lsm);
bool lsm_put(lsm_tree_t *lsm, const uint8_t *key, uint32_t key_len,
             const uint8_t *value, uint32_t value_len);
bool lsm_get(lsm_tree_t *lsm, const uint8_t *key, uint32_t key_len,
             uint8_t *value, uint32_t *value_len);
bool lsm_delete(lsm_tree_t *lsm, const uint8_t *key, uint32_t key_len);
void lsm_compact(lsm_tree_t *lsm, uint32_t level);

#endif /* LSM_TREE_H */
```

### 核心实现

```c
#include "lsm_tree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/* 简单的布隆过滤器实现 */
static uint32_t bloom_hash(const uint8_t *key, uint32_t len, uint32_t seed)
{
    uint32_t hash = seed;
    for (uint32_t i = 0U; i < len; i++) {
        hash = hash * 31U + key[i];
    }
    return hash;
}

static bool bloom_check(const lsm_sstable_meta_t *meta,
                        const uint8_t *key, uint32_t key_len)
{
    if ((meta == NULL) || (meta->bloom_filter == NULL)) {
        return true;  /* 无过滤器时保守返回true */
    }

    for (uint32_t i = 0U; i < 3U; i++) {
        const uint32_t hash = bloom_hash(key, key_len, i * 0x9E3779B9U);
        const uint32_t bit = hash % (meta->bloom_size * 64U);
        const uint32_t word = bit >> 6U;
        const uint32_t bit_mask = 1UL << (bit & 0x3FU);

        if ((meta->bloom_filter[word] & bit_mask) == 0U) {
            return false;
        }
    }
    return true;
}

/* 创建MemTable */
static lsm_memtable_t* memtable_create(void)
{
    lsm_memtable_t *mt = (lsm_memtable_t*)malloc(sizeof(lsm_memtable_t));
    if (mt != NULL) {
        mt->head = NULL;
        mt->tail = NULL;
        mt->entry_count = 0U;
        mt->data_size = 0U;
        mt->sequence_counter = 0U;
    }
    return mt;
}

/* 销毁MemTable */
static void memtable_destroy(lsm_memtable_t *mt)
{
    if (mt != NULL) {
        lsm_entry_t *current = mt->head;
        while (current != NULL) {
            lsm_entry_t *next = current->next;
            free(current->key);
            free(current->value);
            free(current);
            current = next;
        }
        free(mt);
    }
}

/* MemTable查找 - 顺序扫描（简化版，实际使用跳表） */
static lsm_entry_t* memtable_find(lsm_memtable_t *mt,
                                  const uint8_t *key, uint32_t key_len)
{
    if (mt == NULL) {
        return NULL;
    }

    lsm_entry_t *current = mt->head;
    while (current != NULL) {
        if ((current->key_len == key_len) &&
            (memcmp(current->key, key, key_len) == 0)) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/* MemTable插入 - 保持有序 */
static bool memtable_insert(lsm_memtable_t *mt,
                            const uint8_t *key, uint32_t key_len,
                            const uint8_t *value, uint32_t value_len,
                            lsm_op_type_t type)
{
    if ((mt == NULL) || (key == NULL)) {
        return false;
    }

    /* 检查重复键，更新值 */
    lsm_entry_t *existing = memtable_find(mt, key, key_len);
    if (existing != NULL) {
        if (value_len > 0U) {
            uint8_t *new_value = (uint8_t*)realloc(existing->value, value_len);
            if (new_value == NULL) {
                return false;
            }
            existing->value = new_value;
            (void)memcpy(existing->value, value, value_len);
            existing->value_len = value_len;
        }
        existing->type = type;
        existing->sequence = mt->sequence_counter++;
        return true;
    }

    /* 创建新条目 */
    lsm_entry_t *entry = (lsm_entry_t*)malloc(sizeof(lsm_entry_t));
    if (entry == NULL) {
        return false;
    }

    entry->key = (uint8_t*)malloc(key_len);
    entry->value = (value_len > 0U) ? (uint8_t*)malloc(value_len) : NULL;

    if ((entry->key == NULL) || ((value_len > 0U) && (entry->value == NULL))) {
        free(entry->key);
        free(entry->value);
        free(entry);
        return false;
    }

    (void)memcpy(entry->key, key, key_len);
    if (value_len > 0U) {
        (void)memcpy(entry->value, value, value_len);
    }

    entry->key_len = key_len;
    entry->value_len = value_len;
    entry->type = type;
    entry->sequence = mt->sequence_counter++;
    entry->next = NULL;

    /* 有序插入 */
    if ((mt->head == NULL) ||
        (memcmp(key, mt->head->key, (key_len < mt->head->key_len) ?
                key_len : mt->head->key_len) < 0)) {
        entry->next = mt->head;
        mt->head = entry;
        if (mt->tail == NULL) {
            mt->tail = entry;
        }
    } else {
        lsm_entry_t *prev = mt->head;
        while ((prev->next != NULL) &&
               (memcmp(key, prev->next->key, key_len) > 0)) {
            prev = prev->next;
        }
        entry->next = prev->next;
        prev->next = entry;
        if (entry->next == NULL) {
            mt->tail = entry;
        }
    }

    mt->entry_count++;
    mt->data_size += key_len + value_len + sizeof(lsm_entry_t);

    return true;
}

/* 创建LSM树 */
lsm_tree_t* lsm_create(const char *path)
{
    if (path == NULL) {
        return NULL;
    }

    lsm_tree_t *lsm = (lsm_tree_t*)malloc(sizeof(lsm_tree_t));
    if (lsm == NULL) {
        return NULL;
    }

    lsm->memtable = memtable_create();
    lsm->immutable = NULL;
    lsm->next_file_id = 0U;
    lsm->total_write_bytes = 0U;
    lsm->total_read_bytes = 0U;

    for (uint32_t i = 0U; i < LSM_MAX_LEVELS; i++) {
        lsm->level_file_count[i] = 0U;
        for (uint32_t j = 0U; j < LSM_MAX_FILES_PER_LEVEL; j++) {
            lsm->levels[i][j] = NULL;
        }
    }

    const size_t path_len = strlen(path) + 1U;
    lsm->db_path = (char*)malloc(path_len);
    if (lsm->db_path != NULL) {
        (void)memcpy(lsm->db_path, path, path_len);
    }

    return lsm;
}

/* 销毁LSM树 */
void lsm_destroy(lsm_tree_t *lsm)
{
    if (lsm != NULL) {
        memtable_destroy(lsm->memtable);
        memtable_destroy(lsm->immutable);

        for (uint32_t i = 0U; i < LSM_MAX_LEVELS; i++) {
            for (uint32_t j = 0U; j < lsm->level_file_count[i]; j++) {
                if (lsm->levels[i][j] != NULL) {
                    free(lsm->levels[i][j]->meta.bloom_filter);
                    free(lsm->levels[i][j]->index_block);
                    free(lsm->levels[i][j]);
                }
            }
        }

        free(lsm->db_path);
        free(lsm);
    }
}

/* 写入键值对 */
bool lsm_put(lsm_tree_t *lsm, const uint8_t *key, uint32_t key_len,
             const uint8_t *value, uint32_t value_len)
{
    if ((lsm == NULL) || (key == NULL) || (key_len == 0U) ||
        (key_len > LSM_MAX_KEY_SIZE) || (value_len > LSM_MAX_VALUE_SIZE)) {
        return false;
    }

    /* 写入MemTable */
    const bool inserted = memtable_insert(lsm->memtable, key, key_len,
                                          value, value_len, LSM_OP_PUT);
    if (!inserted) {
        return false;
    }

    lsm->total_write_bytes += key_len + value_len;

    /* 检查是否需要刷盘 */
    if (lsm->memtable->data_size >= LSM_MAX_MEMTABLE_SIZE) {
        /* 冻结MemTable */
        lsm->immutable = lsm->memtable;
        lsm->memtable = memtable_create();

        /* 触发后台刷盘（简化：同步执行） */
        lsm_flush_memtable(lsm);
    }

    return true;
}

/* 读取键值对 */
bool lsm_get(lsm_tree_t *lsm, const uint8_t *key, uint32_t key_len,
             uint8_t *value, uint32_t *value_len)
{
    if ((lsm == NULL) || (key == NULL) || (value_len == NULL)) {
        return false;
    }

    /* 查询MemTable */
    lsm_entry_t *entry = memtable_find(lsm->memtable, key, key_len);
    if (entry != NULL) {
        if (entry->type == LSM_OP_DELETE) {
            return false;
        }
        if ((value != NULL) && (*value_len >= entry->value_len)) {
            (void)memcpy(value, entry->value, entry->value_len);
        }
        *value_len = entry->value_len;
        return true;
    }

    /* 查询Immutable MemTable */
    if (lsm->immutable != NULL) {
        entry = memtable_find(lsm->immutable, key, key_len);
        if (entry != NULL) {
            if (entry->type == LSM_OP_DELETE) {
                return false;
            }
            if ((value != NULL) && (*value_len >= entry->value_len)) {
                (void)memcpy(value, entry->value, entry->value_len);
            }
            *value_len = entry->value_len;
            return true;
        }
    }

    /* 查询各层级SSTable */
    for (uint32_t level = 0U; level < LSM_MAX_LEVELS; level++) {
        for (uint32_t i = 0U; i < lsm->level_file_count[level]; i++) {
            lsm_sstable_t *sst = lsm->levels[level][i];
            if (sst == NULL) continue;

            /* 布隆过滤器快速判断 */
            if (!bloom_check(&sst->meta, key, key_len)) {
                continue;
            }

            /* 范围检查 */
            if ((key_len < sst->meta.smallest_key_len) &&
                (memcmp(key, sst->meta.smallest_key, key_len) < 0)) {
                continue;
            }
            if ((key_len > sst->meta.largest_key_len) &&
                (memcmp(key, sst->meta.largest_key, key_len) > 0)) {
                continue;
            }

            /* 从SSTable文件读取 */
            uint8_t sst_value[LSM_MAX_VALUE_SIZE];
            uint32_t sst_value_len = LSM_MAX_VALUE_SIZE;
            if (sstable_get(sst, key, key_len, sst_value, &sst_value_len)) {
                if ((value != NULL) && (*value_len >= sst_value_len)) {
                    (void)memcpy(value, sst_value, sst_value_len);
                }
                *value_len = sst_value_len;
                lsm->total_read_bytes += key_len + sst_value_len;
                return true;
            }
        }
    }

    return false;
}

/* 删除键 */
bool lsm_delete(lsm_tree_t *lsm, const uint8_t *key, uint32_t key_len)
{
    return lsm_put(lsm, key, key_len, NULL, 0U) ||
           memtable_insert(lsm->memtable, key, key_len, NULL, 0U, LSM_OP_DELETE);
}

/* 构建布隆过滤器 */
static void bloom_build(lsm_sstable_meta_t *meta, lsm_memtable_t *mt)
{
    if ((meta == NULL) || (mt == NULL)) return;

    /* 计算布隆过滤器大小 */
    meta->bloom_size = (mt->entry_count * LSM_BLOOM_BITS_PER_KEY + 63) / 64;
    meta->bloom_filter = (uint64_t*)calloc(meta->bloom_size, sizeof(uint64_t));
    if (meta->bloom_filter == NULL) {
        meta->bloom_size = 0;
        return;
    }

    /* 插入所有键 */
    lsm_entry_t *entry = mt->head;
    while (entry != NULL) {
        for (uint32_t i = 0U; i < 3U; i++) {
            const uint32_t hash = bloom_hash(entry->key, entry->key_len, i * 0x9E3779B9U);
            const uint32_t bit = hash % (meta->bloom_size * 64U);
            const uint32_t word = bit >> 6U;
            const uint32_t bit_mask = 1UL << (bit & 0x3FU);
            meta->bloom_filter[word] |= bit_mask;
        }
        entry = entry->next;
    }
}

/* SSTable文件格式：
 * [Header]
 * [Bloom Filter]
 * [Index Block]
 * [Data Blocks]
 * [Footer]
 */

/* 将MemTable刷盘为SSTable */
static bool lsm_flush_memtable(lsm_tree_t *lsm)
{
    if ((lsm == NULL) || (lsm->immutable == NULL) ||
        (lsm->level_file_count[0] >= LSM_MAX_FILES_PER_LEVEL)) {
        return false;
    }

    /* 分配SSTable结构 */
    lsm_sstable_t *sst = (lsm_sstable_t*)calloc(1, sizeof(lsm_sstable_t));
    if (sst == NULL) return false;

    /* 设置元数据 */
    sst->meta.file_id = lsm->next_file_id++;
    sst->meta.level = 0;
    sst->meta.entry_count = lsm->immutable->entry_count;

    /* 获取键范围 */
    if (lsm->immutable->head != NULL) {
        lsm_entry_t *first = lsm->immutable->head;
        lsm_entry_t *last = lsm->immutable->tail;
        sst->meta.smallest_key_len = first->key_len;
        sst->meta.largest_key_len = last->key_len;
        (void)memcpy(sst->meta.smallest_key, first->key, first->key_len);
        (void)memcpy(sst->meta.largest_key, last->key, last->key_len);
    }

    /* 构建布隆过滤器 */
    bloom_build(&sst->meta, lsm->immutable);

    /* 构建数据块（内存表示） */
    uint32_t total_size = 0;
    lsm_entry_t *entry = lsm->immutable->head;
    while (entry != NULL) {
        /* 格式: key_len(4) | value_len(4) | key | value */
        total_size += 8 + entry->key_len + entry->value_len;
        entry = entry->next;
    }

    uint8_t *data_block = (uint8_t*)malloc(total_size);
    if (data_block == NULL) {
        free(sst->meta.bloom_filter);
        free(sst);
        return false;
    }

    /* 序列化数据 */
    uint32_t offset = 0;
    entry = lsm->immutable->head;
    while (entry != NULL) {
        (void)memcpy(&data_block[offset], &entry->key_len, 4);
        offset += 4;
        (void)memcpy(&data_block[offset], &entry->value_len, 4);
        offset += 4;
        (void)memcpy(&data_block[offset], entry->key, entry->key_len);
        offset += entry->key_len;
        if (entry->value_len > 0) {
            (void)memcpy(&data_block[offset], entry->value, entry->value_len);
            offset += entry->value_len;
        }
        entry = entry->next;
    }

    /* 写入文件 */
    char filename[256];
    (void)snprintf(filename, sizeof(filename), "%s/%06u.sst",
                   lsm->db_path, sst->meta.file_id);
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        free(data_block);
        free(sst->meta.bloom_filter);
        free(sst);
        return false;
    }

    /* 写入头部 */
    uint32_t header[8] = {
        0x53535431,  /* "SST1" magic */
        sst->meta.file_id,
        sst->meta.level,
        sst->meta.entry_count,
        sst->meta.smallest_key_len,
        sst->meta.largest_key_len,
        sst->meta.bloom_size,
        total_size
    };
    (void)fwrite(header, sizeof(header), 1, fp);
    (void)fwrite(sst->meta.smallest_key, sst->meta.smallest_key_len, 1, fp);
    (void)fwrite(sst->meta.largest_key, sst->meta.largest_key_len, 1, fp);

    /* 写入布隆过滤器 */
    if (sst->meta.bloom_size > 0) {
        (void)fwrite(sst->meta.bloom_filter, sizeof(uint64_t), sst->meta.bloom_size, fp);
    }

    /* 写入数据 */
    (void)fwrite(data_block, total_size, 1, fp);
    (void)fclose(fp);

    sst->meta.file_size = sizeof(header) + sst->meta.smallest_key_len +
                          sst->meta.largest_key_len +
                          (sst->meta.bloom_size * sizeof(uint64_t)) + total_size;

    /* 保存数据块用于内存查询 */
    sst->index_block = data_block;
    sst->index_size = total_size;

    /* 放入Level 0 */
    uint32_t idx = lsm->level_file_count[0]++;
    lsm->levels[0][idx] = sst;

    /* 释放Immutable MemTable */
    memtable_destroy(lsm->immutable);
    lsm->immutable = NULL;

    return true;
}

/* 从SSTable读取 */
static bool sstable_get(lsm_sstable_t *sst, const uint8_t *key, uint32_t key_len,
                        uint8_t *value, uint32_t *value_len)
{
    if ((sst == NULL) || (key == NULL) || (value_len == NULL)) {
        return false;
    }

    /* 在内存数据块中线性搜索（简化实现） */
    uint32_t offset = 0;
    while (offset < sst->index_size) {
        uint32_t entry_key_len, entry_value_len;
        (void)memcpy(&entry_key_len, &sst->index_block[offset], 4);
        offset += 4;
        (void)memcpy(&entry_value_len, &sst->index_block[offset], 4);
        offset += 4;

        if ((entry_key_len == key_len) &&
            (memcmp(&sst->index_block[offset], key, key_len) == 0)) {
            /* 找到键 */
            offset += entry_key_len;
            if (entry_value_len == 0) {
                /* 删除标记 */
                return false;
            }
            if ((value != NULL) && (*value_len >= entry_value_len)) {
                (void)memcpy(value, &sst->index_block[offset], entry_value_len);
            }
            *value_len = entry_value_len;
            return true;
        }

        offset += entry_key_len + entry_value_len;
    }

    return false;
}

/* Compaction - 合并层级 */
void lsm_compact(lsm_tree_t *lsm, uint32_t level)
{
    if ((lsm == NULL) || (level >= LSM_MAX_LEVELS - 1) ||
        (lsm->level_file_count[level] < LSM_MAX_FILES_PER_LEVEL)) {
        return;
    }

    /* 简化实现：当Level 0满时，合并到Level 1 */
    if ((level == 0) && (lsm->level_file_count[1] < LSM_MAX_FILES_PER_LEVEL)) {
        /* 选择要合并的文件（这里简化：合并所有Level 0文件） */
        for (uint32_t i = 0; i < lsm->level_file_count[0]; i++) {
            lsm_sstable_t *sst = lsm->levels[0][i];
            if (sst == NULL) continue;

            /* 移动到Level 1 */
            uint32_t new_idx = lsm->level_file_count[1]++;
            sst->meta.level = 1;
            lsm->levels[1][new_idx] = sst;
            lsm->levels[0][i] = NULL;

            /* 重命名文件 */
            char old_name[256], new_name[256];
            (void)snprintf(old_name, sizeof(old_name), "%s/%06u.sst",
                           lsm->db_path, sst->meta.file_id);
            (void)snprintf(new_name, sizeof(new_name), "%s/L1_%06u.sst",
                           lsm->db_path, sst->meta.file_id);
            (void)rename(old_name, new_name);
        }
        lsm->level_file_count[0] = 0;

        printf("Compaction: Level 0 -> Level 1 completed\n");
    }
}
```

## 代码示例

### 完整测试程序

```c
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

/* 简化版LSM树实现 */
#define MAX_ENTRIES 1000
#define MAX_KEY_LEN 64
#define MAX_VALUE_LEN 256

typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
    uint64_t sequence;
    bool deleted;
    bool occupied;
} entry_t;

typedef struct {
    entry_t entries[MAX_ENTRIES];
    uint32_t count;
    uint64_t next_sequence;
    uint64_t write_bytes;
    uint64_t read_bytes;
} simple_lsm_t;

/* 创建简化LSM */
simple_lsm_t* simple_lsm_create(void)
{
    simple_lsm_t *lsm = (simple_lsm_t*)calloc(1, sizeof(simple_lsm_t));
    if (lsm != NULL) {
        lsm->next_sequence = 1;
    }
    return lsm;
}

/* 查找槽位 */
static int find_entry(simple_lsm_t *lsm, const char *key)
{
    for (int i = (int)lsm->count - 1; i >= 0; i--) {
        if (lsm->entries[i].occupied &&
            strcmp(lsm->entries[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

/* 写入 */
bool simple_lsm_put(simple_lsm_t *lsm, const char *key, const char *value)
{
    if ((lsm == NULL) || (key == NULL) || (lsm->count >= MAX_ENTRIES)) {
        return false;
    }

    entry_t *e = &lsm->entries[lsm->count];
    strncpy(e->key, key, MAX_KEY_LEN - 1);
    e->key[MAX_KEY_LEN - 1] = '\0';

    if (value != NULL) {
        strncpy(e->value, value, MAX_VALUE_LEN - 1);
        e->value[MAX_VALUE_LEN - 1] = '\0';
        e->deleted = false;
    } else {
        e->value[0] = '\0';
        e->deleted = true;
    }

    e->sequence = lsm->next_sequence++;
    e->occupied = true;
    lsm->count++;
    lsm->write_bytes += strlen(key) + (value ? strlen(value) : 0);

    return true;
}

/* 读取 */
bool simple_lsm_get(simple_lsm_t *lsm, const char *key, char *value)
{
    if ((lsm == NULL) || (key == NULL)) {
        return false;
    }

    int idx = find_entry(lsm, key);
    if (idx < 0) {
        return false;
    }

    entry_t *e = &lsm->entries[idx];
    if (e->deleted) {
        return false;
    }

    if (value != NULL) {
        strcpy(value, e->value);
    }
    lsm->read_bytes += strlen(key) + strlen(e->value);

    return true;
}

/* 删除 */
bool simple_lsm_delete(simple_lsm_t *lsm, const char *key)
{
    return simple_lsm_put(lsm, key, NULL);
}

/* 压缩 - 清理过期和删除的条目 */
void simple_lsm_compact(simple_lsm_t *lsm)
{
    if (lsm == NULL) {
        return;
    }

    /* 标记已处理的key */
    bool processed[MAX_ENTRIES] = {false};
    uint32_t new_count = 0;

    /* 从后向前，保留每个key的最新版本 */
    for (int i = (int)lsm->count - 1; i >= 0; i--) {
        /* 找到key对应的首个槽位 */
        int key_idx = -1;
        for (uint32_t j = 0; j < lsm->count; j++) {
            if (processed[j]) continue;
            if (strcmp(lsm->entries[i].key, lsm->entries[j].key) == 0) {
                key_idx = (int)j;
                break;
            }
        }

        if (key_idx >= 0 && !processed[key_idx]) {
            processed[key_idx] = true;
            if (!lsm->entries[i].deleted) {
                /* 保留有效条目 */
                if (i != (int)new_count) {
                    lsm->entries[new_count] = lsm->entries[i];
                }
                new_count++;
            }
        }
    }

    printf("压缩: %u -> %u 条目\n", lsm->count, new_count);
    lsm->count = new_count;
}

/* 统计信息 */
void simple_lsm_stats(simple_lsm_t *lsm)
{
    printf("\n=== LSM树统计 ===\n");
    printf("总条目数: %u\n", lsm->count);
    printf("写入字节: %llu\n", (unsigned long long)lsm->write_bytes);
    printf("读取字节: %llu\n", (unsigned long long)lsm->read_bytes);
    printf("序列号: %llu\n", (unsigned long long)lsm->next_sequence);
}

/* 主函数 */
int main(void)
{
    printf("=== LSM树内存数据库测试 ===\n\n");

    simple_lsm_t *lsm = simple_lsm_create();
    assert(lsm != NULL);

    /* 测试1: 基本写入和读取 */
    printf("测试1: 基本写入/读取\n");
    simple_lsm_put(lsm, "user:1", "Alice");
    simple_lsm_put(lsm, "user:2", "Bob");
    simple_lsm_put(lsm, "user:3", "Charlie");

    char value[MAX_VALUE_LEN];
    assert(simple_lsm_get(lsm, "user:1", value) == true);
    printf("  user:1 = %s\n", value);
    assert(strcmp(value, "Alice") == 0);

    /* 测试2: 更新操作 */
    printf("\n测试2: 更新操作\n");
    simple_lsm_put(lsm, "user:1", "Alicia");
    assert(simple_lsm_get(lsm, "user:1", value) == true);
    printf("  user:1 (更新后) = %s\n", value);
    assert(strcmp(value, "Alicia") == 0);

    /* 测试3: 删除操作 */
    printf("\n测试3: 删除操作\n");
    simple_lsm_delete(lsm, "user:2");
    assert(simple_lsm_get(lsm, "user:2", value) == false);
    printf("  user:2 删除后查询: 不存在 ✓\n");

    /* 测试4: 批量写入 */
    printf("\n测试4: 批量写入\n");
    for (int i = 0; i < 100; i++) {
        char k[32], v[32];
        snprintf(k, sizeof(k), "key:%d", i);
        snprintf(v, sizeof(v), "value_%d", i * 10);
        simple_lsm_put(lsm, k, v);
    }
    printf("  写入100条记录\n");

    /* 测试5: 压缩 */
    printf("\n测试5: 压缩\n");
    simple_lsm_stats(lsm);
    simple_lsm_compact(lsm);
    simple_lsm_stats(lsm);

    /* 验证压缩后数据正确 */
    assert(simple_lsm_get(lsm, "user:1", value) == true);
    assert(strcmp(value, "Alicia") == 0);
    assert(simple_lsm_get(lsm, "user:2", value) == false);
    assert(simple_lsm_get(lsm, "key:50", value) == true);
    printf("  压缩后数据验证通过 ✓\n");

    /* 测试6: 写放大计算 */
    printf("\n测试6: 放大分析\n");
    uint64_t logical_writes = 0;
    for (uint32_t i = 0; i < lsm->count; i++) {
        logical_writes += strlen(lsm->entries[i].key) +
                         strlen(lsm->entries[i].value);
    }
    double write_amp = (double)lsm->write_bytes / (double)logical_writes;
    printf("  逻辑写入: %llu bytes\n", (unsigned long long)logical_writes);
    printf("  实际写入: %llu bytes\n", (unsigned long long)lsm->write_bytes);
    printf("  写放大: %.2fx\n", write_amp);

    printf("\n=== 所有测试通过 ===\n");

    free(lsm);
    return 0;
}
```

### 编译运行

```bash
gcc -std=c17 -Wall -O2 -o lsm_test lsm_tree.c
./lsm_test
```

## 总结

LSM树通过以下设计权衡实现高性能写入：

| 特性 | 优点 | 缺点 |
|------|------|------|
| 顺序写 | 磁盘友好，写入快 | - |
| 分层结构 | 查询性能可控 | 读取需多层查找 |
| 后台压缩 | 自动维护数据 | 存在写放大 |

**实际优化建议**：

1. 生产环境使用成熟的RocksDB、LevelDB
2. 根据workload选择分层压缩或通用压缩
3. 配置适当的布隆过滤器减少读放大
4. 监控压缩压力和stall情况

本实现展示了LSM树的核心概念，实际生产系统还需处理并发控制、WAL恢复、缓存等复杂问题。


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
