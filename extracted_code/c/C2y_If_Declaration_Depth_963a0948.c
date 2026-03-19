/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 341
 * Language: c
 * Block ID: 963a0948
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// else 分支中访问声明变量
if (FILE* fp = fopen("data.txt", "r"); fp != NULL) {
    // 成功打开，使用 fp
    process_file(fp);
    fclose(fp);
} else {
    // 失败时，fp 仍然可见（值为 NULL）
    // 可以用于错误报告
    fprintf(stderr, "Failed to open file, fp = %p\n", (void*)fp);
    // 但不能解引用！
}
// fp 超出作用域
