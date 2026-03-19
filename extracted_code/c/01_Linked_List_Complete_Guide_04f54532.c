/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 1537
 * Language: c
 * Block ID: 04f54532
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 跳表 (Skip List): 概率性平衡数据结构
 * 平均时间复杂度: O(log n) 查找/插入/删除
 */

#define MAX_LEVEL 16
#define P 0.5

typedef struct SkipNode {
    int data;
    struct SkipNode* forward[MAX_LEVEL];  // 各层的前进指针
    int level;
} SkipNode;

typedef struct {
    SkipNode* header;
    int level;
} SkipList;

// 随机生成层级
int random_level() {
    int level = 1;
    while ((double)rand() / RAND_MAX < P && level < MAX_LEVEL) {
        level++;
    }
    return level;
}

// 初始化跳表
void skip_list_init(SkipList* list) {
    list->header = (SkipNode*)malloc(sizeof(SkipNode));
    for (int i = 0; i < MAX_LEVEL; i++) {
        list->header->forward[i] = nullptr;
    }
    list->header->level = MAX_LEVEL;
    list->level = 1;
}

// 查找
bool skip_list_search(SkipList* list, int target) {
    SkipNode* current = list->header;

    for (int i = list->level - 1; i >= 0; i--) {
        while (current->forward[i] != nullptr &&
               current->forward[i]->data < target) {
            current = current->forward[i];
        }
    }

    current = current->forward[0];
    return (current != nullptr && current->data == target);
}
