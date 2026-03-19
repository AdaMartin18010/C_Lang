/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2596
 * Language: c
 * Block ID: 2457b80e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 不要这样用
FILE *fp = fopen("temp.txt", "w");
defer fclose(fp);

// 处理...

fclose(fp);  // 重复关闭！
fp = fopen("other.txt", "r");  // 重新赋值
