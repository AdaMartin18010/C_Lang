/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Eva_Tutorial.md
 * Line: 755
 * Language: c
 * Block ID: ab74f87a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// tutorial10_bounds.c
#define BUFFER_SIZE 100

// 越界访问
void risky_access(int arr[], int index) {
    arr[index] = 42;  // 如果index越界,崩溃!
}

// 安全检查版本
/*@
  @requires \valid(arr + (0..BUFFER_SIZE-1));
  @requires index >= 0 && index < BUFFER_SIZE;
  @assigns arr[index];
*/
void safe_access(int arr[BUFFER_SIZE], int index) {
    arr[index] = 42;
}

// 运行时检查版本
void checked_access(int arr[BUFFER_SIZE], int index) {
    if (index >= 0 && index < BUFFER_SIZE) {
        arr[index] = 42;
    }
}
