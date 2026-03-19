/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_WP_Tutorial.md
 * Line: 874
 * Language: c
 * Block ID: 7aa2f9b2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// wp_case3_list_reverse.c
#include <stdlib.h>

typedef struct Node {
    int value;
    struct Node* next;
} Node;

/*@
  @predicate list(Node* n, list<int> values) =
    n == \null ? values == nil :
    \exists list<int> tail; values == cons(n->value, tail) &&
    n->next == \null ? tail == nil :
    list(n->next, tail);
*/

/*@
  @requires list(head, ?values);
  @assigns \nothing;
  @frees head;
  @ensures list(\result, reverse(values));
*/
Node* list_reverse(Node* head) {
    Node* prev = NULL;
    Node* current = head;

    /*@
      @loop invariant list(current, ?remaining);
      @loop invariant list(prev, ?reversed);
      @loop invariant reverse(values) == concat(reverse(remaining), reversed);
      @loop assigns current, prev, head;
    */
    while (current != NULL) {
        Node* next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    return prev;
}
