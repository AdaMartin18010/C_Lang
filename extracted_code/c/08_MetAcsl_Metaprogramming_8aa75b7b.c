/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\08_MetAcsl_Metaprogramming.md
 * Line: 288
 * Language: c
 * Block ID: 8aa75b7b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @meta match {
    // 匹配特定代码模式
    pattern: {
      "swap pattern": {
        match: "tmp = *a; *a = *b; *b = tmp",
        generate: "@requires \separated(a, b)"
      },
      
      "memset pattern": {
        match: "for(i=0; i<n; i++) arr[i] = value",
        generate: "@ensures \forall j; 0<=j<n ==> arr[j] == value"
      },
      
      "search pattern": {
        match: "for(... ) if (arr[i] == key) return i",
        generate: "@ensures \result >= 0 ==> arr[\result] == key"
      }
    }
  }
*/

// 应用模式匹配
void swap_ints(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}
// MetAcsl自动添加:
// @requires \separated(a, b);
