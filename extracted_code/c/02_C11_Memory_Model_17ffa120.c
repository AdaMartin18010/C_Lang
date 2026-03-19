/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\02_C11_Memory_Model.md
 * Line: 569
 * Language: c
 * Block ID: 17ffa120
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 问题：CAS可能受ABA问题影响
struct node {
    struct node *next;
    int data;
};

_Atomic(struct node *) head = NULL;

void push_bad(struct node *new_node) {
    struct node *old_head;
    do {
        old_head = atomic_load(&head);
        new_node->next = old_head;
    } while (!atomic_compare_exchange_weak(&head, &old_head, new_node));
}

// ✅ 解决：使用带标签的指针（ Tagged Pointer ）
#include <stdint.h>

typedef struct {
    struct node *ptr;
    uint64_t tag;
} tagged_ptr;

_Atomic(tagged_ptr) head_tagged;

void push_good(struct node *new_node) {
    tagged_ptr old, new;
    do {
        old = atomic_load(&head_tagged);
        new_node->next = old.ptr;
        new.ptr = new_node;
        new.tag = old.tag + 1;  // 递增标签
    } while (!atomic_compare_exchange_weak(&head_tagged, &old, new));
}
