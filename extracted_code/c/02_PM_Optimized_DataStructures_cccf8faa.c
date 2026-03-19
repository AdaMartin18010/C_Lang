/*
 * Auto-generated from: 03_System_Technology_Domains\12_Persistent_Memory\02_PM_Optimized_DataStructures.md
 * Line: 417
 * Language: c
 * Block ID: cccf8faa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* PM优化的线性探测哈希表 */
#define HTABLE_SIZE (1024 * 1024)  /* 必须是2的幂 */
#define HTABLE_MASK (HTABLE_SIZE - 1)
#define TOMBSTONE ((PMEMoid){0, 1})

struct htable_entry {
    uint64_t hash;            /* 完整哈希值 */
    PMEMoid key;              /* 键OID */
    PMEMoid value;            /* 值OID */
};

struct htable {
    uint64_t size;            /* 条目数 */
    uint64_t capacity;        /* 容量 */
    TOID(struct htable_entry) entries;  /* 条目数组 */
};

/* 无锁读取 */
PMEMoid htable_get(PMEMobjpool *pop, TOID(struct htable) ht,
                   const char *key, size_t key_len) {
    uint64_t hash = hash_fn(key, key_len);
    uint64_t idx = hash & HTABLE_MASK;

    struct htable_entry *entries = D_RW(D_RW(ht)->entries);

    while (true) {
        struct htable_entry *e = &entries[idx];

        if (OID_IS_NULL(e->key)) {
            return OID_NULL;  /* 未找到 */
        }

        if (e->hash == hash && key_equal(e->key, key, key_len)) {
            return e->value;
        }

        idx = (idx + 1) & HTABLE_MASK;
    }
}

/* 事务插入 */
int htable_insert(PMEMobjpool *pop, TOID(struct htable) ht,
                  const char *key, size_t key_len, PMEMoid value) {
    TX_BEGIN(pop) {
        /* 检查负载因子... */

        uint64_t hash = hash_fn(key, key_len);
        uint64_t idx = hash & HTABLE_MASK;

        struct htable_entry *entries = D_RW(D_RW(ht)->entries);

        while (true) {
            struct htable_entry *e = &entries[idx];

            if (OID_IS_NULL(e->key) || e->key.oid == TOMBSTONE.oid) {
                /* 找到空槽 */
                PMEMoid key_oid = TX_ALLOC(uint8_t, key_len);
                memcpy(D_RW(key_oid), key, key_len);

                TX_ADD_DIRECT(e);
                e->hash = hash;
                e->key = key_oid;
                e->value = value;

                TX_SET(ht, size, D_RO(ht)->size + 1);
                break;
            }

            if (e->hash == hash && key_equal(e->key, key, key_len)) {
                /* 更新现有值 */
                TX_ADD_DIRECT(e);
                e->value = value;
                break;
            }

            idx = (idx + 1) & HTABLE_MASK;
        }

    } TX_ONABORT {
        return -1;
    } TX_END

    return 0;
}
