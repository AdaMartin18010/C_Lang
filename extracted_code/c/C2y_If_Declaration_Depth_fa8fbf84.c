/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 548
 * Language: c
 * Block ID: fa8fbf84
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 数据结构查找
typedef struct Node {
    int key;
    int value;
    struct Node* left;
    struct Node* right;
} Node;

int* find_value(Node* root, int key) {
    // 在树中查找
    if (Node* found = search_bst(root, key); found != NULL) {
        return &found->value;
    }

    // 在备用表中查找
    if (Node* found = search_hash_table(key); found != NULL) {
        return &found->value;
    }

    // 在缓存中查找
    if (Node* found = search_cache(key); found != NULL) {
        return &found->value;
    }

    return NULL;
}

// 字符串查找
const char* find_config(const char* key) {
    // 先查环境变量
    if (const char* env = getenv(key); env != NULL) {
        return env;
    }

    // 再查配置文件
    if (const char* cfg = config_file_get(key); cfg != NULL) {
        return cfg;
    }

    // 最后使用默认值
    if (const char* def = default_values_get(key); def != NULL) {
        return def;
    }

    return NULL;
}
