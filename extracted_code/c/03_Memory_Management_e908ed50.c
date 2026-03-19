/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\03_Memory_Management.md
 * Line: 389
 * Language: c
 * Block ID: e908ed50
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void uninitialized() {
    int x;           // 未初始化！值不确定
    printf("%d\n", x);  // 未定义行为

    // 正确做法
    int y = 0;       // 明确初始化
    int *p = calloc(1, sizeof(int));  // calloc清零

    free(p);
}
