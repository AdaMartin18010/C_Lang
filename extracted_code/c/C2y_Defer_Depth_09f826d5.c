/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 3073
 * Language: c
 * Block ID: 09f826d5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 原有代码
int old_function(void) {
    FILE *fp = fopen("test.txt", "r");
    if (!fp) return -1;

    int *buffer = malloc(1024);
    if (!buffer) {
        fclose(fp);  // 手动释放
        return -1;
    }

    // 处理...

    free(buffer);  // 手动释放
    fclose(fp);    // 手动释放
    return 0;
}
