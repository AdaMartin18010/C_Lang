/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 212
 * Language: c
 * Block ID: 6e17d91a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C2y 等价代码
if (FILE* fp = fopen("file.txt", "r"); fp != NULL) {
    // 使用 fp
    fclose(fp);
}
// fp 已超出作用域
