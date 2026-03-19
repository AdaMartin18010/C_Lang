/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\04_Frama_C_2024_Guide.md
 * Line: 623
 * Language: c
 * Block ID: 48302cad
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @predicate is_valid_range(int* a, integer n) =
    n >= 0 ==> \valid(a + (0..n-1));

  @predicate is_valid_read_range(int* a, integer n) =
    n >= 0 ==> \valid_read(a + (0..n-1));

  @predicate arrays_separated(int* a, int* b, integer n, integer m) =
    \separated(a+(0..n-1), b+(0..m-1));
*/

/*@
  @requires is_valid_range(dst, n);
  @requires is_valid_read_range(src, n);
  @requires arrays_separated(dst, src, n, n);
  @assigns dst[0..n-1];
  @ensures \forall integer i; 0 <= i < n ==> dst[i] == \old(src[i]);
  @ensures \result == dst;
*/
int* safe_memcpy(int* dst, const int* src, size_t n);
