/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2701
 * Language: c
 * Block ID: 35598c39
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 反模式：在循环外使用defer
for (int i = 0; i < 100; i++) {
    FILE *fp = fopen(files[i], "r");
    // 处理...
}  // 没有关闭任何文件！
defer fclose(fp);  // 只关闭最后一个

// ✅ 正确做法：在循环内使用defer
for (int i = 0; i < 100; i++) {
    FILE *fp = fopen(files[i], "r");
    if (!fp) continue;
    defer fclose(fp);
    // 处理...
}
