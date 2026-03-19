/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_WP_Tutorial.md
 * Line: 312
 * Language: c
 * Block ID: de4abfef
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  // 指针有效性
  @predicate is_valid_array(int* a, integer n) =
    n >= 0 ==> \valid(a + (0..n-1));

  @predicate is_valid_read_array(int* a, integer n) =
    n >= 0 ==> \valid_read(a + (0..n-1));

  // 数组分离
  @predicate arrays_disjoint(int* a, int* b, integer n, integer m) =
    \separated(a + (0..n-1), b + (0..m-1));

  // 数组内容相等
  @predicate array_equal(int* a, int* b, integer n) =
    \forall integer i; 0 <= i < n ==> a[i] == b[i];
*/

/*@
  @requires is_valid_array(dst, n);
  @requires is_valid_read_array(src, n);
  @requires arrays_disjoint(dst, src, n, n);
  @assigns dst[0..n-1];
  @ensures array_equal(dst, \old(src), n);
  @ensures \result == dst;
*/
int* array_copy(int* dst, const int* src, size_t n);
