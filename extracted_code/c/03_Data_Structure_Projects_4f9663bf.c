/*
 * Auto-generated from: 12_Practice_Exercises\03_Data_Structure_Projects.md
 * Line: 280
 * Language: c
 * Block ID: 4f9663bf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct SkipNode {
    int key;
    void *value;
    struct SkipNode **forward;  // 可变长度数组
} SkipNode;

typedef struct {
    SkipNode *header;
    int level;
    int max_level;
    double p;  // 晋升概率
    size_t size;
} SkipList;

SkipList* skiplist_create(int max_level, double p);
void skiplist_destroy(SkipList *sl);

bool skiplist_insert(SkipList *sl, int key, void *value);
void* skiplist_search(SkipList *sl, int key);
bool skiplist_remove(SkipList *sl, int key);
bool skiplist_contains(SkipList *sl, int key);
