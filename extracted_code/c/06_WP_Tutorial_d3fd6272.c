/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_WP_Tutorial.md
 * Line: 244
 * Language: c
 * Block ID: d3fd6272
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @requires n >= 0;
  @requires \valid_read(arr + (0..n-1));
  @requires \valid(\result);
  @assigns \nothing;

  // 后置条件
  @ensures \result >= 0;
  @ensures \forall integer i; 0 <= i < n ==> \result >= arr[i];
  @ensures \exists integer i; 0 <= i < n && \result == arr[i];

  // 行为分支
  @behavior empty:
    @assumes n == 0;
    @ensures \result == 0;

  @behavior non_empty:
    @assumes n > 0;
    @ensures \exists integer i; 0 <= i < n && \result == arr[i];

  @complete behaviors empty, non_empty;
  @disjoint behaviors empty, non_empty;
*/
int find_max(const int* arr, int n);
