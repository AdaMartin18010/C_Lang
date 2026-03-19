/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\10_Formal_Verification.md
 * Line: 652
 * Language: c
 * Block ID: e4da392d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 分离逻辑基本概念:
// P * Q: P和Q描述的内存区域不相交
// emp: 空堆
// p ↦ v: p指向值v

// 链表节点
typedef struct node {
    int value;
    struct node *next;
} node;

// 链表谓词
/*@ inductive list predicate node *list;
    case nil:
      \forall node *l; l == \null ==> list(l, \Nil);
    case cons:
      \forall node *l, node *n, int v, list L;
      l != \null && l->value == v && l->next == n && list(n, L)
      ==> list(l, \Cons(v, L));
*/

// 链表长度
/*@ requires list(head, L);
    assigns \nothing;
    ensures list(head, L);
    ensures \result == \length(L);
*/
int list_length(node *head);
