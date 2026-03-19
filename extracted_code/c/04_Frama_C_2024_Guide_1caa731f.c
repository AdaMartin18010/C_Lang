/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\04_Frama_C_2024_Guide.md
 * Line: 486
 * Language: c
 * Block ID: 1caa731f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @requires n >= 0;
  @requires \valid_read(arr + (0..n-1));  // 读取范围
  @requires \valid(dst + (0..n-1));        // 写入范围
  @requires \separated(dst+(0..n-1), arr+(0..n-1));  // 不重叠
  @assigns dst[0..n-1];
  @ensures \forall integer i; 0 <= i < n ==> dst[i] == arr[i];
*/
void array_copy(int* dst, const int* arr, int n);
