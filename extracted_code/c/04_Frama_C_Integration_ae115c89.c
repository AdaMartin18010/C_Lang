/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\04_Frama_C_Integration.md
 * Line: 129
 * Language: c
 * Block ID: ae115c89
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 验证安全的链表操作
#include <stdlib.h>
#include <stddef.h>

typedef struct Node {
    int value;
    struct Node* next;
} Node;

/*@
  predicate list(struct Node* n, list<int> values) =
    n == \null ? values == nil :
    \exists list<int> tail; values == cons(n->value, tail) &&
    list(n->next, tail);
*/

/*@
  requires list(head, ?values);
  requires \valid(value);
  ensures list(\result, cons(*value, values));
  allocates \result;
*/
Node* list_prepend(Node* head, const int* value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) return head;

    new_node->value = *value;
    new_node->next = head;
    return new_node;
}

/*@
  requires list(head, ?values);
  frees head;
  ensures values == nil ==> \result == \null;
  ensures values != nil ==> \exists int v, list<int> tail;
    values == cons(v, tail) && list(\result, tail);
*/
Node* list_tail(Node* head) {
    if (head == NULL) return NULL;
    Node* next = head->next;
    free(head);
    return next;
}
