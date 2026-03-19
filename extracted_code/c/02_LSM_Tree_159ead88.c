/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\02_LSM_Tree.md
 * Line: 105
 * Language: c
 * Block ID: 159ead88
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
