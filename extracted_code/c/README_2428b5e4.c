/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Practical_Verification_Cases\README.md
 * Line: 193
 * Language: c
 * Block ID: 2428b5e4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@ predicate is_valid_list{L}(struct list* l) =
  @   l == \null ||
  @   (\valid(l) && is_valid_list(l->next));
  @
  @ logic integer list_length{L}(struct list* l) =
  @   (l == \null) ? 0 : 1 + list_length(l->next);
  @
  @ predicate no_cycle{L}(struct list* l) =
  @   l == \null ||
  @   (\valid(l) && (l->next == \null || l->next != l) && no_cycle(l->next));
*/

/*@
  @ requires is_valid_list(lst);
  @ requires no_cycle(lst);
  @ assigns \nothing;
  @ ensures \result == list_length(lst);
*/
size_t list_size(const struct list* lst);
