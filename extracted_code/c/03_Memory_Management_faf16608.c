/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\03_Memory_Management.md
 * Line: 313
 * Language: c
 * Block ID: faf16608
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void use_after_free() {
    int *p = malloc(sizeof(int));
    *p = 42;
    free(p);

    // 危险！p现在是悬空指针
    printf("%d\n", *p);  // Use-after-free! 未定义行为

    // 正确做法
    p = NULL;  // 释放后置空
}
