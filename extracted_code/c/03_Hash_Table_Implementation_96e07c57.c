/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\03_Hash_Table_Implementation.md
 * Line: 588
 * Language: c
 * Block ID: 96e07c57
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 链地址法哈希表实现
 * 每个槽位维护一个链表
 */

typedef struct ChainNode {
    int key;
    int value;
    struct ChainNode* next;
} ChainNode;

typedef struct {
    ChainNode** buckets;    // 桶数组
    size_t capacity;
    size_t size;
    double max_load;
} ChainHashTable;

#define CH_DEFAULT_CAPACITY 16
#define CH_MAX_LOAD_FACTOR 0.75

// 初始化
void chain_hash_init(ChainHashTable* table) {
    table->capacity = CH_DEFAULT_CAPACITY;
    table->buckets = (ChainNode**)calloc(table->capacity, sizeof(ChainNode*));
    if (table->buckets == nullptr) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    table->size = 0;
    table->max_load = CH_MAX_LOAD_FACTOR;
}

// 销毁
void chain_hash_destroy(ChainHashTable* table) {
    for (size_t i = 0; i < table->capacity; i++) {
        ChainNode* current = table->buckets[i];
        while (current != nullptr) {
            ChainNode* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(table->buckets);
    table->buckets = nullptr;
    table->capacity = 0;
    table->size = 0;
}

// 哈希函数
static inline unsigned int chain_hash(int key, size_t capacity) {
    return (unsigned int)(key & 0x7FFFFFFF) % (unsigned int)capacity;
}

// 扩容
static void chain_resize(ChainHashTable* table, size_t new_capacity) {
    ChainNode** old_buckets = table->buckets;
    size_t old_capacity = table->capacity;

    table->capacity = new_capacity;
    table->buckets = (ChainNode**)calloc(table->capacity, sizeof(ChainNode*));
    table->size = 0;

    // 重新插入所有元素
    for (size_t i = 0; i < old_capacity; i++) {
        ChainNode* current = old_buckets[i];
        while (current != nullptr) {
            // 不直接使用insert避免递归扩容
            unsigned int h = chain_hash(current->key, table->capacity);
            ChainNode* new_node = (ChainNode*)malloc(sizeof(ChainNode));
            new_node->key = current->key;
            new_node->value = current->value;
            new_node->next = table->buckets[h];
            table->buckets[h] = new_node;
            table->size++;

            ChainNode* temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(old_buckets);
}

// 插入
bool chain_hash_insert(ChainHashTable* table, int key, int value) {
    // 检查负载因子
    if ((double)table->size / table->capacity > table->max_load) {
        chain_resize(table, table->capacity * 2);
    }

    unsigned int h = chain_hash(key, table->capacity);

    // 检查是否已存在
    ChainNode* current = table->buckets[h];
    while (current != nullptr) {
        if (current->key == key) {
            current->value = value;  // 更新
            return true;
        }
        current = current->next;
    }

    // 插入新节点 (头部插入)
    ChainNode* new_node = (ChainNode*)malloc(sizeof(ChainNode));
    new_node->key = key;
    new_node->value = value;
    new_node->next = table->buckets[h];
    table->buckets[h] = new_node;
    table->size++;

    return true;
}

// 查找
bool chain_hash_find(const ChainHashTable* table, int key, int* out_value) {
    unsigned int h = chain_hash(key, table->capacity);
    ChainNode* current = table->buckets[h];

    while (current != nullptr) {
        if (current->key == key) {
            *out_value = current->value;
            return true;
        }
        current = current->next;
    }

    return false;
}

// 删除
bool chain_hash_delete(ChainHashTable* table, int key) {
    unsigned int h = chain_hash(key, table->capacity);
    ChainNode* current = table->buckets[h];
    ChainNode* prev = nullptr;

    while (current != nullptr) {
        if (current->key == key) {
            if (prev == nullptr) {
                table->buckets[h] = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            table->size--;
            return true;
        }
        prev = current;
        current = current->next;
    }

    return false;
}

// 打印
void chain_hash_print(const ChainHashTable* table) {
    printf("ChainHashTable [size=%zu, capacity=%zu, load=%.2f%%]:\n",
           table->size, table->capacity,
           100.0 * table->size / table->capacity);

    for (size_t i = 0; i < table->capacity; i++) {
        printf("  Bucket[%2zu]: ", i);
        if (table->buckets[i] == nullptr) {
            printf("NULL\n");
        } else {
            ChainNode* current = table->buckets[i];
            while (current != nullptr) {
                printf("(%d:%d)", current->key, current->value);
                if (current->next != nullptr) printf(" -> ");
                current = current->next;
            }
            printf("\n");
        }
    }
}
