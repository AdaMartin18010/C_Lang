/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\02_Database_Engine.md
 * Line: 422
 * Language: c
 * Block ID: ecf5f3f3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
