/*
 * Auto-generated from: 12_Practice_Exercises\10_Debugging_Puzzles.md
 * Line: 119
 * Language: c
 * Block ID: 955cd63a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void free_person(Person *p) {
    free(p->name);  // 先释放内部指针
    free(p);        // 再释放结构体
}
