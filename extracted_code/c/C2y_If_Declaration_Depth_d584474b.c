/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 177
 * Language: c
 * Block ID: d584474b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C2y: 变量作用域精确控制
void process_file_c2y(const char* filename) {
    if (FILE* fp = fopen(filename, "r"); fp != NULL) {
        // fp 只在这里可见
        // ... 使用 fp ...
        fclose(fp);
    } else {
        perror("Failed to open file");
    }
    // fp 在这里不可见！
}
