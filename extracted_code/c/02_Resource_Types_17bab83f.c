/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\02_Resource_Types.md
 * Line: 568
 * Language: c
 * Block ID: 17bab83f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：循环引用导致内存泄漏
typedef struct Node {
    SharedPtr *next;  // 共享指针
    int data;
} Node;

void circular_reference(void) {
    SharedPtr *a = shared_ptr(malloc(sizeof(Node)), free);
    SharedPtr *b = shared_ptr(malloc(sizeof(Node)), free);

    ((Node *)a->ptr)->next = shared_ptr_clone(b);
    ((Node *)b->ptr)->next = shared_ptr_clone(a);

    // 循环引用，内存泄漏！
}

// 正确：使用弱引用或明确所有权
struct NodeWeak {
    struct NodeWeak *next;  // 裸指针，不拥有
    int data;
};
