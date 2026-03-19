/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Eva_Tutorial.md
 * Line: 865
 * Language: c
 * Block ID: 4f2a9fb2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// tutorial13_linked_list.c
#include <stdlib.h>

typedef struct Node {
    int value;
    struct Node* next;
} Node;

/*@
  @requires \valid(list);
  @assigns \nothing;
*/
int list_length(Node* list) {
    int len = 0;
    Node* current = list;

    // Eva挑战: 无法证明终止性(可能存在环)
    // 需要额外的规范支持
    while (current != NULL) {
        len++;
        current = current->next;
    }

    return len;
}

// 更安全的版本(带长度限制)
/*@
  @requires max_depth >= 0;
  @assigns \nothing;
  @ensures \result >= 0;
  @ensures \result <= max_depth;
*/
int list_length_bounded(Node* list, int max_depth) {
    int len = 0;
    Node* current = list;

    // Eva可以分析此版本
    while (current != NULL && len < max_depth) {
        len++;
        current = current->next;
    }

    return len;
}
