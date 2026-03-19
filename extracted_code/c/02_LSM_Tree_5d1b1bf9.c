/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\02_LSM_Tree.md
 * Line: 197
 * Language: c
 * Block ID: 5d1b1bf9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
