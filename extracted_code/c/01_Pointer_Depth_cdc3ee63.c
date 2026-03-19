/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Depth.md
 * Line: 281
 * Language: c
 * Block ID: cdc3ee63
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void foo(void) {
    int *p;          /* p 是野指针 - 未初始化 */
    *p = 10;         /* UB: 使用野指针 */
}
