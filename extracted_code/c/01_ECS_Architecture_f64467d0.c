/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_ECS_Architecture.md
 * Line: 1596
 * Language: c
 * Block ID: f64467d0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：链表遍历，严重指针追逐
typedef struct Node {
    Component data;
    struct Node* next;
} Node;

void process_list(Node* head) {
    for (Node* n = head; n != NULL; n = n->next) {
        process(&n->data);
        // 每次 next 都可能导致缓存未命中
        // 10000个节点 = 10000次可能的缓存未命中
    }
}

// ❌ 错误：多级指针解引用
void update_hierarchical(Entity** entities, int count) {
    for (int i = 0; i < count; i++) {
        Entity* e = entities[i];           // 一级解引用
        Component* c = e->components;       // 二级解引用
        Position* p = (Position*)c->data;   // 三级解引用
        p->x += 1.0f;                       // 四级解引用
        // 每次访问都可能触发4次缓存未命中！
    }
}

// ✅ 正确：扁平数组，单级访问
void update_flat(Position* positions, int count) {
    for (int i = 0; i < count; i++) {
        positions[i].x += 1.0f;  // 直接访问，缓存友好
    }
}
