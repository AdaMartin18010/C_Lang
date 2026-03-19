/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 112
 * Language: c
 * Block ID: 79187713
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 传统 C 代码的问题：变量作用域过大
void process_file(const char* filename) {
    FILE* fp = fopen(filename, "r");  // fp 的作用域延伸到函数结束
    if (fp == NULL) {
        perror("Failed to open file");
        return;
    }
    // ... 使用 fp ...
    fclose(fp);
    // fp 在这里仍然可见，可能被误用
}
