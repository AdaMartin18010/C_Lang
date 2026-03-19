/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 298
 * Language: c
 * Block ID: 927cbfa4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void example(void) {
    int *p = malloc(sizeof(int) * 100);
    defer free(p);  // 单语句形式

    // 使用p...
    for (int i = 0; i < 100; i++) {
        p[i] = i;
    }
}  // free(p) 在这里自动执行
