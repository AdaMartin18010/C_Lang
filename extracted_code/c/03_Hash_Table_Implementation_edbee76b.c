/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\03_Hash_Table_Implementation.md
 * Line: 393
 * Language: c
 * Block ID: edbee76b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 开放寻址哈希表实现
 */

typedef enum {
    ENTRY_EMPTY,      // 从未使用过
    ENTRY_DELETED,    // 被删除(墓碑)
    ENTRY_OCCUPIED    // 占用
} EntryState;

typedef struct {
    int key;
    int value;
    EntryState state;
} HashEntry;

typedef struct {
    HashEntry* entries;
    size_t capacity;      // 表容量
    size_t size;          // 元素数量
    size_t tombstones;    // 墓碑数量
    double max_load;      // 最大负载因子
    double min_load;      // 最小负载因子 (用于缩容)
} OpenAddressHashTable;

#define OA_DEFAULT_CAPACITY 16
#define OA_MAX_LOAD_FACTOR 0.75
#define OA_MIN_LOAD_FACTOR 0.10

// 初始化
void oa_hash_init(OpenAddressHashTable* table) {
    table->capacity = OA_DEFAULT_CAPACITY;
    table->entries = (HashEntry*)calloc(table->capacity, sizeof(HashEntry));
    if (table->entries == nullptr) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    table->size = 0;
    table->tombstones = 0;
    table->max_load = OA_MAX_LOAD_FACTOR;
    table->min_load = OA_MIN_LOAD_FACTOR;
}

// 销毁
void oa_hash_destroy(OpenAddressHashTable* table) {
    free(table->entries);
    table->entries = nullptr;
    table->capacity = 0;
    table->size = 0;
    table->tombstones = 0;
}

// 哈希函数
static inline unsigned int oa_hash(int key, size_t capacity) {
    return (unsigned int)(key & 0x7FFFFFFF) % (unsigned int)capacity;
}

// 查找插入位置或键的位置 (线性探测)
static size_t oa_find_slot(const OpenAddressHashTable* table, int key, bool* found) {
    unsigned int h = oa_hash(key, table->capacity);
    *found = false;

    for (size_t i = 0; i < table->capacity; i++) {
        size_t idx = (h + i) % table->capacity;

        if (table->entries[idx].state == ENTRY_EMPTY) {
            return idx;  // 找到空槽
        }

        if (table->entries[idx].state == ENTRY_OCCUPIED &&
            table->entries[idx].key == key) {
            *found = true;
            return idx;  // 找到键
        }
    }

    return (size_t)-1;  // 表满
}

// 扩容/缩容
static void oa_resize(OpenAddressHashTable* table, size_t new_capacity) {
    HashEntry* old_entries = table->entries;
    size_t old_capacity = table->capacity;

    table->capacity = new_capacity;
    table->entries = (HashEntry*)calloc(table->capacity, sizeof(HashEntry));
    table->size = 0;
    table->tombstones = 0;

    // 重新插入所有有效元素
    for (size_t i = 0; i < old_capacity; i++) {
        if (old_entries[i].state == ENTRY_OCCUPIED) {
            bool found;
            size_t idx = oa_find_slot(table, old_entries[i].key, &found);
            table->entries[idx].key = old_entries[i].key;
            table->entries[idx].value = old_entries[i].value;
            table->entries[idx].state = ENTRY_OCCUPIED;
            table->size++;
        }
    }

    free(old_entries);
}

// 插入
bool oa_hash_insert(OpenAddressHashTable* table, int key, int value) {
    // 检查是否需要扩容
    if ((double)(table->size + table->tombstones) / table->capacity > table->max_load) {
        oa_resize(table, table->capacity * 2);
    }

    bool found;
    size_t idx = oa_find_slot(table, key, &found);

    if (idx == (size_t)-1) {
        fprintf(stderr, "Error: Hash table is full\n");
        return false;
    }

    if (found) {
        // 更新已有键
        table->entries[idx].value = value;
    } else {
        // 插入新键
        if (table->entries[idx].state == ENTRY_DELETED) {
            table->tombstones--;
        }
        table->entries[idx].key = key;
        table->entries[idx].value = value;
        table->entries[idx].state = ENTRY_OCCUPIED;
        table->size++;
    }

    return true;
}

// 查找
bool oa_hash_find(const OpenAddressHashTable* table, int key, int* out_value) {
    bool found;
    size_t idx = oa_find_slot(table, key, &found);

    if (found) {
        *out_value = table->entries[idx].value;
        return true;
    }
    return false;
}

// 删除
bool oa_hash_delete(OpenAddressHashTable* table, int key) {
    bool found;
    size_t idx = oa_find_slot(table, key, &found);

    if (!found) return false;

    table->entries[idx].state = ENTRY_DELETED;
    table->size++;
    table->tombstones++;

    // 检查是否需要缩容
    if (table->capacity > OA_DEFAULT_CAPACITY &&
        (double)table->size / table->capacity < table->min_load) {
        oa_resize(table, table->capacity / 2);
    }

    return true;
}

// 打印
void oa_hash_print(const OpenAddressHashTable* table) {
    printf("OpenAddressHashTable [size=%zu, capacity=%zu, load=%.2f%%]:\n",
           table->size, table->capacity,
           100.0 * table->size / table->capacity);

    for (size_t i = 0; i < table->capacity; i++) {
        printf("  [%3zu] ", i);
        switch (table->entries[i].state) {
            case ENTRY_EMPTY:
                printf("[EMPTY]\n");
                break;
            case ENTRY_DELETED:
                printf("[DELETED]\n");
                break;
            case ENTRY_OCCUPIED:
                printf("key=%d, value=%d\n",
                       table->entries[i].key, table->entries[i].value);
                break;
        }
    }
}
