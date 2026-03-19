/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\README.md
 * Line: 270
 * Language: c
 * Block ID: 116d2994
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 安全的数组访问
/*@
  requires 0 <= index < length;
  requires \valid_read(array + (0 .. length-1));
  assigns \nothing;
*/
int safe_access(int *array, int length, int index) {
    return array[index];
}
