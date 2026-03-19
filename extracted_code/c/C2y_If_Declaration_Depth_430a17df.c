/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 883
 * Language: c
 * Block ID: 430a17df
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 示例 3: 查找操作中的 if 声明
 * 演示多级查找策略和短路求值
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// 简单哈希表实现
typedef struct HashEntry {
    char* key;
    int value;
    struct HashEntry* next;
} HashEntry;

typedef struct {
    HashEntry** buckets;
    size_t size;
} HashTable;

#define HASH_SIZE 101

unsigned int hash(const char* key) {
    unsigned int h = 0;
    while (*key) {
        h = h * 31 + (unsigned char)*key++;
    }
    return h % HASH_SIZE;
}

HashTable* hash_table_create(void) {
    HashTable* ht = malloc(sizeof(HashTable));
    ht->size = HASH_SIZE;
    ht->buckets = calloc(HASH_SIZE, sizeof(HashEntry*));
    return ht;
}

void hash_table_put(HashTable* ht, const char* key, int value) {
    unsigned int h = hash(key);
    HashEntry* entry = malloc(sizeof(HashEntry));
    entry->key = strdup(key);
    entry->value = value;
    entry->next = ht->buckets[h];
    ht->buckets[h] = entry;
}

int* hash_table_get(HashTable* ht, const char* key) {
    unsigned int h = hash(key);
    for (HashEntry* e = ht->buckets[h]; e != NULL; e = e->next) {
        if (strcmp(e->key, key) == 0) {
            return &e->value;
        }
    }
    return NULL;
}

// 缓存（LRU 简化版）
typedef struct Cache {
    char key[64];
    int value;
    bool valid;
} Cache;

#define CACHE_SIZE 16
static Cache cache[CACHE_SIZE];

int* cache_get(const char* key) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].valid && strcmp(cache[i].key, key) == 0) {
            return &cache[i].value;
        }
    }
    return NULL;
}

void cache_put(const char* key, int value) {
    static int next_slot = 0;
    strncpy(cache[next_slot].key, key, 63);
    cache[next_slot].key[63] = '\0';
    cache[next_slot].value = value;
    cache[next_slot].valid = true;
    next_slot = (next_slot + 1) % CACHE_SIZE;
}

// 多级查找：缓存 -> 哈希表 -> 计算
int get_config_value(const char* key, HashTable* config_table) {
    // 1. 先查缓存
    if (int* cached = cache_get(key); cached != NULL) {
        printf("  [Cache hit] %s = %d\n", key, *cached);
        return *cached;
    }

    // 2. 再查哈希表
    if (int* from_table = hash_table_get(config_table, key); from_table != NULL) {
        printf("  [Table hit] %s = %d\n", key, *from_table);
        // 更新缓存
        cache_put(key, *from_table);
        return *from_table;
    }

    // 3. 使用默认值
    int default_value = 0;
    if (strcmp(key, "timeout") == 0) default_value = 30;
    else if (strcmp(key, "retries") == 0) default_value = 3;
    else if (strcmp(key, "buffer_size") == 0) default_value = 4096;

    printf("  [Default] %s = %d\n", key, default_value);
    cache_put(key, default_value);
    return default_value;
}

// 查找用户（多级回退）
typedef struct User {
    int id;
    char name[64];
    char email[128];
} User;

User* find_user_by_id(int id);
User* find_user_by_email(const char* email);
User* find_user_by_phone(const char* phone);

User* find_user(int id, const char* email, const char* phone) {
    // 按优先级查找用户
    if (User* user = find_user_by_id(id); user != NULL) {
        printf("Found user by ID\n");
        return user;
    } else if (User* user = find_user_by_email(email); user != NULL) {
        printf("Found user by email\n");
        return user;
    } else if (User* user = find_user_by_phone(phone); user != NULL) {
        printf("Found user by phone\n");
        return user;
    }

    printf("User not found\n");
    return NULL;
}

// 模拟查找函数
User database[] = {
    {1, "Alice", "alice@example.com"},
    {2, "Bob", "bob@example.com"},
    {3, "Charlie", "charlie@example.com"}
};

User* find_user_by_id(int id) {
    for (size_t i = 0; i < sizeof(database) / sizeof(database[0]); i++) {
        if (database[i].id == id) return &database[i];
    }
    return NULL;
}

User* find_user_by_email(const char* email) {
    for (size_t i = 0; i < sizeof(database) / sizeof(database[0]); i++) {
        if (strcmp(database[i].email, email) == 0) return &database[i];
    }
    return NULL;
}

User* find_user_by_phone(const char* phone) {
    (void)phone;  // 模拟：数据库中没有电话信息
    return NULL;
}

int main(void) {
    printf("=== Lookup Operations Demo ===\n\n");

    // 初始化哈希表
    HashTable* ht = hash_table_create();
    hash_table_put("timeout", 60);
    hash_table_put("retries", 5);

    // 配置查找演示
    printf("Config lookup:\n");
    get_config_value("timeout", ht);
    get_config_value("retries", ht);
    get_config_value("buffer_size", ht);  // 使用默认值
    get_config_value("timeout", ht);      // 从缓存获取

    printf("\nUser lookup:\n");
    User* u1 = find_user(1, NULL, NULL);
    if (u1) printf("  Found: %s\n", u1->name);

    User* u2 = find_user(999, "bob@example.com", NULL);
    if (u2) printf("  Found: %s\n", u2->name);

    User* u3 = find_user(999, "unknown@example.com", "1234567890");
    if (u3) printf("  Found: %s\n", u3->name);

    // 清理
    for (size_t i = 0; i < HASH_SIZE; i++) {
        HashEntry* e = ht->buckets[i];
        while (e) {
            HashEntry* next = e->next;
            free(e->key);
            free(e);
            e = next;
        }
    }
    free(ht->buckets);
    free(ht);

    return 0;
}
