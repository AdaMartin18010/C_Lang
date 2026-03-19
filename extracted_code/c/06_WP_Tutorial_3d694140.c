/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_WP_Tutorial.md
 * Line: 343
 * Language: c
 * Block ID: 3d694140
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @ghost typedef struct {
    integer visited;
    integer comparisons;
  } SearchStats;
*/

/*@
  @requires n >= 0;
  @requires \valid_read(arr + (0..n-1));
  @assigns \nothing;
  @ensures 0 <= \result <= n;
*/
size_t linear_search(const int* arr, size_t n, int key) {
    /*@ ghost SearchStats stats = {0, 0}; */

    /*@
      @loop invariant 0 <= i <= n;
      @loop invariant stats.visited == i;
      @loop invariant stats.comparisons == i;
      @loop assigns i, stats;
      @loop variant n - i;
    */
    for (size_t i = 0; i < n; i++) {
        /*@ ghost stats.visited++; */
        /*@ ghost stats.comparisons++; */

        if (arr[i] == key) {
            return i;
        }
    }

    /*@ assert stats.visited == n; */
    return n;
}
