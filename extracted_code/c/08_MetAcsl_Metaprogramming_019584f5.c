/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\08_MetAcsl_Metaprogramming.md
 * Line: 248
 * Language: c
 * Block ID: 019584f5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 定义模板
/*@
  @meta template array_function {
    @requires n >= 0;
    @requires \valid(arr + (0..n-1));
    @requires \initialized(arr + (0..n-1));
    @assigns arr[0..n-1];
    @ensures \forall integer i; 0 <= i < n ==> 
      arr[i] == \old(arr[i]) <op> <value>;
  }
*/

// 应用模板
/*@ @meta apply_template array_function with op="+", value="1" */
void increment_array(int* arr, int n) {
    for (int i = 0; i < n; i++) {
        arr[i]++;
    }
}

// 生成的规范:
/*@
  @requires n >= 0;
  @requires \valid(arr + (0..n-1));
  @requires \initialized(arr + (0..n-1));
  @assigns arr[0..n-1];
  @ensures \forall integer i; 0 <= i < n ==> 
    arr[i] == \old(arr[i]) + 1;
*/
