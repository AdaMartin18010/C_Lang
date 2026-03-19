/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 3114
 * Language: c
 * Block ID: ddb13ed7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 混合使用defer和传统方式

void legacy_cleanup(void *resource) {
    // 旧版清理代码
    custom_release(resource);
}

void new_function(void) {
    void *resource = legacy_acquire();
    defer legacy_cleanup(resource);

    // 新代码使用defer...
    FILE *fp = fopen("new.txt", "r");
    defer fclose(fp);

    // 混合使用...
}
