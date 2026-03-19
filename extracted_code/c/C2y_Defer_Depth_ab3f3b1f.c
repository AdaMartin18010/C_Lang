/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2609
 * Language: c
 * Block ID: ab3f3b1f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 不要这样用
FILE *open_file_wrong(const char *path) {
    FILE *fp = fopen(path, "r");
    defer fclose(fp);  // 返回后文件会被关闭！
    return fp;
}

// ✅ 正确做法
FILE *open_file_correct(const char *path) {
    return fopen(path, "r");  // 调用者负责关闭
}
