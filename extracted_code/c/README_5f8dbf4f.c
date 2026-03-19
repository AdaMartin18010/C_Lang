/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Practical_Verification_Cases\README.md
 * Line: 310
 * Language: c
 * Block ID: 5f8dbf4f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define MAX_SIZE 100

struct stack {
    int data[MAX_SIZE];
    size_t top;
};

/*@
  @ predicate is_valid_stack(struct stack* s) =
  @   \valid(s) && s->top <= MAX_SIZE;
  @
  @ predicate is_empty(struct stack* s) = s->top == 0;
  @
  @ predicate is_full(struct stack* s) = s->top == MAX_SIZE;
*/

/*@
  @ requires \valid(s);
  @ assigns s->top;
  @ ensures is_empty(s);
*/
void stack_init(struct stack* s) {
    s->top = 0;
}

/*@
  @ requires is_valid_stack(s);
  @ requires !is_full(s);
  @ assigns s->data[s->top], s->top;
  @ ensures s->top == \old(s->top) + 1;
  @ ensures s->data[s->top-1] == value;
*/
void stack_push(struct stack* s, int value) {
    s->data[s->top] = value;
    s->top++;
}

/*@
  @ requires is_valid_stack(s);
  @ requires !is_empty(s);
  @ assigns \nothing;
  @ ensures \result == s->data[s->top-1];
*/
int stack_peek(const struct stack* s) {
    return s->data[s->top - 1];
}
