/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 3095
 * Language: c
 * Block ID: f5fbc2db
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 迁移后
int new_function(void) {
    FILE *fp = fopen("test.txt", "r");
    if (!fp) return -1;
    defer fclose(fp);  // 立即注册释放

    int *buffer = malloc(1024);
    if (!buffer) return -1;  // fp自动关闭
    defer free(buffer);       // 立即注册释放

    // 处理...

    return 0;  // 自动释放所有资源
}
