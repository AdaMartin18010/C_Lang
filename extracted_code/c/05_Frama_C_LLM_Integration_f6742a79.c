/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Frama_C_LLM_Integration.md
 * Line: 276
 * Language: c
 * Block ID: f6742a79
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 链表节点结构 */
struct Node {
    int value;
    struct Node *next;
};

/* 原始代码：链表长度计算 */
int list_length(struct Node *head) {
    int len = 0;
    struct Node *p = head;
    while (p != NULL) {
        len++;
        p = p->next;
    }
    return len;
}

/* LLM生成的ACSL规范（包含归纳谓词） */
/*@
  @ inductive valid_list{L}(struct Node *n) {
  @   case nil{L}:
  @     \valid(n) ==> n == NULL ==> valid_list(n);
  @   case cons{L}:
  @     \valid(n) ==> valid_list(n->next) ==> valid_list(n);
  @ }
  @
  @ logic integer list_len{L}(struct Node *n) =
  @   (n == NULL) ? 0 : 1 + list_len(n->next);
  @*/

/*@
  @ requires valid_list(head);
  @ ensures \result == list_len(head);
  @ assigns \nothing;
  @*/
int list_length(struct Node *head);
